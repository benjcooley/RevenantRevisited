// FUN_00428540 @ 00428540 size=106

undefined4 FUN_00428540(undefined4 param_1,int param_2)

{
  int iVar1;
  
  if (*(int *)(param_2 + 0x28) == 0) {
    return 4;
  }
  FUN_0048d260(0);
  iVar1 = FUN_0048d6d0(*(undefined4 *)(param_2 + 0x28));
  FUN_00479580();
  if (iVar1 < 0) {
    FUN_0041ee50(s_Invalid_Game_005ccb14);
    return 0;
  }
  FUN_0041ee50(s_Loading_game____005ccb24);
  FUN_0047e770(iVar1);
  return 0;
}


