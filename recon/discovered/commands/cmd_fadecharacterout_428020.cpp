// FUN_00428020 @ 00428020 size=79

undefined4 FUN_00428020(undefined4 param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  iVar1 = FUN_0041e690(*(undefined4 *)(param_2 + 0x28),param_1,param_4);
  if (iVar1 != 0) {
    FUN_004d56c0(0xffffffff);
    if (DAT_0067682c != 0) {
      FUN_00587040(iVar1,0xffffffff,5,0);
    }
    return 0;
  }
  return 4;
}


