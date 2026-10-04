// FUN_00428200 @ 00428200 size=69

undefined4 FUN_00428200(int param_1,int param_2)

{
  if (*(int *)(param_2 + 0x28) == 0) {
    FUN_0041ee50(s_Statmod_list_required_005ccab4);
    return 4;
  }
  if (*(short *)(param_1 + 4) != 0xb) {
    FUN_0041ee50(s_Player_required_005ccacc);
    return 4;
  }
  FUN_0051c550(param_2);
  return 0;
}


