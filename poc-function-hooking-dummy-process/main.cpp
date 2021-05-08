#include <Windows.h>
#include <iostream>

/*
00F41000 >/$ 55             PUSH EBP
00F41001  |. 8BEC           MOV EBP,ESP
00F41003  |. B8 02000000    MOV EAX,2    <-- this is where we want to place out hook
00F41008  |. 5D             POP EBP
00F41009  \. C3             RETN
*/
int functionToHook()
{
	return 1 + 1;
}

int main()
{
	while (!(GetAsyncKeyState(VK_ESCAPE) & 1))
	{
		std::cout << functionToHook() << std::endl;
		Sleep(100);
	}
}