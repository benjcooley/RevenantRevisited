// FUN_0041e8e0 @ 0041e8e0 size=1383

uint FUN_0041e8e0(int *param_1,uint param_2,int param_3,undefined4 param_4)

{
  undefined **ppuVar1;
  char cVar2;
  int iVar3;
  bool bVar4;
  bool bVar5;
  int iVar6;
  int *piVar7;
  undefined4 uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  undefined **ppuVar12;
  char *pcVar13;
  char *pcVar14;
  uint local_5c;
  int local_58;
  int *piStack_3c;
  
  iVar9 = param_2;
  bVar4 = false;
  bVar5 = false;
  FUN_00479680();
  if (*(int *)(param_2 + 0x10) == 10) {
    return 0;
  }
  uVar10 = 0xffffffff;
  pcVar14 = *(char **)(param_2 + 0x28);
  do {
    pcVar13 = pcVar14;
    if (uVar10 == 0) break;
    uVar10 = uVar10 - 1;
    pcVar13 = pcVar14 + 1;
    cVar2 = *pcVar14;
    pcVar14 = pcVar13;
  } while (cVar2 != '\0');
  uVar10 = ~uVar10;
  local_5c = 0xffffffff;
  pcVar14 = pcVar13 + -uVar10;
  pcVar13 = (char *)&DAT_00654a88;
  for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
    *(undefined4 *)pcVar13 = *(undefined4 *)pcVar14;
    pcVar14 = pcVar14 + 4;
    pcVar13 = pcVar13 + 4;
  }
  for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
    *pcVar13 = *pcVar14;
    pcVar14 = pcVar14 + 1;
    pcVar13 = pcVar13 + 1;
  }
  FUN_00533dc0(param_1);
  piVar7 = param_1;
  if ((*(int *)(param_2 + 0x10) == 4) || (*(int *)(param_2 + 0x10) == 2)) {
    iVar6 = FUN_00479700(s_nowait_005caa60,0);
    if (iVar6 != 0) {
      bVar5 = true;
      FUN_00479580();
      uVar10 = 0xffffffff;
      pcVar14 = *(char **)(param_2 + 0x28);
      do {
        pcVar13 = pcVar14;
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        pcVar13 = pcVar14 + 1;
        cVar2 = *pcVar14;
        pcVar14 = pcVar13;
      } while (cVar2 != '\0');
      uVar10 = ~uVar10;
      pcVar14 = pcVar13 + -uVar10;
      pcVar13 = (char *)&DAT_00654a88;
      for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
        *(undefined4 *)pcVar13 = *(undefined4 *)pcVar14;
        pcVar14 = pcVar14 + 4;
        pcVar13 = pcVar13 + 4;
      }
      for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
        *pcVar13 = *pcVar14;
        pcVar14 = pcVar14 + 1;
        pcVar13 = pcVar13 + 1;
      }
    }
    FUN_00478a10();
    iVar6 = FUN_00479700(&DAT_005caa68,0);
    if (iVar6 != 0) {
      iVar6 = FUN_0059a600(&DAT_00654a88,s_group_005caa6c,5);
      if (iVar6 == 0) {
        local_5c = FUN_0058b42c(&DAT_00654a8d);
        FUN_00478a10();
        if (*(int *)(param_2 + 0x10) != 4) {
          FUN_0041ee50(s_Specify_command_following_group_c_005caa74);
          return 4;
        }
        uVar10 = 0xffffffff;
        pcVar14 = *(char **)(param_2 + 0x28);
        do {
          pcVar13 = pcVar14;
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          pcVar13 = pcVar14 + 1;
          cVar2 = *pcVar14;
          pcVar14 = pcVar13;
        } while (cVar2 != '\0');
        uVar10 = ~uVar10;
        pcVar14 = pcVar13 + -uVar10;
        pcVar13 = (char *)&DAT_00654a88;
        for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
          *(undefined4 *)pcVar13 = *(undefined4 *)pcVar14;
          pcVar14 = pcVar14 + 4;
          pcVar13 = pcVar13 + 4;
        }
      }
      else {
        piVar7 = (int *)FUN_0041e690(&DAT_00654a88,param_1,param_4);
        if (piVar7 == (int *)0x0) {
          FUN_0058b100(&DAT_00654a88,s__s__Context_not_found_005caacc,&DAT_00654a88);
          FUN_0041ee50(&DAT_00654a88);
          return 2;
        }
        FUN_00478a10();
        if (*(int *)(param_2 + 0x10) != 4) {
          FUN_0041ee50(s_Specify_command_following_object_005caaa0);
          return 4;
        }
        uVar10 = 0xffffffff;
        pcVar14 = *(char **)(param_2 + 0x28);
        do {
          pcVar13 = pcVar14;
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          pcVar13 = pcVar14 + 1;
          cVar2 = *pcVar14;
          pcVar14 = pcVar13;
        } while (cVar2 != '\0');
        uVar10 = ~uVar10;
        pcVar14 = pcVar13 + -uVar10;
        pcVar13 = (char *)&DAT_00654a88;
        for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
          *(undefined4 *)pcVar13 = *(undefined4 *)pcVar14;
          pcVar14 = pcVar14 + 4;
          pcVar13 = pcVar13 + 4;
        }
      }
      for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
        *pcVar13 = *pcVar14;
        pcVar14 = pcVar14 + 1;
        pcVar13 = pcVar13 + 1;
      }
    }
  }
  param_2 = 2;
  local_58 = 0;
  if (PTR_s_activate_005c6e88 != (undefined *)0x0) {
    ppuVar12 = &PTR_s_activate_005c6e88;
    do {
      if (((param_3 != 0) && (iVar6 = FUN_004796a0(&DAT_00654a88,*ppuVar12), param_3 <= iVar6)) ||
         (iVar6 = FUN_0059a530(&DAT_00654a88,*ppuVar12), iVar6 == 0)) {
        if (((&DAT_005c6e9c)[local_58 * 7] != 0) && (DAT_00668154 == 0)) {
          FUN_0041ee50(s_Command_can_only_be_used_in_the_e_005caae4);
          bVar4 = true;
        }
        if ((int)local_5c < 0) {
          iVar6 = (&DAT_005c6e90)[local_58 * 7];
          iVar3 = (&DAT_005c6e94)[local_58 * 7];
          if (iVar6 < 0) {
            if (iVar3 < 0) goto LAB_0041ebd4;
            if (piVar7 == (int *)0x0) goto LAB_0041eb8c;
          }
          else if (piVar7 != (int *)0x0) {
LAB_0041eb8c:
            if (((iVar6 < 1) || (piVar7 == (int *)0x0)) || ((short)piVar7[1] == iVar6))
            goto LAB_0041ebd4;
          }
          if ((iVar3 < 0) || (iVar6 = FUN_00429910(piVar7,iVar3), iVar6 == 0)) {
            bVar4 = true;
            if (piVar7 == (int *)0x0) {
              pcVar14 = s_Object_context_required_for_comm_005cab0c;
            }
            else {
              pcVar14 = s_Command_not_availible_for_contex_005cab34;
            }
            FUN_0041ee50(pcVar14);
          }
        }
LAB_0041ebd4:
        uVar8 = (**(code **)(**(int **)(iVar9 + 0xc) + 0xc))();
        if (*(int *)(iVar9 + 0x10) != 9) {
          FUN_00479580();
        }
        if (((&DAT_005c6e98)[local_58 * 7] == 0) ||
           ((*(int *)(iVar9 + 0x10) != 9 && (*(int *)(iVar9 + 0x10) != 10)))) {
          if (bVar4) {
            FUN_004795c0();
          }
          else {
            if ((int)local_5c < 0) {
              param_2 = (**(code **)(&DAT_005c6e8c + local_58 * 0x1c))(piVar7,iVar9,param_1,param_4)
              ;
              goto LAB_0041ed1c;
            }
            FUN_0044cf80(0,0,0,0,0xffffffff);
            if (piStack_3c != (int *)0x0) goto LAB_0041ec5e;
          }
        }
        else {
          FUN_0041ee50((&PTR_s_usage__<object>_activate_005c6ea0)[local_58 * 7]);
          FUN_004795c0();
        }
        break;
      }
      ppuVar1 = ppuVar12 + 7;
      ppuVar12 = ppuVar12 + 7;
      local_58 = local_58 + 1;
    } while (*ppuVar1 != (undefined *)0x0);
  }
  goto LAB_0041ed21;
