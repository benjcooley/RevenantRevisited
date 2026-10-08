// FUN_00429850 @ 00429850 size=92

undefined4 FUN_00429850(undefined4 param_1,int param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  if (param_4 != 0) {
    if (*(int *)(param_2 + 0x10) == 2) {
      uVar2 = *(undefined4 *)(param_2 + 0x28);
LAB_00429890:
      FUN_004944c0(uVar2);
      FUN_00479580();
      return 0;
    }
    if (*(int *)(param_2 + 0x10) == 4) {
      iVar1 = FUN_0049d6d0(*(undefined4 *)(param_2 + 0x28));
      if (-1 < iVar1) {
        uVar2 = FUN_0049d800(*(undefined4 *)(param_2 + 0x28));
        goto LAB_00429890;
      }
    }
  }
  return 4;
}


