// FUN_00428070 @ 00428070 size=105

undefined4 FUN_00428070(undefined4 param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  iVar1 = FUN_0041e690(*(undefined4 *)(param_2 + 0x28),param_1,param_4);
  if (iVar1 != 0) {
    FUN_004d56c0(1);
    if (DAT_0067682c != 0) {
      FUN_00587040(iVar1,0xffffffff,5,100);
      if (DAT_0067682c != 0) {
        FUN_00587040(iVar1,0xffffffff,5,100);
      }
    }
    return 0;
  }
  return 4;
}


