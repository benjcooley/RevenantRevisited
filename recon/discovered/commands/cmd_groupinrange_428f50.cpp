// FUN_00428f50 @ 00428f50 size=266

undefined4 FUN_00428f50(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int local_3c;
  
  iVar2 = 100;
  if (param_1 == (int *)0x0) {
    FUN_0041ee50(s_Context_is_required_005ccd00);
    return 4;
  }
  if ((short)param_1[1] == 0xb) {
    if (*(int *)(param_2 + 0x10) == 8) {
      iVar2 = __ftol();
      FUN_00479580();
    }
    FUN_0044cf80(0,0x20,2,0,0xffffffff);
    while( true ) {
      if (local_3c == 0) {
        FUN_0041ee50(s_All_party_is_in_range_of__d_005ccd34,iVar2);
        return 0x40;
      }
      if ((((*(short *)(local_3c + 4) == 0xb) && (local_3c != 0)) &&
          (*(char *)(local_3c + 0x494) != '\0')) &&
         ((iVar1 = FUN_0059a530(local_3c + 0x494,param_1 + 0x125), iVar1 == 0 &&
          (iVar1 = (**(code **)(*param_1 + 4))(local_3c), iVar2 < iVar1)))) break;
      FUN_0044d080();
    }
    FUN_0041ee50(s_Member_of_the_party_is_out_of__d_005ccd54,iVar2);
    return 0x80;
  }
  FUN_0041ee50(s_Context_must_be_a_player_005ccd18);
  return 4;
}


