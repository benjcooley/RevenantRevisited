// Disassembly: the tail of BuildItem 0x0052da90: the icon's imagery (0x00446b10(type+8, 1)),
// then in sell mode (flags & 2) price = __ftol(Value * float[0x005aedb0] = 0.3f).
---- 0052ef90
0052ef8f  JMP 0x0052ef97
0052ef91  MOV dword ptr [EBP + 0x34],ECX
0052ef94  MOV dword ptr [EBP],ECX
0052ef97  MOV ESI,dword ptr [EBX + 0x34]
0052ef9a  TEST ESI,ESI
0052ef9c  JZ 0x0052efb7
0052ef9e  MOV EAX,dword ptr [ESP + 0x1c]
0052efa2  MOV ECX,dword ptr [EBX + 0x24]
0052efa5  CMP EAX,ECX
0052efa7  JNC 0x0052efb7
0052efa9  MOV EDI,dword ptr [ESI + EAX*0x4]
0052efac  XOR ECX,ECX
0052efae  TEST EDI,EDI
0052efb0  SETNZ CL
0052efb3  TEST ECX,ECX
0052efb5  JNZ 0x0052efbb
0052efb7  XOR EAX,EAX
0052efb9  JMP 0x0052efc8
0052efbb  MOV ECX,dword ptr [EBX + 0x34]
0052efbe  MOV EAX,dword ptr [ECX + EAX*0x4]
0052efc1  TEST EAX,EAX
0052efc3  JNZ 0x0052efc8
0052efc5  MOV EAX,dword ptr [EBX + 0x38]
0052efc8  MOV EDX,dword ptr [EAX + 0x8]
0052efcb  PUSH 0x1
0052efcd  PUSH EDX
0052efce  CALL 0x00446b10
0052efd3  MOV dword ptr [EBP + 0x40],EAX
0052efd6  MOV AL,byte ptr [ESP + 0xa0]
0052efdd  ADD ESP,0x8
0052efe0  TEST AL,0x2
0052efe2  JZ 0x0052eff5
0052efe4  FILD dword ptr [EBP + 0x38]
0052efe7  FMUL float ptr [0x005aedb0]
0052efed  CALL 0x0058a840
0052eff2  MOV dword ptr [EBP + 0x38],EAX
0052eff5  POP EDI
0052eff6  POP ESI
0052eff7  POP EBP
0052eff8  MOV EAX,0x1
0052effd  POP EBX
0052effe  ADD ESP,0x80
0052f004  RET 0xc
