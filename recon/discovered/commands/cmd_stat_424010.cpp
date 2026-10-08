// FUN_00424010 @ 00424010 size=1

undefined4 FUN_00424010(int *param_1,int param_2,undefined4 param_3)

{
  char cVar1;
  char cVar2;
  byte bVar3;
  undefined2 uVar4;
  int iVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int iVar8;
  int iVar9;
  undefined4 uVar10;
  int *piVar11;
  uint uVar12;
  uint uVar13;
  undefined4 *puVar14;
  char **ppcVar15;
  char *pcVar16;
  char *pcVar17;
  undefined4 *puStack_9c;
  undefined4 *puStack_98;
  undefined4 *puStack_94;
  undefined4 uStack_90;
  undefined4 uStack_8c;
  undefined4 uStack_88;
  undefined4 uStack_84;
  char *apcStack_80 [4];
  char *pcStack_70;
  undefined4 uStack_6c;
  char *pcStack_68;
  char *pcStack_64;
  char *pcStack_60;
  char *pcStack_5c;
  char *pcStack_58;
  char *pcStack_54;
  undefined4 uStack_50;
  char *pcStack_4c;
  char *pcStack_48;
  char *pcStack_44;
  char *pcStack_40;
  char *pcStack_3c;
  char *pcStack_38;
  char *pcStack_34;
  char *pcStack_30;
  char *pcStack_2c;
  char *pcStack_28;
  char *pcStack_24;
  char *pcStack_20;
  undefined4 uStack_1c;
  char *pcStack_18;
  char *pcStack_14;
  char *pcStack_10;
  char *pcStack_c;
  char *pcStack_8;
  char *pcStack_4;
  
  apcStack_80[0] = s_IMMOBILE_005cb90c;
  apcStack_80[1] = s_EDITORLOCK_005cb918;
  apcStack_80[2] = s_LIGHT_005cb924;
  apcStack_80[3] = s_MOVING_005cb92c;
  pcStack_70 = s_ANIMATING_005cb934;
  uStack_6c = 0x5cb940;
  pcStack_68 = s_DISABLED_005cb944;
  pcStack_64 = s_INVISIBLE_005cb950;
  pcStack_60 = s_EDITOR_005cb95c;
  pcStack_5c = s_FOREGROUND_005cb964;
  pcStack_58 = s_SELDRAW_005cb970;
  pcStack_54 = s_REVEAL_005cb978;
  uStack_50 = 0x5cb980;
  pcStack_4c = s_GENERATED_005cb988;
  pcStack_48 = s_ANIMATE_005cb994;
  pcStack_44 = s_PULSE_005cb99c;
  pcStack_40 = s_WEIGHTLESS_005cb9a4;
  pcStack_3c = s_COMPLEX_005cb9b0;
  pcStack_38 = s_NOTIFY_005cb9b8;
  pcStack_34 = s_NONMAP_005cb9c0;
  pcStack_30 = s_ONEXIT_005cb9c8;
  pcStack_2c = s_PAUSE_005cb9d0;
  pcStack_28 = s_NOWALK_005cb9d8;
  pcStack_24 = s_PARALIZE_005cb9e0;
  pcStack_20 = s_NOCOLLISION_005cb9ec;
  uStack_1c = 0x5cb9f8;
  pcStack_18 = s_VIRGIN_005cba00;
  pcStack_14 = s_LOADING_005cba08;
  pcStack_10 = s_INVULNERABLE_005cba10;
  pcStack_c = s_BACKGROUND_005cba20;
  pcStack_8 = s_INVENTORY_005cba2c;
  pcStack_4 = s_CALLEDPREDEL_005cba38;
  uStack_90 = 0x5cba48;
  uStack_8c = 0x5cba4c;
  uStack_88 = 0x5cba50;
  uStack_84 = 0;
  if ((uint)(int)(short)param_1[1] < DAT_0065a258) {
    puStack_9c = (undefined4 *)(&DAT_0065a148)[(short)param_1[1]];
  }
  else {
    puStack_9c = (undefined4 *)0x0;
  }
  puVar14 = puStack_9c;
  if (*(int *)(param_2 + 0x10) != 4) {
    FUN_0058b100(&DAT_00654a88,s___s___s__s___d___d___d__005cbb54,param_1[0xe],
                 *(undefined4 *)param_1[0x13],*(undefined4 *)(param_1[0x12] + 4),param_1[4],
                 param_1[5],param_1[6]);
    FUN_0041ee50(&DAT_00654a88);
    uVar12 = 0xffffffff;
    pcVar16 = s_Flags__005cbb70;
    do {
      pcVar17 = pcVar16;
      if (uVar12 == 0) break;
      uVar12 = uVar12 - 1;
      pcVar17 = pcVar16 + 1;
      cVar1 = *pcVar16;
      pcVar16 = pcVar17;
    } while (cVar1 != '\0');
    uVar12 = ~uVar12;
    pcVar16 = pcVar17 + -uVar12;
    pcVar17 = (char *)&DAT_00654a88;
    for (uVar13 = uVar12 >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
      *(undefined4 *)pcVar17 = *(undefined4 *)pcVar16;
      pcVar16 = pcVar16 + 4;
      pcVar17 = pcVar17 + 4;
    }
    for (uVar12 = uVar12 & 3; uVar10 = DAT_005cbb7c, uVar12 != 0; uVar12 = uVar12 - 1) {
      *pcVar17 = *pcVar16;
      pcVar16 = pcVar16 + 1;
      pcVar17 = pcVar17 + 1;
    }
    uVar12 = param_1[2];
    if (uVar12 == 0) {
      iVar5 = -1;
      pcVar16 = (char *)&DAT_00654a88;
      do {
        pcVar17 = pcVar16;
        if (iVar5 == 0) break;
        iVar5 = iVar5 + -1;
        pcVar17 = pcVar16 + 1;
        cVar1 = *pcVar16;
        pcVar16 = pcVar17;
      } while (cVar1 != '\0');
      *(undefined4 *)(pcVar17 + -1) = DAT_005cbb78;
      *(undefined4 *)(pcVar17 + 3) = uVar10;
    }
    else {
      iVar5 = 0;
      ppcVar15 = apcStack_80;
      do {
        if ((uVar12 & 1 << ((byte)iVar5 & 0x1f)) != 0) {
          FUN_0058b100(&DAT_00654a88,s__s__s_005cabe0,&DAT_00654a88,*ppcVar15);
        }
        iVar5 = iVar5 + 1;
        ppcVar15 = ppcVar15 + 1;
      } while (iVar5 < 0x20);
    }
    iVar5 = -1;
    pcVar16 = (char *)&DAT_00654a88;
    do {
      pcVar17 = pcVar16;
      if (iVar5 == 0) break;
      iVar5 = iVar5 + -1;
      pcVar17 = pcVar16 + 1;
      cVar1 = *pcVar16;
      pcVar16 = pcVar17;
    } while (cVar1 != '\0');
    *(undefined2 *)(pcVar17 + -1) = DAT_005cbb80;
    FUN_0041ee50(&DAT_00654a88);
    piVar11 = (int *)FUN_0046e8a0();
    uVar4 = (undefined2)param_1[3];
    uVar10 = (**(code **)(*piVar11 + 0x7c))(uVar4);
    uVar10 = (**(code **)(*piVar11 + 0x78))(uVar4,uVar10);
    uVar10 = (**(code **)(*piVar11 + 0x74))(uVar4,uVar10);
    uVar10 = (**(code **)(*piVar11 + 0x6c))(uVar4,uVar10);
    uVar10 = (**(code **)(*piVar11 + 0x68))(uVar4,uVar10);
    uVar10 = (**(code **)(*piVar11 + 100))(uVar4,uVar10);
    FUN_0058b100(&DAT_00654a88,s_Registration___d___d___d__AnimRe_005cbb84,uVar10);
    FUN_0041ee50(&DAT_00654a88);
    if ((*(byte *)(param_1 + 2) & 4) == 0) {
      return 0;
    }
    FUN_0058b100(&DAT_00654a88,s_Light__intensity__d__multiplier___005cbbb8,
                 *(undefined1 *)((int)param_1 + 0x89),(int)*(short *)((int)param_1 + 0x8a));
    cVar1 = DAT_005cbbe6;
    bVar3 = *(byte *)(param_1 + 0x22);
    if (bVar3 == 0) {
      iVar5 = -1;
      pcVar16 = (char *)&DAT_00654a88;
      do {
        pcVar17 = pcVar16;
        if (iVar5 == 0) break;
        iVar5 = iVar5 + -1;
        pcVar17 = pcVar16 + 1;
        cVar2 = *pcVar16;
        pcVar16 = pcVar17;
      } while (cVar2 != '\0');
      *(undefined2 *)(pcVar17 + -1) = DAT_005cbbe4;
      pcVar17[1] = cVar1;
    }
    else {
      iVar5 = 0;
      puVar14 = (undefined4 *)&stack0xffffff58;
      do {
        if (((uint)bVar3 & 1 << ((byte)iVar5 & 0x1f)) != 0) {
          FUN_0058b100(&DAT_00654a88,s__s__s_005cabe0,&DAT_00654a88,*puVar14);
        }
        iVar5 = iVar5 + 1;
        puVar14 = puVar14 + 1;
      } while (iVar5 < 0x20);
    }
    FUN_0058b100(&DAT_00654a88,s__s__color___d___d___d___pos___d__005cbbe8,&DAT_00654a88,
                 *(undefined1 *)((int)param_1 + 0x8e),*(undefined1 *)((int)param_1 + 0x8d),
                 (char)param_1[0x23],param_1[0x24],param_1[0x25],param_1[0x26],param_1[0x27],
                 param_1[0x28]);
    FUN_0041ee50(&DAT_00654a88);
    return 0;
  }
  iVar5 = FUN_00479700(&DAT_005cba60,0);
  if (iVar5 != 0) {
    FUN_00479580();
    FUN_0041ee50(s_Class_stats_005cba68);
    iVar5 = 0;
    if (0 < *(short *)(puVar14 + 5)) {
      do {
        iVar8 = *(int *)(puVar14[0xd] + *(short *)((int)param_1 + 6) * 4);
        if (iVar8 == 0) {
          iVar8 = puVar14[0xe];
        }
        uVar10 = *(undefined4 *)(*(int *)(iVar8 + 0x10) + iVar5 * 4);
        uVar6 = FUN_004741b0(iVar5,&DAT_00654a88);
        FUN_0041ee50(s__s____d_005cba78,uVar6,uVar10);
        if ((iVar5 != 0) && (iVar5 % 5 == 0)) {
          FUN_0041ee50(s_Press_any_key_005cba84);
          FUN_0043f4e0();
        }
        iVar5 = iVar5 + 1;
      } while (iVar5 < *(short *)(puVar14 + 5));
    }
    FUN_0041ee50(s_Object_stats_005cba94);
    FUN_0043f4e0();
    iVar5 = 0;
    if (*(short *)(puVar14 + 7) < 1) {
      return 0;
    }
    do {
      iVar8 = *(int *)(puVar14[0xd] + *(short *)((int)param_1 + 6) * 4);
      if (iVar8 == 0) {
        iVar8 = puVar14[0xe];
      }
      uVar10 = *(undefined4 *)(*(int *)(iVar8 + 0x18) + iVar5 * 4);
      uVar6 = FUN_004741b0(iVar5,&DAT_00654a88);
      uVar7 = (**(code **)(*param_1 + 0xdc))(iVar5);
      FUN_0041ee50(s__s___d_____d_005cbaa4,uVar6,uVar10,uVar7);
      if ((iVar5 != 0) && (iVar5 % 5 == 0)) {
        FUN_0041ee50(s_Press_any_key_005cbab4);
        FUN_0043f4e0();
      }
      iVar5 = iVar5 + 1;
      puVar14 = puStack_9c;
    } while (iVar5 < *(short *)(puStack_9c + 7));
    return 0;
  }
  uVar12 = 0xffffffff;
  pcVar16 = *(char **)(param_2 + 0x28);
  do {
    pcVar17 = pcVar16;
    if (uVar12 == 0) break;
    uVar12 = uVar12 - 1;
    pcVar17 = pcVar16 + 1;
    cVar1 = *pcVar16;
    pcVar16 = pcVar17;
  } while (cVar1 != '\0');
  uVar12 = ~uVar12;
  pcVar16 = pcVar17 + -uVar12;
  pcVar17 = (char *)&DAT_00654a88;
  for (uVar13 = uVar12 >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
    *(undefined4 *)pcVar17 = *(undefined4 *)pcVar16;
    pcVar16 = pcVar16 + 4;
    pcVar17 = pcVar17 + 4;
  }
  for (uVar12 = uVar12 & 3; uVar12 != 0; uVar12 = uVar12 - 1) {
    *pcVar17 = *pcVar16;
    pcVar16 = pcVar16 + 1;
    pcVar17 = pcVar17 + 1;
  }
  FUN_00479580();
  puStack_94 = puVar14 + 5;
  iVar5 = FUN_00474210(&DAT_00654a88);
  puStack_98 = puVar14 + 7;
  iVar8 = FUN_00474210(&DAT_00654a88);
  if ((iVar5 < 0) && (iVar8 < 0)) {
    FUN_0041ee50(s_No_stat_by_that_name_in_that_obj_005cbac4);
    return 0;
  }
  iVar9 = FUN_00479700(s_delete_005cbaf4,0);
  if (iVar9 != 0) {
    FUN_00479580();
    if (-1 < iVar5) {
      FUN_00475850(iVar5);
    }
    if (-1 < iVar8) {
      FUN_00475e70(iVar8);
    }
    FUN_0041ee50(s_Stat_Deleted_005cbafc);
    return 0;
  }
  iVar9 = FUN_00479700(&DAT_005cbb0c,0);
  if (iVar9 != 0) {
    FUN_00479580();
    iVar9 = FUN_0047a410(param_2,&DAT_005cbb10,&puStack_9c);
    if ((iVar9 == 0) &&
       (puStack_9c = (undefined4 *)FUN_00497800(*(undefined4 *)(param_2 + 0x28),param_3),
       puStack_9c == (undefined4 *)0xfeced300)) {
      return 4;
    }
    (**(code **)(*param_1 + 0xe0))(&DAT_00654a88,puStack_9c);
    FUN_0041ee50(s_Stat_Set__005cbb14);
  }
  iVar9 = FUN_00479700(s_reset_005cbb20,0);
  if (iVar9 == 0) goto LAB_004244cc;
  FUN_00479580();
  if (iVar5 < 0) {
    if (-1 < iVar8) {
      iVar9 = *(int *)(*(int *)(param_1[0x12] + 0x34) + *(short *)((int)param_1 + 6) * 4);
      if (iVar9 == 0) {
        iVar9 = *(int *)(param_1[0x12] + 0x38);
      }
      puStack_9c = (undefined4 *)(*(int *)(iVar9 + 0x10) + iVar8 * 4);
      uVar10 = FUN_00429970(iVar8);
      goto LAB_004244bd;
    }
  }
  else {
    iVar9 = *(int *)(puVar14[0xd] + *(short *)((int)param_1 + 6) * 4);
    if (iVar9 == 0) {
      iVar9 = puVar14[0xe];
    }
    puStack_9c = (undefined4 *)(*(int *)(iVar9 + 0x10) + iVar5 * 4);
    uVar10 = FUN_00429970(iVar5);
LAB_004244bd:
    *puStack_9c = uVar10;
  }
  FUN_0041ee50(s_Stat_Reset__005cbb28);
LAB_004244cc:
  if (*(int *)(param_2 + 0x10) != 10) {
    return 4;
  }
  if (iVar5 < 0) {
    if (iVar8 < 0) {
      return 0;
    }
    iVar5 = *(int *)(puVar14[0xd] + *(short *)((int)param_1 + 6) * 4);
    if (iVar5 == 0) {
      iVar5 = puVar14[0xe];
    }
    uVar10 = *(undefined4 *)(*(int *)(iVar5 + 0x18) + iVar8 * 4);
    uVar6 = FUN_004741b0(iVar8,&DAT_00654a88);
    uVar7 = (**(code **)(*param_1 + 0xdc))(iVar8);
    FUN_0041ee50(s__s___d_____d_005cbb44,uVar6,uVar10,uVar7);
    return 0;
  }
  iVar8 = *(int *)(puVar14[0xd] + *(short *)((int)param_1 + 6) * 4);
  if (iVar8 == 0) {
    iVar8 = puVar14[0xe];
  }
  uVar10 = *(undefined4 *)(*(int *)(iVar8 + 0x10) + iVar5 * 4);
  uVar6 = FUN_004741b0(iVar5,&DAT_00654a88);
  FUN_0041ee50(s__s____d_005cbb38,uVar6,uVar10);
  return 0;
}


