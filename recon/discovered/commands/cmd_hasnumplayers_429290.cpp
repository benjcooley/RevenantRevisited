// FUN_00429290 @ 00429290 size=256

int FUN_00429290(int param_1,int param_2)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  char *pcVar6;
  
  iVar2 = param_1;
  if (param_1 == 0) {
    FUN_0041ee50(s_Context_is_required_005cce88);
    return 4;
  }
  if (*(short *)(param_1 + 4) != 0xb) {
    FUN_0041ee50(s_Context_must_be_a_player_005ccea0);
    return 4;
  }
  iVar3 = FUN_0047a410(param_2,s__d__d_005ccebc,&param_1,&param_2);
  if (iVar3 == 0) {
    return 4;
  }
  iVar4 = 0;
  iVar5 = 0;
  iVar3 = FUN_0051ee70(0);
  if (0 < iVar3) {
    do {
      iVar3 = FUN_0051eea0(iVar4,0);
      if ((iVar3 != 0) && (*(char *)(iVar3 + 0x494) != '\0')) {
        iVar3 = FUN_0059a530(iVar3 + 0x494,iVar2 + 0x494);
        if (iVar3 == 0) {
          iVar5 = iVar5 + 1;
        }
      }
      iVar4 = iVar4 + 1;
      iVar3 = FUN_0051ee70(0);
    } while (iVar4 < iVar3);
  }
  bVar1 = 1;
  if ((iVar5 < param_1) || (param_2 < iVar5)) {
    bVar1 = 0;
    pcVar6 = s_Proper_number_of_players_in_part_005ccee8;
  }
  else {
    pcVar6 = s_To_many_or_few_players_in_party_005ccec4;
  }
  FUN_0041ee50(pcVar6);
  return (-(uint)bVar1 & 0xffffffc0) + 0x80;
}


