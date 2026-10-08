// FUN_00429180 @ 00429180 size=258

int FUN_00429180(int param_1,uint param_2)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  char *pcVar6;
  
  iVar2 = param_1;
  if (param_1 == 0) {
    FUN_0041ee50(s_Context_is_required_005ccdfc);
    return 4;
  }
  if (*(short *)(param_1 + 4) != 0xb) {
    FUN_0041ee50(s_Context_must_be_a_player_005cce14);
    return 4;
  }
  iVar3 = FUN_0047a410(param_2,&DAT_005cce30,&param_1);
  if (iVar3 == 0) {
    return 4;
  }
  iVar5 = 0;
  bVar1 = 1;
  iVar3 = FUN_0051ee70(0);
  if (0 < iVar3) {
    do {
      piVar4 = (int *)FUN_0051eea0(iVar5,0);
      if ((piVar4 != (int *)0x0) && ((char)piVar4[0x125] != '\0')) {
        iVar3 = FUN_0059a530(piVar4 + 0x125,iVar2 + 0x494);
        param_2 = (uint)(iVar3 == 0);
        if (param_2 != 0) {
          iVar3 = (**(code **)(*piVar4 + 0x354))();
          if (iVar3 < param_1) {
            bVar1 = 0;
            pcVar6 = s_Not_all_party_members_are_above_l_005cce5c;
            goto LAB_00429268;
          }
        }
      }
      iVar5 = iVar5 + 1;
      iVar3 = FUN_0051ee70(0);
    } while (iVar5 < iVar3);
  }
  pcVar6 = s_All_party_members_are_above_leve_005cce34;
LAB_00429268:
  FUN_0041ee50(pcVar6,param_1);
  return (-(uint)bVar1 & 0xffffffc0) + 0x80;
}


