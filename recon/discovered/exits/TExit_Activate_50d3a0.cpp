// TExit_Activate @ 0x0050d3a0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x248: Activate(user, flag); disassembly appended
// FUN_0050d3a0 @ 0050d3a0 size=361

undefined4 __thiscall FUN_0050d3a0(int *param_1,int *param_2,int param_3)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  int iStack_1c;
  
  if ((DAT_0066829c != 0) && (DAT_0067682c == 0)) {
    return 0;
  }
  if (param_2 == (int *)0x0) {
    param_2 = DAT_00667fcc;
  }
  iStack_1c = 0x50d3dd;
  iVar2 = (**(code **)(*param_1 + 0x208))();
  if (iVar2 != 0) {
    return 0;
  }
  if ((param_1[0x21] != 0) && (param_3 == 0)) {
    iStack_1c = param_3;
    FUN_00492640(6,0,0,param_2,&DAT_005e17d8,0);
    return 1;
  }
  iStack_1c = DAT_0066d1cc;
  iVar2 = (**(code **)(*param_1 + 0xdc))();
  if ((iVar2 == 0) && (param_3 == 0)) {
    return 0;
  }
  iVar2 = param_1[0xe];
  puVar1 = DAT_0066d1c4;
  do {
    if (puVar1 == (undefined4 *)0x0) {
LAB_0050d463:
      if (param_1[0x21] != 0) {
        return 1;
      }
      return 0;
    }
    iVar3 = FUN_0059a530(*puVar1,iVar2);
    if (iVar3 == 0) {
      if (puVar1 != (undefined4 *)0x0) {
        if (param_2 == (int *)(-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4)) {
          DAT_006669b0 = DAT_006669b0 | 8;
        }
        FUN_004d4790(0);
        (**(code **)(*param_2 + 8))(&stack0xfffffff0,puVar1[4],0);
        if (DAT_0066829c == 0) {
          return 1;
        }
        if (DAT_0067682c == 0) {
          return 1;
        }
        FUN_00586bb0(param_2,&iStack_1c,puVar1[4]);
        return 1;
      }
      goto LAB_0050d463;
    }
    puVar1 = (undefined4 *)puVar1[8];
  } while( true );
}



