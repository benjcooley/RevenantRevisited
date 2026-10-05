// FUN_0054d700 @ 0054d700 size=1596

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0054d700(undefined4 param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  int *piVar3;
  undefined4 *puVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  char *pcVar10;
  undefined **local_60;
  char *local_5c;
  char *local_58;
  char *local_54;
  char *local_50;
  undefined4 local_4c;
  int local_48;
  int local_44;
  undefined ***local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_2c;
  undefined4 local_28;
  undefined1 *local_24;
  undefined1 local_20;
  undefined4 local_1c;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2244;
  local_c = ExceptionList;
  if (DAT_0066829c != 0) {
    ExceptionList = &local_c;
    FUN_005701f0(DAT_00676874,param_2);
    ExceptionList = local_c;
    return;
  }
  if ((*param_2 == '@') && (DAT_00667fcc != (int *)0x0)) {
    local_58 = param_2 + 1;
    uVar8 = 0xffffffff;
    pcVar10 = local_58;
    do {
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    local_60 = &PTR_LAB_005a36f8;
    local_54 = local_58 + (~uVar8 - 1);
    local_5c = s_String_005e5884;
    ExceptionList = &local_c;
    local_50 = local_58;
    FUN_00478720();
    local_40 = &local_60;
    local_4 = 0;
    local_4c = 0;
    local_48 = 0;
    local_44 = 0;
    local_3c = 0;
    local_38 = 0;
    local_34 = 0;
    local_2c = 0;
    local_28 = 0;
    local_20 = 0;
    local_1c = 1;
    local_24 = (undefined1 *)FUN_00482fb0(0x2000);
    *local_24 = 0;
    local_4 = 1;
    FUN_00478a10();
    FUN_0041e8e0(DAT_00667fcc,&local_4c,1,0);
    local_4 = 2;
    FUN_004830f0(local_24);
    if (local_48 == 0) {
      if (local_44 != 0) {
        FUN_004830f0(local_40);
        FUN_004a1540(local_44);
      }
    }
    else {
      FUN_004830f0(local_40);
      FUN_004830f0(local_48);
    }
    local_4 = 0xffffffff;
    FUN_00478730();
    ExceptionList = local_c;
    return;
  }
  ExceptionList = &local_c;
  iVar2 = FUN_0059a530(param_2,s_alreadydead_005e588c);
  if (iVar2 == 0) {
    DAT_00668104 = (uint)(DAT_00668104 == 0);
    uVar8 = DAT_00668104;
LAB_0054dcb5:
    if (uVar8 == 0) {
      pcVar10 = s_cheatdisabled_005e593c;
      goto LAB_0054dcc5;
    }
  }
  else {
    iVar2 = FUN_0059a530(param_2,s_alchemy_005e5898);
    if (iVar2 != 0) {
      iVar2 = FUN_0059a530(param_2,s_nahkranoth_005e58a0);
      if (iVar2 == 0) {
        DAT_00668108 = (uint)(DAT_00668108 == 0);
        uVar8 = DAT_00668108;
      }
      else {
        iVar2 = FUN_0059a530(param_2,s_noamnesia_005e58ac);
        if (iVar2 == 0) {
          if (DAT_00667fcc != (int *)0x0) {
            (**(code **)(*DAT_00667fcc + 0x358))(0x1e);
            uVar6 = (**(code **)(*DAT_00667fcc + 0x354))();
            (**(code **)(*DAT_00667fcc + 0x370))(uVar6);
            pcVar10 = s_cheatenabled_005e592c;
            goto LAB_0054dcc5;
          }
          goto LAB_0054dcb9;
        }
        iVar2 = FUN_0059a530(param_2,s_lookunderthehood_005e58b8);
        if (iVar2 == 0) {
          uVar8 = (uint)(DAT_0066812c == 0);
          DAT_0066812c = uVar8;
        }
        else {
          iVar2 = FUN_0059a530(param_2,s_dummies_005e58cc);
          if (iVar2 == 0) {
            DAT_00668110 = (uint)(DAT_00668110 == 0);
            uVar8 = DAT_00668110;
          }
          else {
            iVar2 = FUN_0059a530(param_2,s_abracadabra_005e58d4);
            if (iVar2 == 0) {
              DAT_0066810c = (uint)(DAT_0066810c == 0);
              if (DAT_00667fcc != (int *)0x0) {
                piVar3 = (int *)(**(code **)(*DAT_00667fcc + 0xa8))(s_spell_pouch_005e58e0);
                if (piVar3 == (int *)0x0) {
                  (**(code **)(*DAT_00667fcc + 0x54))(s_spell_pouch_005e58ec,1,0xffffffff);
                  piVar3 = (int *)(**(code **)(*DAT_00667fcc + 0xa8))(s_spell_pouch_005e58f8);
                  if (piVar3 == (int *)0x0) {
                    piVar3 = DAT_00667fcc;
                  }
                }
                uVar8 = 0;
                if (0 < (int)DAT_0066deec) {
                  do {
                    if (((DAT_0066defc == 0) || (DAT_0066deec <= uVar8)) ||
                       (*(int *)(DAT_0066defc + uVar8 * 4) == 0)) {
                      puVar4 = (undefined4 *)0x0;
                    }
                    else {
                      puVar4 = *(undefined4 **)(DAT_0066defc + uVar8 * 4);
                      if (puVar4 == (undefined4 *)0x0) {
                        puVar4 = DAT_0066df00;
                      }
                    }
                    uVar6 = *puVar4;
                    iVar2 = (**(code **)(*DAT_00667fcc + 0xa8))(uVar6);
                    if (iVar2 == 0) {
                      (**(code **)(*piVar3 + 0x54))(uVar6,1,0xffffffff);
                      iVar2 = (**(code **)(*DAT_00667fcc + 0xa8))(uVar6);
                      if (iVar2 != 0) goto LAB_0054da79;
                    }
                    else {
LAB_0054da79:
                      if (*(int **)(iVar2 + 100) != piVar3) {
                        (**(code **)(*piVar3 + 0x58))(iVar2,0xffffffff);
                      }
                    }
                    uVar8 = uVar8 + 1;
                  } while ((int)uVar8 < (int)DAT_0066deec);
                }
                iVar2 = DAT_00667c3c;
                iVar7 = 0;
                if (0 < DAT_00667c3c) {
                  do {
                    iVar9 = 0;
                    piVar3 = *(int **)(DAT_00667c4c + iVar7 * 4);
                    if (0 < *piVar3) {
                      do {
                        iVar5 = *(int *)(piVar3[4] + iVar9 * 4);
                        if (iVar5 == 0) {
                          iVar5 = piVar3[5];
                        }
                        FUN_00544fb0(iVar5 + 0x24);
                        iVar9 = iVar9 + 1;
                      } while (iVar9 < *piVar3);
                    }
                    iVar7 = iVar7 + 1;
                  } while (iVar7 < iVar2);
                }
              }
              _DAT_0065b078 = 1;
              _DAT_0065d548 = 1;
              _DAT_0065aa28 = 1;
              uVar8 = DAT_0066810c;
            }
            else {
              iVar2 = FUN_0059a530(param_2,s_potionsnlotions_005e5904);
              if (iVar2 == 0) {
                uVar8 = 0;
                if (0 < (int)DAT_0066d28c) {
                  do {
                    if (((DAT_0066d29c == 0) || (DAT_0066d28c <= uVar8)) ||
                       (*(int *)(DAT_0066d29c + uVar8 * 4) == 0)) {
                      puVar4 = (undefined4 *)0x0;
                    }
                    else {
                      puVar4 = *(undefined4 **)(DAT_0066d29c + uVar8 * 4);
                      if (puVar4 == (undefined4 *)0x0) {
                        puVar4 = DAT_0066d2a0;
                      }
                    }
                    uVar6 = *puVar4;
                    piVar3 = (int *)(**(code **)(*DAT_00667fcc + 0xa8))(uVar6);
                    if (piVar3 == (int *)0x0) {
                      (**(code **)(*DAT_00667fcc + 0x54))(uVar6,5,0xffffffff);
                    }
                    else {
                      iVar2 = (**(code **)(*piVar3 + 0x198))();
                      if (iVar2 < 5) {
                        uVar6 = 5;
                      }
                      else {
                        uVar6 = (**(code **)(*piVar3 + 0x198))();
                      }
                      (**(code **)(*piVar3 + 0x19c))(uVar6);
                    }
                    uVar8 = uVar8 + 1;
                  } while ((int)uVar8 < (int)DAT_0066d28c);
                }
                pcVar10 = s_cheatenabled_005e592c;
                _DAT_0065b078 = 1;
                _DAT_0065d548 = 1;
                goto LAB_0054dcc5;
              }
              iVar2 = FUN_0059a530(param_2,s_gimmesomegrub_005e5914);
              if (iVar2 == 0) {
                uVar8 = 0;
                if (0 < (int)DAT_0066d2cc) {
                  do {
                    if (((DAT_0066d2dc == 0) || (DAT_0066d2cc <= uVar8)) ||
                       (*(int *)(DAT_0066d2dc + uVar8 * 4) == 0)) {
                      puVar4 = (undefined4 *)0x0;
                    }
                    else {
                      puVar4 = *(undefined4 **)(DAT_0066d2dc + uVar8 * 4);
                      if (puVar4 == (undefined4 *)0x0) {
                        puVar4 = DAT_0066d2e0;
                      }
                    }
                    uVar6 = *puVar4;
                    piVar3 = (int *)(**(code **)(*DAT_00667fcc + 0xa8))(uVar6);
                    if (piVar3 == (int *)0x0) {
                      (**(code **)(*DAT_00667fcc + 0x54))(uVar6,5,0xffffffff);
                    }
                    else {
                      iVar2 = (**(code **)(*piVar3 + 0x198))();
                      if (iVar2 < 5) {
                        uVar6 = 5;
                      }
                      else {
                        uVar6 = (**(code **)(*piVar3 + 0x198))();
                      }
                      (**(code **)(*piVar3 + 0x19c))(uVar6);
                    }
                    uVar8 = uVar8 + 1;
                  } while ((int)uVar8 < (int)DAT_0066d2cc);
                }
                pcVar10 = s_cheatenabled_005e592c;
                _DAT_0065b078 = 1;
                _DAT_0065d548 = 1;
                goto LAB_0054dcc5;
              }
              iVar2 = FUN_0059a530(param_2,s_debug_005e5924);
              if (iVar2 != 0) {
                ExceptionList = local_c;
                return;
              }
              uVar8 = (uint)(DAT_00668130 == 0);
              DAT_0066812c = uVar8;
              DAT_00668130 = uVar8;
            }
          }
        }
      }
      goto LAB_0054dcb5;
    }
    if (DAT_00667fcc != (int *)0x0) {
      FUN_0051e900(999999);
      pcVar10 = s_cheatenabled_005e592c;
      goto LAB_0054dcc5;
    }
  }
LAB_0054dcb9:
  pcVar10 = s_cheatenabled_005e592c;
LAB_0054dcc5:
  iVar2 = FUN_0049d6d0(pcVar10);
  if (-1 < iVar2) {
    uVar6 = FUN_0049d780(iVar2);
    FUN_0054d170(param_1,uVar6);
  }
  iVar2 = FUN_0049c430(s_potionmix_005e594c);
  if ((-1 < iVar2) && (iVar7 = FUN_0049b650(iVar2), iVar7 != 0)) {
    FUN_0049b990(iVar2,0x7f,1,0,0x50,700);
  }
  ExceptionList = local_c;
  return;
}


