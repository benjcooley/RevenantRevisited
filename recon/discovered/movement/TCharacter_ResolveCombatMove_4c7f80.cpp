// TCharacter_ResolveCombatMove @ 0x004c7f80 (vtable slot 0x324) -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/COMMAND_SYSTEM.md §6.5)
// Combat-mode walk: arrival only with a pick-up pending (+0x288) and flag 0x1000; then slot 0x328.
// FUN_004c7f80 @ 004c7f80 size=313

undefined4 __thiscall FUN_004c7f80(int *param_1,int param_2,uint param_3)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  if (((param_3 & 2) != 0) || ((*(uint *)(param_2 + 0x60) & 0x100) != 0)) {
    param_1[0x2d] = 0;
    (**(code **)(*param_1 + 0x218))(param_1[0x38],0,0);
    return 0;
  }
  if (param_1[0x37] == param_1[0x38]) {
    (**(code **)(*param_1 + 0x208))(param_1[0x36],0);
  }
  if (((param_1[0xa2] != 0) && ((*(uint *)(param_2 + 0x60) & 0x1000) != 0)) &&
     ((iVar2 = *(int *)(param_2 + 0x38), iVar2 != 0 ||
      ((*(int *)(param_2 + 0x3c) != 0 || (*(int *)(param_2 + 0x40) != 0)))))) {
    iVar4 = param_1[4] - iVar2;
    if (iVar4 < 0) {
      iVar4 = iVar2 - param_1[4];
    }
    iVar2 = param_1[5] - *(int *)(param_2 + 0x3c);
    if (iVar2 < 0) {
      iVar2 = *(int *)(param_2 + 0x3c) - param_1[5];
    }
    iVar3 = iVar4;
    if (iVar2 <= iVar4) {
      iVar3 = iVar2;
    }
    if ((iVar2 - (iVar3 >> 1)) + iVar4 < 8) {
      iVar2 = *param_1;
      *(int *)(param_2 + 0x40) = param_1[6];
      (**(code **)(iVar2 + 0xc))(param_2 + 0x38);
      iVar2 = param_1[0xa2];
      *(uint *)(param_2 + 0x60) = *(uint *)(param_2 + 0x60) & 0xffffefff | 0x40;
      FUN_004cfef0(iVar2,0);
      param_1[0xa2] = 0;
      return 0;
    }
    uVar1 = FUN_0046dc60(param_1 + 4,param_2 + 0x38);
    *(undefined4 *)(param_2 + 0x30) = uVar1;
  }
  uVar1 = (**(code **)(*param_1 + 0x328))(param_2,param_3);
  return uVar1;
}


