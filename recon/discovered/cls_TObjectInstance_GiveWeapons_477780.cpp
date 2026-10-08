// FUN_00477780 @ 00477780 size=189

/* WARNING: Removing unreachable block (ram,0x004777e4) */
/* WARNING: Removing unreachable block (ram,0x004777f1) */
/* WARNING: Removing unreachable block (ram,0x004777f7) */
/* WARNING: Removing unreachable block (ram,0x004777fd) */
/* WARNING: Removing unreachable block (ram,0x00477810) */
/* WARNING: Removing unreachable block (ram,0x00477816) */
/* WARNING: Removing unreachable block (ram,0x004777de) */
/* WARNING: Removing unreachable block (ram,0x00477822) */

void FUN_00477780(undefined4 param_1)

{
  int iVar1;
  
  iVar1 = FUN_00451d70(param_1,1,0);
  if (iVar1 == 0) {
    FUN_00481c10(s_Unable_to_find_object__s_to_give_005d5300,param_1);
  }
  FUN_0046dfb0();
  return;
}



/* Disassembly (Ghidra dropped the inventory walk from the decompile):
   00477780  SUB ESP,0x1c  
   00477783  PUSH EBX  
   00477784  PUSH EBP  
   00477785  PUSH ESI  
   00477786  XOR EBP,EBP  
   00477788  PUSH EDI  
   00477789  MOV EDI,dword ptr [ESP + 0x30]  
   0047778d  MOV ESI,ECX  
   0047778f  PUSH EBP  
   00477790  PUSH 0x1  
   00477792  PUSH EDI  
   00477793  MOV ECX,0x6668d8  
   00477798  CALL 0x00451d70  
   0047779d  MOV EBX,EAX  
   0047779f  CMP EBX,EBP  
   004777a1  JNZ 0x004777b1  
   004777a3  PUSH EDI  
   004777a4  PUSH 0x5d5300  
   004777a9  CALL 0x00481c10  
   004777ae  ADD ESP,0x8  
   004777b1  LEA ECX,[ESP + 0x10]  
   004777b5  MOV dword ptr [ESP + 0x10],EBP  
   004777b9  MOV dword ptr [ESP + 0x18],ESI  
   004777bd  MOV dword ptr [ESP + 0x14],ESI  
   004777c1  MOV dword ptr [ESP + 0x20],EBP  
   004777c5  MOV dword ptr [ESP + 0x1c],EBP  
   004777c9  MOV dword ptr [ESP + 0x24],EBP  
   004777cd  MOV dword ptr [ESP + 0x28],EBP  
   004777d1  CALL 0x0046dfb0  
   004777d6  MOV ECX,dword ptr [ESP + 0x28]  
   004777da  CMP ECX,EBP  
   004777dc  JZ 0x00477833  
   004777de  CMP ECX,EBP  
   004777e0  MOV ESI,ECX  
   004777e2  JZ 0x00477822  
   004777e4  MOV AX,word ptr [ECX + 0x4]  
   004777e8  LEA EDI,[ECX + 0x4]  
   004777eb  CMP AX,0x1  
   004777ef  JZ 0x004777fd  
   004777f1  CMP AX,0x17  
   004777f5  JZ 0x004777fd  
   004777f7  CMP AX,0x15  
   004777fb  JNZ 0x00477810  
   004777fd  MOV EAX,dword ptr [ECX]  
   004777ff  CALL dword ptr [EAX + 0x60]  
   00477802  MOV EDX,dword ptr [EBX]  
   00477804  PUSH -0x1  
   00477806  PUSH ESI  
   00477807  MOV ECX,EBX  
   00477809  CALL dword ptr [EDX + 0x58]  
   0047780c  DEC dword ptr [ESP + 0x24]  
   00477810  CMP word ptr [EDI],0x11  
   00477814  JNZ 0x00477822  
   00477816  MOV ECX,dword ptr [ESP + 0x30]  
   0047781a  MOV EAX,dword ptr [ESI]  
   0047781c  PUSH ECX  
   0047781d  MOV ECX,ESI  
   0047781f  CALL dword ptr [EAX + 0x68]  
   00477822  LEA ECX,[ESP + 0x10]  
   00477826  CALL 0x0046dfb0  
   0047782b  MOV ECX,dword ptr [ESP + 0x28]  
   0047782f  CMP ECX,EBP  
   00477831  JNZ 0x004777de  
   00477833  POP EDI  
   00477834  POP ESI  
   00477835  POP EBP  
   00477836  POP EBX  
   00477837  ADD ESP,0x1c  
   0047783a  RET 0x4  
   00477840  PUSH ESI  
   00477841  MOV ESI,ECX  
*/
