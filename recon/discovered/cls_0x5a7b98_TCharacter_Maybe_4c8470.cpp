// FUN_004c8470_TCharacter_ResolvePivot @ 004c8470 size=131

undefined4 __thiscall FUN_004c8470_TCharacter_ResolvePivot(int *param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  
  param_1[0x2d] = 0;
  iVar2 = FUN_0059a530_stricmp(param_2 + 4,param_1[0x38] + 4);
  if (iVar2 == 0) {
    uVar1 = *(uint *)(param_2 + 0x2c);
    if (*(byte *)((int)param_1 + 0x36) == uVar1) {
      (**(code **)(*param_1 + 0x208))(0,0);
      return 0;
    }
    FUN_004c5ad0_AdvanceAngles(uVar1,uVar1,*(undefined4 *)(param_2 + 0x34));
  }
  else if (param_1[0x20] != 0) {
    (**(code **)(*param_1 + 0x208))(0,0);
    return 0;
  }
  return 2;
}


