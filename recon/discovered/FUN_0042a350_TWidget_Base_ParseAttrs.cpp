// FUN_0042a350_DefWidget_TWidgetBase_ParseAttrs @ 0042a350 size=1001

undefined4 * __thiscall FUN_0042a350_DefWidget_TWidgetBase_ParseAttrs(undefined4 *param_1,int *param_2,uint param_3,int param_4)

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  char *pcVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  char *pcVar8;
  
  iVar5 = param_4;
  param_1[3] = 0xffffffff;
  param_1[0x16] = 0xffffffff;
  param_1[0x1d] = 0xffffffff;
  param_1[0x20] = 0;
  param_1[4] = param_3;
  param_1[0x21] = 0;
  param_1[2] = 0;
  *(undefined1 *)(param_1 + 6) = 0;
  *(undefined1 *)(param_1 + 0xe) = 0;
  param_1[0x17] = 0;
  *(undefined2 *)(param_1 + 0x1c) = 0;
  param_1[0x1e] = 0;
  param_1[0x22] = 0;
  param_1[0x23] = 0xffffffff;
  *param_1 = &PTR_FUN_005a3ab8;
  param_1[0x19] = 0xffffd8f0;
  param_1[0x18] = 0xffffd8f0;
  param_1[0x1b] = 0xffffd8f0;
  param_1[0x1a] = 0xffffd8f0;
  param_1[5] = 1;
  iVar2 = FUN_00479700(&DAT_005ccfb8,0);
  if (iVar2 != 0) {
    FUN_00479580();
    iVar2 = FUN_0047a410(iVar5,s__i__i_005ccfbc,param_1 + 0x18,param_1 + 0x19);
    if (iVar2 == 0) {
      (**(code **)(*param_2 + 0x98))(s_Unable_to_parse_control_pos_005ccfc4,0);
      return param_1;
    }
    if (*(int *)(iVar5 + 0x10) == 8) {
      iVar2 = FUN_0047a410(iVar5,s__i__i_005ccfe0,param_1 + 0x1a,param_1 + 0x1b);
      if (iVar2 == 0) {
        (**(code **)(*param_2 + 0x98))(s_Unable_to_parse_control_size_005ccfe8,0);
        return param_1;
      }
    }
  }
  iVar2 = FUN_00479700(&DAT_005cd008,0);
  if (iVar2 != 0) {
    FUN_00479580();
    if (*(int *)(iVar5 + 0x10) != 2) {
      (**(code **)(*param_2 + 0x98))(0,0);
      return param_1;
    }
    _strncpy((char *)(param_1 + 6),*(char **)(iVar5 + 0x28),0x1f);
    *(undefined1 *)((int)param_1 + 0x37) = 0;
    FUN_00479580();
  }
  iVar2 = FUN_00479700(s_FIELD_005cd010,0);
  if (iVar2 != 0) {
    FUN_00479580();
    if (*(int *)(iVar5 + 0x10) != 2) goto LAB_0042a4fe;
    _strncpy((char *)(param_1 + 0xe),*(char **)(iVar5 + 0x28),0x1f);
    *(undefined1 *)((int)param_1 + 0x57) = 0;
    FUN_00479580();
  }
  iVar2 = FUN_00479700(s_INDEX_005cd018,0);
  if (iVar2 != 0) {
    FUN_00479580();
    if (*(int *)(iVar5 + 0x10) != 8) {
LAB_0042a4fe:
      (**(code **)(*param_2 + 0x98))(0,0);
      return param_1;
    }
    param_1[0x16] = *(undefined4 *)(iVar5 + 0x14);
    FUN_00479580();
  }
  iVar2 = FUN_00479700(&DAT_005cd020,0);
  if (iVar2 != 0) {
    FUN_00479580();
    if (param_1[0x17] != 0) {
      FUN_00482f80(param_1[0x17]);
    }
    iVar2 = *(int *)(iVar5 + 0x10);
    param_1[0x17] = 0;
    if (iVar2 != 2) {
      (**(code **)(*param_2 + 0x98))(s_Text_expected_005cd028,0);
      return param_1;
    }
    pcVar4 = *(char **)(iVar5 + 0x28);
    uVar6 = 0xffffffff;
    pcVar3 = pcVar4;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar3;
      pcVar3 = pcVar3 + 1;
    } while (cVar1 != '\0');
    pcVar3 = (char *)FUN_00482ef0(~uVar6);
    uVar6 = 0xffffffff;
    do {
      pcVar8 = pcVar4;
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      pcVar8 = pcVar4 + 1;
      cVar1 = *pcVar4;
      pcVar4 = pcVar8;
    } while (cVar1 != '\0');
    uVar6 = ~uVar6;
    pcVar4 = pcVar8 + -uVar6;
    pcVar8 = pcVar3;
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined4 *)pcVar8 = *(undefined4 *)pcVar4;
      pcVar4 = pcVar4 + 4;
      pcVar8 = pcVar8 + 4;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *pcVar8 = *pcVar4;
      pcVar4 = pcVar4 + 1;
      pcVar8 = pcVar8 + 1;
    }
    param_1[0x17] = pcVar3;
    FUN_00479580();
  }
  iVar2 = FUN_00479700(&DAT_005cd038,0);
  if (iVar2 != 0) {
    FUN_00479580();
    pcVar4 = (char *)FUN_0049d800(*(undefined4 *)(iVar5 + 0x28));
    if (pcVar4 == (char *)0x0) {
      param_1[0x17] = 0;
    }
    else {
      uVar6 = 0xffffffff;
      pcVar3 = pcVar4;
      do {
        if (uVar6 == 0) break;
        uVar6 = uVar6 - 1;
        cVar1 = *pcVar3;
        pcVar3 = pcVar3 + 1;
      } while (cVar1 != '\0');
      pcVar3 = (char *)FUN_00482ef0(~uVar6);
      uVar6 = 0xffffffff;
      do {
        pcVar8 = pcVar4;
        if (uVar6 == 0) break;
        uVar6 = uVar6 - 1;
        pcVar8 = pcVar4 + 1;
        cVar1 = *pcVar4;
        pcVar4 = pcVar8;
      } while (cVar1 != '\0');
      uVar6 = ~uVar6;
      pcVar4 = pcVar8 + -uVar6;
      pcVar8 = pcVar3;
      for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
        *(undefined4 *)pcVar8 = *(undefined4 *)pcVar4;
        pcVar4 = pcVar4 + 4;
        pcVar8 = pcVar8 + 4;
      }
      for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
        *pcVar8 = *pcVar4;
        pcVar4 = pcVar4 + 1;
        pcVar8 = pcVar8 + 1;
      }
      param_1[0x17] = pcVar3;
    }
    FUN_00479580();
  }
  iVar2 = FUN_00479700(s_FLAGS_005cd03c,0);
  if (iVar2 != 0) {
    FUN_00479580();
    iVar2 = FUN_0047a410(iVar5,&DAT_005cd044,&param_3);
    if (iVar2 == 0) {
      (**(code **)(*param_2 + 0x98))(s_Unable_to_parse_control_flags_005cd048,0);
      return param_1;
    }
    param_1[5] = param_1[5] | param_3;
  }
  iVar2 = FUN_00479700(s_GROUP_005cd068,0);
  if (iVar2 != 0) {
    FUN_00479580();
    iVar2 = FUN_0047a410(iVar5,&DAT_005cd070,param_1 + 0x1d);
    if (iVar2 == 0) {
      (**(code **)(*param_2 + 0x98))(s_Unable_to_parse_control_group_005cd074,0);
      return param_1;
    }
  }
  iVar2 = FUN_00479700(&DAT_005cd094,0);
  if (iVar2 != 0) {
    FUN_00479580();
    iVar5 = FUN_0047a410(iVar5,&DAT_005cd098,&param_3);
    if (iVar5 == 0) {
      (**(code **)(*param_2 + 0x98))(s_Unable_to_parse_control_hotkey_005cd09c,0);
      return param_1;
    }
    *(short *)(param_1 + 0x1c) = (short)(char)param_3;
  }
  param_1[5] = param_1[5] & 0xfffffffe;
  return param_1;
}


