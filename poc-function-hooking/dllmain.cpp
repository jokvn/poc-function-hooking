#include <Windows.h>
#include <iostream>

bool placeHook(void* hookAddress, void* hookFunction, int length)
{
	//if bytes of our hook is less than 5 return, because we need
	//atleast 5 bytes for our jmp instruction
	if(length < 5)
		return false;

	//Store old protection, so we can set it to it's initial sate afte we overwrote the instuctions
	DWORD currentProtection;
	VirtualProtect(hookAddress, length, PAGE_EXECUTE_READWRITE, &currentProtection);

	
	//jmp instruction itself has a size of 5 bytes so subtract it from our
	//total "offset". 
	DWORD relativeAddress = ((DWORD)hookFunction - (DWORD)hookAddress) - 5;

	//0xE9 is the opcode for a relative jump
	//replace instruction where we want to place our hook with jmp
	*(BYTE*)hookAddress = 0xE9;
	*(DWORD*)((DWORD)hookAddress + 1) = relativeAddress;

	//set protection to initial protection
	DWORD oldProtection;
	VirtualProtect(hookAddress, length, currentProtection, &oldProtection);

	return true;

}

DWORD jmpBackAddress;

//We don't want prologue and epilogue assembly code therefore we use a naked declspec function
//This function is our hook function
_declspec(naked) void hookFunction() 
{
	_asm
	{
		mov eax, 999
		jmp [jmpBackAddress]
	}
}


DWORD WINAPI mainThread(LPVOID params)
{
	/*
	.text:00401003 B802000000      mov     eax, 2
				   ^
				   This instruction takes up 5 bytes,
				   so our hook fits perfectly. We don't
				   need to overwrite bytes from the next
				   instruction, and there are no left out
				   bytes.
	*/
	int hookLength = 5;

	/*
	"poc-function-hooking-dummy-process.exe"+3334

	or

	00F41003  |. B8 02000000    MOV EAX,2

	*/
	DWORD hookAddress = 0xF41003;

	//our jump back address needs to be after our hook
	jmpBackAddress = hookAddress + hookLength;

	/*
	Place our hook.
	After injection into our dummy process, we can see our jmp instruction:

	00F41000 >/$ 55             PUSH EBP
	00F41001  |. 8BEC           MOV EBP,ESP
	00F41003  |.-E9 F8FF8056    JMP poc-fu_1.hookFunction
	00F41008  |. 5D             POP EBP
	00F41009  \. C3             RETN

	*/
	if (placeHook((void*)hookAddress, hookFunction, hookLength))
	{
		MessageBoxA(0, "Injected!", "Success", 0);
	}

	while (!(GetAsyncKeyState(VK_ESCAPE)))
	{
		Sleep(200);
	}

	FreeLibraryAndExitThread((HMODULE)params, 0);

	return 0;
}


BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, mainThread, hModule, 0, 0);
		break;
	}

	return TRUE;
}