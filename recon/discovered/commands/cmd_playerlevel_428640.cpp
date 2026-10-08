// FUN_00428640 @ 00428640 size=47

undefined4 FUN_00428640(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  
  if (*(int *)(param_2 + 0x10) != 8) {
    return 4;
  }
  uVar1 = __ftol();
  FUN_0051d840(uVar1);
  FUN_00478a10();
  return 0;
}


