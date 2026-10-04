// FUN_004280e0 @ 004280e0 size=93

undefined4 FUN_004280e0(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  if (*(int *)(param_2 + 0x10) == 2) {
    uVar2 = *(undefined4 *)(param_2 + 0x28);
  }
  else {
    if (*(int *)(param_2 + 0x10) != 4) {
      FUN_0041ee50(s_There_was_no_message_to_display__005cca90);
      return 2;
    }
    iVar1 = FUN_0049d800(*(undefined4 *)(param_2 + 0x28));
    if (iVar1 == 0) {
      return 0;
    }
    uVar2 = FUN_0049d800(*(undefined4 *)(param_2 + 0x28));
  }
  FUN_0054d170(&DAT_0065c5d0,uVar2);
  return 0;
}


