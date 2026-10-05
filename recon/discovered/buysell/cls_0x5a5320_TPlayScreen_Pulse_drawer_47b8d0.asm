// Disassembly: TPlayScreen Pulse 0x0047b4d0, drawer mode 3 opening: TextBar (0x0065c5d0) Hide
// 0x0054c9c0; 0x0065a8c0, the dialog pane and the side tabs shown (+0x48/+0x4c = 0, slot 10);
// mode = 3; AddPane(0x0065a3b8, -1); the shop shown; its +0x50 = 1; PlayScreen +0x6b8 = 1.
---- 0047b8d0
0047b8bd  JMP 0x0047bbc9
0047b8c2  CMP EAX,0x3
0047b8c5  JNZ 0x0047b96b
0047b8cb  MOV ECX,0x65c5d0
0047b8d0  CALL 0x0054c9c0
0047b8d5  MOV EDX,dword ptr [0x0065a8c0]
0047b8db  MOV ECX,0x65a8c0
0047b8e0  MOV dword ptr [0x0065a908],EBX
0047b8e6  MOV dword ptr [0x0065a90c],EBX
0047b8ec  CALL dword ptr [EDX + 0x28]
0047b8ef  MOV EAX,[0x00667cc8]
0047b8f4  MOV ECX,0x667cc8
0047b8f9  MOV dword ptr [0x00667d10],EBX
0047b8ff  MOV dword ptr [0x00667d14],EBX
0047b905  CALL dword ptr [EAX + 0x28]
0047b908  MOV EDX,dword ptr [0x0065be50]
0047b90e  MOV ECX,0x65be50
0047b913  MOV dword ptr [0x0065be98],EBX
0047b919  MOV dword ptr [0x0065be9c],EBX
0047b91f  CALL dword ptr [EDX + 0x28]
0047b922  PUSH -0x1
0047b924  PUSH 0x65a3b8
0047b929  MOV ECX,ESI
0047b92b  MOV dword ptr [ESI + 0x6c0],0x3
0047b935  CALL 0x0048ed90
0047b93a  MOV EAX,[0x0065a3b8]
0047b93f  MOV ECX,0x65a3b8
0047b944  MOV dword ptr [0x0065a400],EBX
0047b94a  MOV dword ptr [0x0065a404],EBX
0047b950  CALL dword ptr [EAX + 0x28]
0047b953  MOV dword ptr [0x0065a408],EDI
0047b959  MOV dword ptr [ESI + 0x6b8],EDI
0047b95f  MOV byte ptr [0x00667cbc],0x1
0047b966  JMP 0x0047bbc9
0047b96b  MOV EDX,dword ptr [0x0065c5d0]
0047b971  MOV ECX,0x65c5d0
0047b976  MOV dword ptr [0x0065c618],EBX
0047b97c  MOV dword ptr [0x0065c61c],EBX
0047b982  CALL dword ptr [EDX + 0x28]
0047b985  MOV EAX,[0x0065a8c0]
0047b98a  MOV ECX,0x65a8c0
0047b98f  MOV dword ptr [0x0065a908],EBX
0047b995  MOV dword ptr [0x0065a90c],EBX
0047b99b  CALL dword ptr [EAX + 0x28]
0047b99e  MOV EDX,dword ptr [0x00667cc8]
