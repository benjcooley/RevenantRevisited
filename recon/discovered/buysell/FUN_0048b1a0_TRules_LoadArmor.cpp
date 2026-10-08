// FUN_0048b1a0 @ 0048b1a0 size=755

undefined4 __thiscall FUN_0048b1a0(char *param_1,char *param_2,int param_3)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  char *pcVar6;
  char *pcVar7;
  char *pcVar8;
  char local_28 [39];
  undefined1 local_1;
  
  _strncpy(param_1,param_2,0x1f);
  param_1[0x1f] = '\0';
  FUN_00479680();
  iVar2 = FUN_00479700(s_BEGIN_005d9790,0);
  if (iVar2 == 0) {
    FUN_00479950(s_Char_block_BEGIN_expected_005d9798,0);
  }
  FUN_004795a0();
  iVar2 = *(int *)(param_3 + 0x10);
  do {
    if ((iVar2 == 10) || (iVar2 = FUN_00479700(&DAT_005d97b4,0), iVar2 != 0)) {
      iVar2 = FUN_00479700(&DAT_005d9850,0);
      if (iVar2 == 0) {
        FUN_00479950(s_Armor_block_END_expected_005d9854,0);
      }
      FUN_00478a10();
      return 1;
    }
    if (*(int *)(param_3 + 0x10) != 4) {
      FUN_00479950(s_Armor_data_keyword_expected_005d97b8,0);
    }
    _strncpy(local_28,*(char **)(param_3 + 0x28),0x27);
    local_1 = 0;
    FUN_00479580();
    iVar2 = FUN_0059a530(local_28,s_BASICMODS_005d97d4);
    if (iVar2 == 0) {
      pcVar6 = param_1 + 0xbc;
      pcVar8 = param_1 + 0xb8;
      iVar2 = FUN_0047a410(param_3,s__i___i___i___i___i___i___i___i_005d97e0,param_1 + 0xa0,
                           param_1 + 0xa4,param_1 + 0xa8,param_1 + 0xac,param_1 + 0xb4,
                           param_1 + 0xb0,pcVar8,pcVar6);
      if (iVar2 == 0) {
        param_1[0xa0] = '\x03';
        param_1[0xa1] = '\0';
        param_1[0xa2] = '\0';
        param_1[0xa3] = '\0';
        param_1[0xa4] = '\0';
        param_1[0xa5] = '\0';
        param_1[0xa6] = '\0';
        param_1[0xa7] = '\0';
        param_1[0xa8] = '\0';
        param_1[0xa9] = '\0';
        param_1[0xaa] = '\0';
        param_1[0xab] = '\0';
        param_1[0xac] = '\0';
        param_1[0xad] = '\0';
        param_1[0xae] = '\0';
        param_1[0xaf] = '\0';
        param_1[0xb0] = '\0';
        param_1[0xb1] = '\0';
        param_1[0xb2] = '\0';
        param_1[0xb3] = '\0';
        param_1[0xb4] = '\0';
        param_1[0xb5] = '\0';
        param_1[0xb6] = '\0';
        param_1[0xb7] = '\0';
        pcVar8[0] = '\0';
        pcVar8[1] = '\0';
        pcVar8[2] = '\0';
        pcVar8[3] = '\0';
        pcVar6[0] = '\0';
        pcVar6[1] = '\0';
        pcVar6[2] = '\0';
        pcVar6[3] = '\0';
LAB_0048b2d1:
        pcVar6 = local_28;
        pcVar8 = s_Error_parsing_tag__s_005d8dfc;
LAB_0048b2db:
        FUN_00479950(pcVar8,pcVar6);
      }
    }
    else {
      iVar2 = FUN_0059a530(local_28,s_DESCRIPTION_005d9800);
      if (iVar2 == 0) {
        iVar2 = FUN_0047a410(param_3,&DAT_005d980c,param_1 + 0x20);
        if (iVar2 == 0) {
          param_1[0x20] = '\0';
          goto LAB_0048b2d1;
        }
      }
      else {
        iVar2 = FUN_0059a530(local_28,s_STATLINE_005d9810);
        if (iVar2 != 0) {
          pcVar6 = *(char **)(param_3 + 0x28);
          pcVar8 = s_Invalid_character_tag__s_005d9824;
          goto LAB_0048b2db;
        }
        uVar3 = FUN_00482fb0(0x80);
        *(undefined4 *)(param_1 + 200) = uVar3;
        iVar2 = *(int *)(param_3 + 0x10);
        if (iVar2 != 9) {
          param_2 = (char *)0xa0;
          do {
            if (iVar2 == 10) break;
            pcVar6 = *(char **)(param_1 + 200);
            uVar4 = 0xffffffff;
            pcVar8 = pcVar6;
            do {
              if (uVar4 == 0) break;
              uVar4 = uVar4 - 1;
              cVar1 = *pcVar8;
              pcVar8 = pcVar8 + 1;
            } while (cVar1 != '\0');
            if ((int)param_2 < (int)(~uVar4 - 1)) {
              uVar4 = 0xffffffff;
              pcVar8 = pcVar6;
              do {
                if (uVar4 == 0) break;
                uVar4 = uVar4 - 1;
                cVar1 = *pcVar8;
                pcVar8 = pcVar8 + 1;
              } while (cVar1 != '\0');
              pcVar8 = (char *)FUN_00482ef0(~uVar4);
              uVar4 = 0xffffffff;
              do {
                pcVar7 = pcVar6;
                if (uVar4 == 0) break;
                uVar4 = uVar4 - 1;
                pcVar7 = pcVar6 + 1;
                cVar1 = *pcVar6;
                pcVar6 = pcVar7;
              } while (cVar1 != '\0');
              uVar4 = ~uVar4;
              pcVar6 = pcVar7 + -uVar4;
              pcVar7 = pcVar8;
              for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
                *(undefined4 *)pcVar7 = *(undefined4 *)pcVar6;
                pcVar6 = pcVar6 + 4;
                pcVar7 = pcVar7 + 4;
              }
              for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
                *pcVar7 = *pcVar6;
                pcVar6 = pcVar6 + 1;
                pcVar7 = pcVar7 + 1;
              }
              FUN_004830f0(*(undefined4 *)(param_1 + 200));
              param_2 = (char *)((int)param_2 + 0x20);
              uVar4 = 0xffffffff;
              pcVar6 = pcVar8;
              do {
                pcVar7 = pcVar6;
                if (uVar4 == 0) break;
                uVar4 = uVar4 - 1;
                pcVar7 = pcVar6 + 1;
                cVar1 = *pcVar6;
                pcVar6 = pcVar7;
              } while (cVar1 != '\0');
              uVar4 = ~uVar4;
              pcVar6 = pcVar7 + -uVar4;
              pcVar7 = *(char **)(param_1 + 200);
              for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
                *(undefined4 *)pcVar7 = *(undefined4 *)pcVar6;
                pcVar6 = pcVar6 + 4;
                pcVar7 = pcVar7 + 4;
              }
              for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
                *pcVar7 = *pcVar6;
                pcVar6 = pcVar6 + 1;
                pcVar7 = pcVar7 + 1;
              }
              FUN_004830f0(pcVar8);
            }
            FUN_0058b100(*(undefined4 *)(param_1 + 200),s__s__s_005d981c,
                         *(undefined4 *)(param_1 + 200),*(undefined4 *)(param_3 + 0x28));
            FUN_00479580();
            iVar2 = *(int *)(param_3 + 0x10);
          } while (iVar2 != 9);
        }
      }
    }
    if (*(int *)(param_3 + 0x10) != 9) {
      FUN_00479950(s_Return_expected_005d9840,0);
    }
    FUN_004795a0();
    iVar2 = *(int *)(param_3 + 0x10);
  } while( true );
}


