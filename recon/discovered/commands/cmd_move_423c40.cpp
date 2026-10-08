// FUN_00423c40 @ 00423c40 size=246

undefined4 FUN_00423c40(int *param_1,int param_2)

{
  int iVar1;
  int iStack_18;
  int iStack_14;
  int iStack_10;
  int iStack_c;
  int iStack_8;
  int iStack_4;
  
  iStack_18 = 0;
  iStack_14 = 0;
  iStack_10 = 0;
  iVar1 = FUN_0047a410(param_2,s__i__i_005cb8dc,&iStack_18,&iStack_14);
  if (iVar1 == 0) {
    return 4;
  }
  if (*(int *)(param_2 + 0x10) == 8) {
    iVar1 = FUN_0047a410(param_2,&DAT_005cb8e4,&iStack_10);
    if (iVar1 == 0) {
      return 4;
    }
  }
  iStack_c = param_1[4] + iStack_18;
  iStack_8 = param_1[5] + iStack_14;
  iStack_4 = param_1[6] + iStack_10;
  FUN_00454920(param_1[0x10]);
  (**(code **)(*param_1 + 8))(&iStack_c,0xffffffff,DAT_00655490);
  FUN_00454920(param_1[0x10]);
  if ((DAT_0066829c != 0) && (DAT_0067682c != 0)) {
    FUN_00586bb0(param_1,&iStack_18,0xffffffff);
  }
  return 0;
}