LAB_0041ec5e:
  do {
    if (*(byte *)((int)piStack_3c + 0x37) == local_5c) {
      (**(code **)(**(int **)(iVar9 + 0xc) + 0x10))(uVar8);
      FUN_00479580();
      iVar6 = (&DAT_005c6e90)[local_58 * 7];
      iVar3 = (&DAT_005c6e94)[local_58 * 7];
      piVar7 = piStack_3c;
      if (iVar6 < 0) {
        if (-1 < iVar3) {
          if (piStack_3c == (int *)0x0) goto LAB_0041ec9d;
          goto LAB_0041ecb1;
        }
      }
      else {
        if (piStack_3c != (int *)0x0) {
LAB_0041ec9d:
          if (((iVar6 < 1) || (piStack_3c == (int *)0x0)) || ((short)piStack_3c[1] == iVar6))
          goto LAB_0041ecc5;
        }
LAB_0041ecb1:
        if (((iVar3 < 0) || (piStack_3c == (int *)0x0)) ||
           ((0 < iVar3 && ((short)piStack_3c[1] != iVar3)))) goto LAB_0041ece2;
      }
LAB_0041ecc5:
      param_2 = (**(code **)(&DAT_005c6e8c + local_58 * 0x1c))(piStack_3c,iVar9,param_1,param_4);
      if ((param_2 & 0xe) != 0) break;
    }
LAB_0041ece2:
    FUN_0044d080();
  } while (piStack_3c != (int *)0x0);
