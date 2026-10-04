// FUN_00426cd0 @ 00426cd0 size=101

undefined4 FUN_00426cd0(int *param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  
  if (param_1 != (int *)0x0) {
    if ((*(int *)(param_2 + 0x10) == 2) || (*(int *)(param_2 + 0x10) == 4)) {
      uVar1 = FUN_0041e690(*(undefined4 *)(param_2 + 0x28),param_1,param_4);
      FUN_00479580();
      (**(code **)(*param_1 + 0x250))(uVar1);
      if (DAT_0067682c != 0) {
        FUN_00586750(param_1,uVar1);
      }
      return 0;
    }
  }
  return 4;
}


