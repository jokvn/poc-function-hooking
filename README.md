# poc-function-hooking

Proof of concept for function hooking.

The goal is to redirect program flow in a function, to our own function, to change the logic.
Therefore we override the desired instruction in this function with a jump instruction to our function address.
Afterwards we jump back to continue program flow.

This dummy program contains a function, which returns the value `2` and we want it to return `999`.
Therefore we overrite the `MOV` instruction with a `JMP` instruction to our function in our module.

Target function before injection:

```
  00F41000 >/$ 55             PUSH EBP
  00F41001  |. 8BEC           MOV EBP,ESP
  00F41003  |. B8 02000000    MOV EAX,2                   <-- this is where we want to place out hook
  00F41008  |. 5D             POP EBP
  00F41009  \. C3             RETN
```

after injection:

```
	00F41000 >/$ 55             PUSH EBP
	00F41001  |. 8BEC           MOV EBP,ESP
	00F41003  |.-E9 F8FF8056    JMP poc-fu_1.hookFunction
	00F41008  |. 5D             POP EBP
	00F41009  \. C3             RETN
```

After the hook was placed, we jump back to the next instruction after our jump to continue program flow.

```
57761000 > B8 E7030000      MOV EAX,3E7
57761005  -FF25 88337657    JMP DWORD PTR DS:[jmpBackAddress]
```

```
	00F41000 >/$ 55             PUSH EBP
	00F41001  |. 8BEC           MOV EBP,ESP
	00F41003  |.-E9 F8FF8056    JMP poc-fu_1.hookFunction
	00F41008  |. 5D             POP EBP                      <-- jump back to here
	00F41009  \. C3             RETN
```

Disassembler used was `OllyDbg`.
