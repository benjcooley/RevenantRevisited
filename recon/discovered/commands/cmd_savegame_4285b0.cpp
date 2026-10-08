// FUN_004285b0 @ 004285b0 size=132

undefined4 FUN_004285b0(undefined4 param_1,int param_2)

{
  int iVar1;
  char *pcVar2;
  char acStack_80 [127];
  undefined1 uStack_1;
  
  if (*(char **)(param_2 + 0x28) == (char *)0x0) {
    return 4;
  }
  _strncpy(acStack_80,*(char **)(param_2 + 0x28),0x7f);
  uStack_1 = 0;
  FUN_00479580();
  iVar1 = FUN_0059a530(acStack_80,s_newgame_005ccb38);
  if (iVar1 == 0) {
    pcVar2 = s_Saving__newgame_sav__file_to_cur_005ccb40;
  }
  else {
    pcVar2 = s_Saving_game____005ccb7c;
  }
  FUN_0041ee50(pcVar2);
  FUN_0047e810(acStack_80);
  return 0;
}


