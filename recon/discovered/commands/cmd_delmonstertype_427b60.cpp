// FUN_00427b60 @ 00427b60 size=105

undefined4 FUN_00427b60(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined1 auStack_3c [60];
  
  iVar1 = FUN_0047a410(param_2,&DAT_005cc9b0,auStack_3c);
  if (iVar1 == 0) {
    return 4;
  }
  iVar1 = FUN_00517110(auStack_3c);
  if (iVar1 != 0) {
    FUN_0041ee50(s_Removed___s__from_the_monster_li_005cc9b4,auStack_3c);
    return 0;
  }
  FUN_0041ee50(s___s__not_found_in_monster_list_005cc9d8,auStack_3c);
  return 0;
}