LAB_0041ed1c:
  if (param_2 != 2) goto LAB_0041ed34;
LAB_0041ed21:
  if (piVar7 != (int *)0x0) {
    param_2 = (**(code **)(*piVar7 + 0x150))(iVar9);
  }
LAB_0041ed34:
  if ((param_2 & 4) != 0) {
    FUN_0041ee50(s_Bad_parameters__005cab60);
  }
  if ((param_2 & 8) != 0) {
    FUN_0041ee50(s_Couldn_t_complete_command_due_to_005cab74);
  }
  if ((param_2 & 0x14) != 0) {
    FUN_0041ee50((&PTR_s_usage__<object>_activate_005c6ea0)[local_58 * 7]);
  }
  if ((param_2 & 2) == 0) {
    if (((*(int *)(iVar9 + 0x10) != 9) && (*(int *)(iVar9 + 0x10) != 10)) &&
       ((param_2 & 0x100) == 0)) {
      if ((param_2 & 0xe) == 0) {
        FUN_0041ee50(s__extra_parameters_ignored__005cabbc);
      }
      iVar6 = *(int *)(iVar9 + 0x10);
      while ((iVar6 != 9 && (iVar6 != 10))) {
        FUN_00478a10();
        iVar6 = *(int *)(iVar9 + 0x10);
      }
    }
  }
  else {
    FUN_0041ee50(s_Unrecognized_command__005caba4);
  }
  if ((bVar5) && (param_2 == 1)) {
    param_2 = 0;
  }
  else if (param_2 == 0x20) {
    return 0x20;
  }
  if (((param_1 != (int *)0x0) && (piVar7 != (int *)0x0)) && (iVar9 = FUN_00471390(), iVar9 == 0)) {
    if (param_2 == 1) {
      FUN_00471310(piVar7);
      return 1;
    }
    if (param_2 == 0x4000) {
      FUN_00471330(piVar7);
    }
  }
  return param_2;
}


