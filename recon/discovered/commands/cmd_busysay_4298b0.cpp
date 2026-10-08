// FUN_004298b0 @ 004298b0 size=95

undefined4 FUN_004298b0(undefined4 param_1,int param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  if (param_4 != 0) {
    if (*(int *)(param_2 + 0x10) == 2) {
      uVar2 = *(undefined4 *)(param_2 + 0x28);
      uVar3 = 0;
LAB_004298f3:
      FUN_00494530(uVar2,uVar3);
      FUN_00479580();
      return 0;
    }
    if (*(int *)(param_2 + 0x10) == 4) {
      iVar1 = FUN_0049d6d0(*(undefined4 *)(param_2 + 0x28));
      if (-1 < iVar1) {
        uVar3 = *(undefined4 *)(param_2 + 0x28);
        uVar2 = FUN_0049d800(uVar3);
        goto LAB_004298f3;
      }
    }
  }
  return 4;
}


