// FUN_00422ff0 @ 00422ff0 size=74

undefined4 FUN_00422ff0(int param_1,int param_2)

{
  if ((*(int *)(param_2 + 0x10) != 2) && (*(int *)(param_2 + 0x10) != 4)) {
    return 4;
  }
  FUN_00520d10(*(undefined4 *)(param_2 + 0x28));
  FUN_0041ee50(s_Scroll___s__set_to_text_tag___s__005cb650,*(undefined4 *)(param_1 + 0x38),
               *(undefined4 *)(param_2 + 0x28));
  FUN_00479580();
  return 0;
}


