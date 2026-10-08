// FUN_00428e50 @ 00428e50 size=251

undefined4 FUN_00428e50(undefined4 param_1,int param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int iStack_3c;
  
  if (param_4 == 0) {
    FUN_0041ee50(s_No_script_available_005ccc98);
    return 4;
  }
  if ((*(int *)(param_2 + 0x10) != 2) && (*(int *)(param_2 + 0x10) != 4)) {
    FUN_0041ee50(s_Name__class__or_player_class_nam_005cccb0);
    return 4;
  }
  FUN_0044cf80(0,0x20,2,0,0xffffffff);
  while( true ) {
    if (iStack_3c == 0) {
      FUN_00479580();
      FUN_0041ee50(s_No_matching_characters_in_range_005cccdc);
      return 4;
    }
    iVar1 = FUN_0059a530(*(undefined4 *)(iStack_3c + 0x38),*(undefined4 *)(param_2 + 0x28));
    if (((iVar1 == 0) ||
        (iVar1 = FUN_0059a530(*(undefined4 *)(*(int *)(iStack_3c + 0x48) + 4),
                              *(undefined4 *)(param_2 + 0x28)), iVar1 == 0)) ||
       ((*(short *)(iStack_3c + 4) == 0xb &&
        (iVar1 = FUN_0059a530(*(undefined4 *)(*(int *)(iStack_3c + 0xfc) + 400),
                              *(undefined4 *)(param_2 + 0x28)), iVar1 == 0)))) break;
    FUN_0044d080();
  }
  *(int *)(param_4 + 0xd4) = iStack_3c;
  return 0;
}


