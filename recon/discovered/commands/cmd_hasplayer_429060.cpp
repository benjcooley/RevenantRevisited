// FUN_00429060 @ 00429060 size=53

int FUN_00429060(int param_1,int param_2)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  char *pcVar5;
  undefined4 uVar6;
  
  if (param_1 == 0) {
    FUN_0041ee50(s_Context_is_required_005ccd7c);
    return 4;
  }
  if (*(short *)(param_1 + 4) != 0xb) {
    FUN_0041ee50(s_Context_must_be_a_player_005ccd94);
    return 4;
  }
  iVar4 = 0;
  bVar1 = 0;
  iVar2 = FUN_0051ee70(0);
  if (0 < iVar2) {
    do {
      iVar2 = FUN_0051eea0(iVar4,0);
      if ((iVar2 != 0) && (*(char *)(iVar2 + 0x494) != '\0')) {
        iVar3 = FUN_0059a530(iVar2 + 0x494,param_1 + 0x494);
        if (iVar3 == 0) {
          iVar3 = FUN_0059a530(*(undefined4 *)(iVar2 + 0x38),*(undefined4 *)(param_2 + 0x28));
          if (iVar3 != 0) {
            iVar2 = FUN_0059a530(*(undefined4 *)(*(int *)(iVar2 + 0xfc) + 400),
                                 *(undefined4 *)(param_2 + 0x28));
            if (iVar2 != 0) goto LAB_00429127;
          }
          uVar6 = *(undefined4 *)(param_2 + 0x28);
          bVar1 = 1;
          pcVar5 = s_No_character_of_class__s_in_part_005ccdb0;
          goto LAB_00429145;
        }
      }
LAB_00429127:
      iVar4 = iVar4 + 1;
      iVar2 = FUN_0051ee70(0);
    } while (iVar4 < iVar2);
  }
  uVar6 = *(undefined4 *)(param_2 + 0x28);
  pcVar5 = s_Character_of_class__s_found_in_p_005ccdd4;
LAB_00429145:
  FUN_0041ee50(pcVar5,uVar6);
  FUN_00479580();
  return (-(uint)bVar1 & 0xffffffc0) + 0x80;
}


