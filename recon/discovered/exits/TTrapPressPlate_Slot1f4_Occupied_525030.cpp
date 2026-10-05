// TTrapPressPlate_Slot1f4_Occupied @ 0x00525030 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// TRAP-class slot 0x1f4: someone stands on the plate
// FUN_00525030 @ 00525030 size=429

undefined4 __fastcall FUN_00525030(int *param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int unaff_EDI;
  undefined1 auStack_58 [4];
  int *piStack_54;
  undefined1 auStack_50 [4];
  undefined1 auStack_4c [76];
  
  piVar1 = (int *)FUN_0046e8a0();
  (**(code **)(*piVar1 + 0xb4))((short)param_1[3],auStack_50,auStack_4c,auStack_58);
  piVar1 = (int *)FUN_0046e8a0();
  iVar2 = (**(code **)(*piVar1 + 0xbc))((short)param_1[3]);
  piVar1 = (int *)FUN_0046e8a0();
  iVar3 = (**(code **)(*piVar1 + 0xc0))((short)param_1[3]);
  if ((param_1[0x36] == 0) || (param_1[0x3e] != 0)) {
    iVar5 = 0xffff;
    FUN_0044cf80(0,0,2,0,0xffffffff);
    piVar1 = piStack_54;
    while (piVar1 != (int *)0x0) {
      piStack_54 = piVar1;
      iVar4 = (**(code **)(*param_1 + 4))(piVar1);
      if (((iVar4 < iVar5) && (iVar4 = (**(code **)(*piVar1 + 0x1c0))(), 0 < iVar4)) &&
         (piVar1[0x19] == 0)) {
        iVar5 = (**(code **)(*param_1 + 4))(piVar1);
        param_1[0x38] = (int)piVar1;
      }
      FUN_0044d080();
      piVar1 = piStack_54;
    }
    iVar5 = param_1[0x38];
    if (iVar5 != 0) {
      iVar2 = (iVar2 * 0x10 - param_1[4]) + *(int *)(iVar5 + 0x10);
      iVar5 = iVar3 * 0x10 + (*(int *)(iVar5 + 0x14) - param_1[5]);
      iVar4 = (int)(iVar2 + (iVar2 >> 0x1f & 0xfU)) >> 4;
      iVar2 = (int)(iVar5 + (iVar5 >> 0x1f & 0xfU)) >> 4;
      piVar1 = (int *)FUN_0046e8a0();
      (**(code **)(*piVar1 + 0xc4))((short)param_1[3]);
      if (((-1 < iVar4) && (-1 < iVar2)) && ((iVar4 < iVar3 && (iVar2 < unaff_EDI)))) {
        if (param_1[0x3e] == 0) {
          param_1[0x36] = 1;
        }
        param_1[0x3e] = 1;
        return 1;
      }
      if (param_1[0x3e] != 0) {
        (**(code **)(*param_1 + 0x18))(0);
        param_1[0x3e] = 0;
      }
    }
  }
  return 0;
}