// ---- Disassembly 0x0050d3a0.. (first 130 instructions) ----
// ---- 0050d3a0
// 0050d3a0  MOV EAX,[0x0066829c]
// 0050d3a5  SUB ESP,0xc
// 0050d3a8  TEST EAX,EAX
// 0050d3aa  PUSH EBX
// 0050d3ab  PUSH ESI
// 0050d3ac  PUSH EDI
// 0050d3ad  MOV EBX,ECX
// 0050d3af  JZ 0x0050d3c5
// 0050d3b1  MOV EAX,[0x0067682c]
// 0050d3b6  TEST EAX,EAX
// 0050d3b8  JNZ 0x0050d3c5
// 0050d3ba  POP EDI
// 0050d3bb  POP ESI
// 0050d3bc  XOR EAX,EAX
// 0050d3be  POP EBX
// 0050d3bf  ADD ESP,0xc
// 0050d3c2  RET 0x8
// 0050d3c5  MOV EDI,dword ptr [ESP + 0x1c]
// 0050d3c9  TEST EDI,EDI
// 0050d3cb  JNZ 0x0050d3d3
// 0050d3cd  MOV EDI,dword ptr [0x00667fcc]
// 0050d3d3  MOV EAX,dword ptr [EBX]
// 0050d3d5  MOV ECX,EBX
// 0050d3d7  CALL dword ptr [EAX + 0x208]
// 0050d3dd  TEST EAX,EAX
// 0050d3df  JZ 0x0050d3ec
// 0050d3e1  POP EDI
// 0050d3e2  POP ESI
// 0050d3e3  XOR EAX,EAX
// 0050d3e5  POP EBX
// 0050d3e6  ADD ESP,0xc
// 0050d3e9  RET 0x8
// 0050d3ec  MOV ECX,dword ptr [EBX + 0x84]
// 0050d3f2  MOV ESI,dword ptr [ESP + 0x20]
// 0050d3f6  TEST ECX,ECX
// 0050d3f8  JZ 0x0050d41d
// 0050d3fa  TEST ESI,ESI
// 0050d3fc  JNZ 0x0050d41d
// 0050d3fe  PUSH ESI
// 0050d3ff  PUSH ESI
// 0050d400  PUSH 0x5e17d8
// 0050d405  PUSH EDI
// 0050d406  PUSH ESI
// 0050d407  PUSH ESI
// 0050d408  PUSH 0x6
// 0050d40a  CALL 0x00492640
// 0050d40f  POP EDI
// 0050d410  POP ESI
// 0050d411  MOV EAX,0x1
// 0050d416  POP EBX
// 0050d417  ADD ESP,0xc
// 0050d41a  RET 0x8
// 0050d41d  MOV EAX,[0x0066d1cc]
// 0050d422  MOV EDX,dword ptr [EBX]
// 0050d424  PUSH EAX
// 0050d425  MOV ECX,EBX
// 0050d427  CALL dword ptr [EDX + 0xdc]
// 0050d42d  TEST EAX,EAX
// 0050d42f  JNZ 0x0050d43e
// 0050d431  TEST ESI,ESI
// 0050d433  JNZ 0x0050d43e
// 0050d435  POP EDI
// 0050d436  POP ESI
// 0050d437  POP EBX
// 0050d438  ADD ESP,0xc
// 0050d43b  RET 0x8
// 0050d43e  MOV ESI,dword ptr [0x0066d1c4]
// 0050d444  PUSH EBP
// 0050d445  MOV EBP,dword ptr [EBX + 0x38]
// 0050d448  TEST ESI,ESI
// 0050d44a  JZ 0x0050d463
// 0050d44c  MOV ECX,dword ptr [ESI]
// 0050d44e  PUSH EBP
// 0050d44f  PUSH ECX
// 0050d450  CALL 0x0059a530
// 0050d455  ADD ESP,0x8
// 0050d458  TEST EAX,EAX
// 0050d45a  JZ 0x0050d47d
// 0050d45c  MOV ESI,dword ptr [ESI + 0x20]
// 0050d45f  TEST ESI,ESI
// 0050d461  JNZ 0x0050d44c
// 0050d463  MOV EAX,dword ptr [EBX + 0x84]
// 0050d469  TEST EAX,EAX
// 0050d46b  JNZ 0x0050d4fa
// 0050d471  POP EBP
// 0050d472  POP EDI
// 0050d473  POP ESI
// 0050d474  XOR EAX,EAX
// 0050d476  POP EBX
// 0050d477  ADD ESP,0xc
// 0050d47a  RET 0x8
// 0050d47d  TEST ESI,ESI
// 0050d47f  JZ 0x0050d463
// 0050d481  MOV EDX,dword ptr [ESI + 0x4]
// 0050d484  MOV dword ptr [ESP + 0x10],EDX
// 0050d488  MOV EAX,dword ptr [ESI + 0x8]
// 0050d48b  MOV EDX,dword ptr [0x006669b4]
// 0050d491  MOV dword ptr [ESP + 0x14],EAX
// 0050d495  MOV EAX,[0x006669b0]
// 0050d49a  MOV ECX,dword ptr [ESI + 0xc]
// 0050d49d  AND AL,0x1
// 0050d49f  MOV dword ptr [ESP + 0x18],ECX
// 0050d4a3  NEG AL
// 0050d4a5  SBB EAX,EAX
// 0050d4a7  AND EAX,EDX
// 0050d4a9  CMP EDI,EAX
// 0050d4ab  JNZ 0x0050d4b9
// 0050d4ad  MOV EAX,[0x006669b0]
// 0050d4b2  OR AL,0x8
// 0050d4b4  MOV [0x006669b0],EAX
// 0050d4b9  PUSH 0x0
// 0050d4bb  MOV ECX,EDI
// 0050d4bd  CALL 0x004d4790
// 0050d4c2  MOV EAX,dword ptr [ESI + 0x10]
// 0050d4c5  MOV EDX,dword ptr [EDI]
// 0050d4c7  PUSH 0x0
// 0050d4c9  LEA ECX,[ESP + 0x14]
// 0050d4cd  PUSH EAX
// 0050d4ce  PUSH ECX
// 0050d4cf  MOV ECX,EDI
// 0050d4d1  CALL dword ptr [EDX + 0x8]
// 0050d4d4  MOV EAX,[0x0066829c]
// 0050d4d9  TEST EAX,EAX
// 0050d4db  JZ 0x0050d4fa
// 0050d4dd  MOV EAX,[0x0067682c]
// 0050d4e2  TEST EAX,EAX
// 0050d4e4  JZ 0x0050d4fa
// 0050d4e6  MOV EDX,dword ptr [ESI + 0x10]
// 0050d4e9  LEA EAX,[ESP + 0x10]
// 0050d4ed  PUSH EDX
// 0050d4ee  PUSH EAX
