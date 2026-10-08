// FUN_0052da90 @ 0052da90 size=5495

undefined4 __thiscall
FUN_0052da90(undefined4 *param_1,char *param_2,byte param_3,undefined4 param_4)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  char *pcVar6;
  int iVar7;
  int iVar8;
  char *pcVar9;
  uint uVar10;
  uint uVar11;
  int *piVar12;
  char *pcVar13;
  char *pcVar14;
  char local_7d;
  uint local_7c;
  int local_78;
  char local_68 [52];
  char local_34 [52];
  
  param_1[0x11] = 0;
  param_1[2] = 0;
  local_7c = 0;
  piVar12 = &DAT_0065a148;
  while (((DAT_0065a258 <= local_7c || (iVar2 = *piVar12, iVar2 == 0)) ||
         (iVar3 = FUN_00475210(param_2,0), iVar3 < 0))) {
    piVar12 = piVar12 + 1;
    local_7c = local_7c + 1;
    if (0x65a247 < (int)piVar12) {
      return 0;
    }
  }
  iVar3 = *(int *)(iVar2 + 8);
  uVar4 = FUN_00475210(param_2,0);
  uVar5 = FUN_00482fb0(100);
  param_1[1] = uVar5;
  FUN_0046e7f0(uVar5,100,param_2);
  uVar10 = 0xffffffff;
  pcVar6 = param_2;
  do {
    if (uVar10 == 0) break;
    uVar10 = uVar10 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  pcVar6 = (char *)FUN_00482ef0(~uVar10);
  uVar10 = 0xffffffff;
  do {
    pcVar9 = param_2;
    if (uVar10 == 0) break;
    uVar10 = uVar10 - 1;
    pcVar9 = param_2 + 1;
    cVar1 = *param_2;
    param_2 = pcVar9;
  } while (cVar1 != '\0');
  uVar10 = ~uVar10;
  pcVar9 = pcVar9 + -uVar10;
  pcVar13 = pcVar6;
  for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
    *(undefined4 *)pcVar13 = *(undefined4 *)pcVar9;
    pcVar9 = pcVar9 + 4;
    pcVar13 = pcVar13 + 4;
  }
  for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
    *pcVar13 = *pcVar9;
    pcVar9 = pcVar9 + 1;
    pcVar13 = pcVar13 + 1;
  }
  param_1[2] = pcVar6;
  uVar10 = FUN_00474210(s_Value_005e380c);
  if (uVar4 < *(uint *)(iVar2 + 0x24)) {
    iVar7 = *(int *)(*(int *)(iVar2 + 0x34) + uVar4 * 4);
    if (iVar7 == 0) {
      iVar7 = *(int *)(iVar2 + 0x38);
    }
    if ((uint)(int)*(short *)(iVar7 + 0xc) <= uVar10) goto LAB_0052dba3;
    iVar7 = *(int *)(*(int *)(iVar2 + 0x34) + uVar4 * 4);
    if (iVar7 == 0) {
      iVar7 = *(int *)(iVar2 + 0x38);
    }
    uVar5 = *(undefined4 *)(*(int *)(iVar7 + 0x10) + uVar10 * 4);
  }
  else {
LAB_0052dba3:
    uVar5 = 0;
  }
  param_1[0xe] = uVar5;
  param_1[0xf] = param_4;
  iVar7 = FUN_0048cb50(iVar3,uVar4);
  if (iVar7 != 0) {
    if (iVar3 != 1) {
      if (iVar3 != 2) {
        *param_1 = 0;
        param_1[0xd] = 0;
        goto LAB_0052ef97;
      }
      iVar3 = iVar7 + 0x20;
      param_1[0xd] = 5;
      if ((iVar3 == 0) || (iVar8 = FUN_0049d6d0(iVar3), iVar8 == -1)) {
        if (*(int *)(iVar7 + 200) == 0) {
          *param_1 = 0;
        }
        else {
          uVar5 = FUN_0048cce0(*(int *)(iVar7 + 200));
          *param_1 = uVar5;
        }
      }
      else {
        pcVar6 = (char *)FUN_0049d800(iVar3);
        pcVar9 = (char *)FUN_0049d800(iVar3);
        uVar10 = 0xffffffff;
        do {
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          cVar1 = *pcVar9;
          pcVar9 = pcVar9 + 1;
        } while (cVar1 != '\0');
        pcVar9 = (char *)FUN_00482ef0(~uVar10);
        uVar10 = 0xffffffff;
        do {
          pcVar13 = pcVar6;
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          pcVar13 = pcVar6 + 1;
          cVar1 = *pcVar6;
          pcVar6 = pcVar13;
        } while (cVar1 != '\0');
        uVar10 = ~uVar10;
        pcVar6 = pcVar13 + -uVar10;
        pcVar13 = pcVar9;
        for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
          *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
          pcVar6 = pcVar6 + 4;
          pcVar13 = pcVar13 + 4;
        }
        for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
          *pcVar13 = *pcVar6;
          pcVar6 = pcVar6 + 1;
          pcVar13 = pcVar13 + 1;
        }
        *param_1 = pcVar9;
      }
      param_1[4] = *(undefined4 *)(iVar7 + 0xa4);
      iVar3 = FUN_0049d6d0(s_BSARM1_005e3814);
      if (iVar3 == -1) {
        uVar10 = 0xffffffff;
        pcVar6 = s_Protection_005e3838;
        do {
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          cVar1 = *pcVar6;
          pcVar6 = pcVar6 + 1;
        } while (cVar1 != '\0');
        pcVar9 = (char *)FUN_00482ef0(~uVar10);
        pcVar6 = s_Protection_005e382c;
      }
      else {
        pcVar6 = (char *)FUN_0049d800(s_BSARM1_005e381c);
        pcVar9 = (char *)FUN_0049d800(s_BSARM1_005e3824);
        uVar10 = 0xffffffff;
        do {
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          cVar1 = *pcVar9;
          pcVar9 = pcVar9 + 1;
        } while (cVar1 != '\0');
        pcVar9 = (char *)FUN_00482ef0(~uVar10);
      }
      uVar10 = 0xffffffff;
      do {
        pcVar13 = pcVar6;
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        pcVar13 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar13;
      } while (cVar1 != '\0');
      uVar10 = ~uVar10;
      pcVar6 = pcVar13 + -uVar10;
      pcVar13 = pcVar9;
      for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
        *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
        pcVar6 = pcVar6 + 4;
        pcVar13 = pcVar13 + 4;
      }
      for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
        *pcVar13 = *pcVar6;
        pcVar6 = pcVar6 + 1;
        pcVar13 = pcVar13 + 1;
      }
      uVar5 = *(undefined4 *)(iVar7 + 0xac);
      param_1[3] = pcVar9;
      param_1[6] = uVar5;
      iVar3 = FUN_0049d6d0(s_BSARM2_005e3844);
      if (iVar3 == -1) {
        uVar10 = 0xffffffff;
        pcVar6 = s_Rst_Poison_005e3868;
        do {
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          cVar1 = *pcVar6;
          pcVar6 = pcVar6 + 1;
        } while (cVar1 != '\0');
        pcVar9 = (char *)FUN_00482ef0(~uVar10);
        pcVar6 = s_Rst_Poison_005e385c;
      }
      else {
        pcVar6 = (char *)FUN_0049d800(s_BSARM2_005e384c);
        pcVar9 = (char *)FUN_0049d800(s_BSARM2_005e3854);
        uVar10 = 0xffffffff;
        do {
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          cVar1 = *pcVar9;
          pcVar9 = pcVar9 + 1;
        } while (cVar1 != '\0');
        pcVar9 = (char *)FUN_00482ef0(~uVar10);
      }
      uVar10 = 0xffffffff;
      do {
        pcVar13 = pcVar6;
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        pcVar13 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar13;
      } while (cVar1 != '\0');
      uVar10 = ~uVar10;
      pcVar6 = pcVar13 + -uVar10;
      pcVar13 = pcVar9;
      for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
        *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
        pcVar6 = pcVar6 + 4;
        pcVar13 = pcVar13 + 4;
      }
      for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
        *pcVar13 = *pcVar6;
        pcVar6 = pcVar6 + 1;
        pcVar13 = pcVar13 + 1;
      }
      uVar5 = *(undefined4 *)(iVar7 + 0xb4);
      param_1[5] = pcVar9;
      param_1[8] = uVar5;
      iVar3 = FUN_0049d6d0(s_BSARM3_005e3874);
      if (iVar3 == -1) {
        uVar10 = 0xffffffff;
        pcVar6 = &DAT_005e3894;
        do {
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          cVar1 = *pcVar6;
          pcVar6 = pcVar6 + 1;
        } while (cVar1 != '\0');
        pcVar9 = (char *)FUN_00482ef0(~uVar10);
        pcVar6 = s_Stlth_005e388c;
      }
      else {
        pcVar6 = (char *)FUN_0049d800(s_BSARM3_005e387c);
        pcVar9 = (char *)FUN_0049d800(s_BSARM3_005e3884);
        uVar10 = 0xffffffff;
        do {
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          cVar1 = *pcVar9;
          pcVar9 = pcVar9 + 1;
        } while (cVar1 != '\0');
        pcVar9 = (char *)FUN_00482ef0(~uVar10);
      }
      uVar10 = 0xffffffff;
      do {
        pcVar13 = pcVar6;
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        pcVar13 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar13;
      } while (cVar1 != '\0');
      uVar10 = ~uVar10;
      pcVar6 = pcVar13 + -uVar10;
      pcVar13 = pcVar9;
      for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
        *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
        pcVar6 = pcVar6 + 4;
        pcVar13 = pcVar13 + 4;
      }
      for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
        *pcVar13 = *pcVar6;
        pcVar6 = pcVar6 + 1;
        pcVar13 = pcVar13 + 1;
      }
      uVar5 = *(undefined4 *)(iVar7 + 0xb8);
      param_1[7] = pcVar9;
      param_1[10] = uVar5;
      iVar3 = FUN_0049d6d0(s_BSARM4_005e389c);
      if (iVar3 == -1) {
        uVar10 = 0xffffffff;
        pcVar6 = s_Min_Strn_005e38c0;
        do {
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          cVar1 = *pcVar6;
          pcVar6 = pcVar6 + 1;
        } while (cVar1 != '\0');
        pcVar9 = (char *)FUN_00482ef0(~uVar10);
        pcVar6 = s_Min_Strn_005e38b4;
      }
      else {
        pcVar6 = (char *)FUN_0049d800(s_BSARM4_005e38a4);
        pcVar9 = (char *)FUN_0049d800(s_BSARM4_005e38ac);
        uVar10 = 0xffffffff;
        do {
          if (uVar10 == 0) break;
          uVar10 = uVar10 - 1;
          cVar1 = *pcVar9;
          pcVar9 = pcVar9 + 1;
        } while (cVar1 != '\0');
        pcVar9 = (char *)FUN_00482ef0(~uVar10);
      }
      uVar10 = 0xffffffff;
      do {
        pcVar13 = pcVar6;
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        pcVar13 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar13;
      } while (cVar1 != '\0');
      uVar10 = ~uVar10;
      pcVar6 = pcVar13 + -uVar10;
      pcVar13 = pcVar9;
      for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
        *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
        pcVar6 = pcVar6 + 4;
        pcVar13 = pcVar13 + 4;
      }
      for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
        *pcVar13 = *pcVar6;
        pcVar6 = pcVar6 + 1;
        pcVar13 = pcVar13 + 1;
      }
      uVar5 = *(undefined4 *)(iVar7 + 0xbc);
      param_1[9] = pcVar9;
      param_1[0xc] = uVar5;
      iVar3 = FUN_0049d6d0(s_BSARM5_005e38cc);
      if (iVar3 != -1) {
        pcVar6 = (char *)FUN_0049d800(s_BSARM5_005e38d4);
        pcVar9 = s_BSARM5_005e38dc;
        goto LAB_0052e4cd;
      }
      uVar10 = 0xffffffff;
      pcVar6 = s_Min_Cons_005e38f0;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Min_Cons_005e38e4;
      goto LAB_0052e4ef;
    }
    iVar3 = iVar7 + 0x20;
    param_1[0xd] = 2;
    if ((iVar3 == 0) || (iVar8 = FUN_0049d6d0(iVar3), iVar8 == -1)) {
      if (*(int *)(iVar7 + 0xcc) == 0) {
        *param_1 = 0;
      }
      else {
        uVar5 = FUN_0048cce0(*(int *)(iVar7 + 0xcc));
        *param_1 = uVar5;
      }
    }
    else {
      pcVar6 = (char *)FUN_0049d800(iVar3);
      pcVar9 = (char *)FUN_0049d800(iVar3);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      uVar10 = 0xffffffff;
      do {
        pcVar13 = pcVar6;
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        pcVar13 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar13;
      } while (cVar1 != '\0');
      uVar10 = ~uVar10;
      pcVar6 = pcVar13 + -uVar10;
      pcVar13 = pcVar9;
      for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
        *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
        pcVar6 = pcVar6 + 4;
        pcVar13 = pcVar13 + 4;
      }
      for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
        *pcVar13 = *pcVar6;
        pcVar6 = pcVar6 + 1;
        pcVar13 = pcVar13 + 1;
      }
      *param_1 = pcVar9;
    }
    param_1[4] = *(undefined4 *)(iVar7 + 0xa8);
    iVar3 = FUN_0049d6d0(s_BSWEA1_005e38fc);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Damage_005e391c;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Damage_005e3914;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSWEA1_005e3904);
      pcVar9 = (char *)FUN_0049d800(s_BSWEA1_005e390c);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    uVar5 = *(undefined4 *)(iVar7 + 0xbc);
    param_1[3] = pcVar9;
    param_1[6] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSWEA2_005e3924);
    if (iVar3 != -1) {
      pcVar6 = (char *)FUN_0049d800(s_BSWEA2_005e392c);
      pcVar9 = s_BSWEA2_005e3934;
      goto LAB_0052e68f;
    }
    uVar10 = 0xffffffff;
    pcVar6 = s_Min_Strength_005e394c;
    do {
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    pcVar9 = (char *)FUN_00482ef0(~uVar10);
    pcVar6 = s_Min_Strength_005e393c;
LAB_0052e6b1:
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[5] = pcVar9;
    goto LAB_0052ef97;
  }
  switch(iVar3) {
  case 1:
    param_1[0xd] = 2;
    *param_1 = 0;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_Damage_005e3a84);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052e584;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052e584:
      uVar5 = 0;
    }
    param_1[4] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSWEA1_005e3a8c);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Damage_005e3aac;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Damage_005e3aa4;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSWEA1_005e3a94);
      pcVar9 = (char *)FUN_0049d800(s_BSWEA1_005e3a9c);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[3] = pcVar9;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_MinStrength_005e3ab4);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052e660;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052e660:
      uVar5 = 0;
    }
    param_1[6] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSWEA2_005e3ac0);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Min_Strength_005e3ae8;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Min_Strength_005e3ad8;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSWEA2_005e3ac8);
      pcVar9 = s_BSWEA2_005e3ad0;
LAB_0052e68f:
      pcVar9 = (char *)FUN_0049d800(pcVar9);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    goto LAB_0052e6b1;
  case 2:
    *param_1 = 0;
    param_1[0xd] = 5;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_Protection_005e395c);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052e12e;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052e12e:
      uVar5 = 0;
    }
    param_1[4] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSARM1_005e3968);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Protection_005e398c;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Protection_005e3980;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSARM1_005e3970);
      pcVar9 = (char *)FUN_0049d800(s_BSARM1_005e3978);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[3] = pcVar9;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_ResistPoison_005e3998);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052e20a;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052e20a:
      uVar5 = 0;
    }
    param_1[6] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSARM2_005e39a8);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Rst_Poison_005e39cc;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Rst_Poison_005e39c0;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSARM2_005e39b0);
      pcVar9 = (char *)FUN_0049d800(s_BSARM2_005e39b8);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[5] = pcVar9;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_Stealth_005e39d8);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052e2e6;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052e2e6:
      uVar5 = 0;
    }
    param_1[8] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSARM3_005e39e0);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = &DAT_005e3a00;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Stlth_005e39f8;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSARM3_005e39e8);
      pcVar9 = (char *)FUN_0049d800(s_BSARM3_005e39f0);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[7] = pcVar9;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_MinStrength_005e3a08);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052e3c2;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052e3c2:
      uVar5 = 0;
    }
    param_1[10] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSARM4_005e3a14);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Min_Strn_005e3a38;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Min_Strn_005e3a2c;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSARM4_005e3a1c);
      pcVar9 = (char *)FUN_0049d800(s_BSARM4_005e3a24);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[9] = pcVar9;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_MinConstitution_005e3a44);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052e49e;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052e49e:
      uVar5 = 0;
    }
    param_1[0xc] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSARM4_005e3a54);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Min_Cons_005e3a78;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Min_Cons_005e3a6c;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSARM4_005e3a5c);
      pcVar9 = s_BSARM4_005e3a64;
LAB_0052e4cd:
      pcVar9 = (char *)FUN_0049d800(pcVar9);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
LAB_0052e4ef:
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[0xb] = pcVar9;
    break;
  default:
    param_1[0xd] = 0;
    *param_1 = 0;
    break;
  case 4:
    *param_1 = 0;
    param_1[0xd] = 4;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_Health_005e3af8);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052e74a;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052e74a:
      uVar5 = 0;
    }
    param_1[4] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSFOOD1_005e3b00);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Cure_Health_005e3b24;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Cure_Health_005e3b18;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSFOOD1_005e3b08);
      pcVar9 = (char *)FUN_0049d800(s_BSFOOD1_005e3b10);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[3] = pcVar9;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(&DAT_005e3b30);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052e826;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052e826:
      uVar5 = 0;
    }
    param_1[6] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSFOOD2_005e3b38);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Cure_Mana_005e3b5c;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Cure_Mana_005e3b50;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSFOOD2_005e3b40);
      pcVar9 = (char *)FUN_0049d800(s_BSFOOD2_005e3b48);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[5] = pcVar9;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_Fatigue_005e3b68);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052e902;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052e902:
      uVar5 = 0;
    }
    param_1[8] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSFOOD3_005e3b70);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Cure_Fatigue_005e3b98;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Cure_Fatigue_005e3b88;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSFOOD3_005e3b78);
      pcVar9 = (char *)FUN_0049d800(s_BSFOOD3_005e3b80);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[7] = pcVar9;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_Poison_005e3ba8);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052e9de;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052e9de:
      uVar5 = 0;
    }
    param_1[10] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSFOOD4_005e3bb0);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Cure_Poison_005e3bd4;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Cure_Poison_005e3bc8;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSFOOD4_005e3bb8);
      pcVar9 = s_BSFOOD4_005e3bc0;
LAB_0052ea0d:
      pcVar9 = (char *)FUN_0049d800(pcVar9);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    goto LAB_0052ed91;
  case 0x12:
    *param_1 = 0;
    param_1[0xd] = 4;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_Health_005e3be0);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052eaab;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052eaab:
      uVar5 = 0;
    }
    param_1[4] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSFOOD1_005e3be8);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Cure_Health_005e3c0c;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Cure_Health_005e3c00;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSFOOD1_005e3bf0);
      pcVar9 = (char *)FUN_0049d800(s_BSFOOD1_005e3bf8);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[3] = pcVar9;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(&DAT_005e3c18);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052eb87;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052eb87:
      uVar5 = 0;
    }
    param_1[6] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSFOOD2_005e3c20);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Cure_Mana_005e3c44;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Cure_Mana_005e3c38;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSFOOD2_005e3c28);
      pcVar9 = (char *)FUN_0049d800(s_BSFOOD2_005e3c30);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[5] = pcVar9;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_Fatigue_005e3c50);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052ec63;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052ec63:
      uVar5 = 0;
    }
    param_1[8] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSFOOD3_005e3c58);
    if (iVar3 == -1) {
      uVar10 = 0xffffffff;
      pcVar6 = s_Cure_Fatigue_005e3c80;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      pcVar6 = s_Cure_Fatigue_005e3c70;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSFOOD3_005e3c60);
      pcVar9 = (char *)FUN_0049d800(s_BSFOOD3_005e3c68);
      uVar10 = 0xffffffff;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[7] = pcVar9;
    uVar10 = FUN_00475210(param_1[2],0);
    uVar11 = FUN_00474210(s_Poison_005e3c90);
    if (uVar10 < *(uint *)(iVar2 + 0x24)) {
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar11) goto LAB_0052ed3f;
      iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar10 * 4);
      if (iVar3 == 0) {
        iVar3 = *(int *)(iVar2 + 0x38);
      }
      uVar5 = *(undefined4 *)(*(int *)(iVar3 + 0x10) + uVar11 * 4);
    }
    else {
LAB_0052ed3f:
      uVar5 = 0;
    }
    param_1[10] = uVar5;
    iVar3 = FUN_0049d6d0(s_BSFOOD4_005e3c98);
    if (iVar3 != -1) {
      pcVar6 = (char *)FUN_0049d800(s_BSFOOD4_005e3ca0);
      pcVar9 = s_BSFOOD4_005e3ca8;
      goto LAB_0052ea0d;
    }
    uVar10 = 0xffffffff;
    pcVar6 = s_Cure_Poison_005e3cbc;
    do {
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    pcVar9 = (char *)FUN_00482ef0(~uVar10);
    pcVar6 = s_Cure_Poison_005e3cb0;
LAB_0052ed91:
    uVar10 = 0xffffffff;
    do {
      pcVar13 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar13 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar13;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar13 + -uVar10;
    pcVar13 = pcVar9;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar13 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar13 = pcVar13 + 1;
    }
    param_1[9] = pcVar9;
    break;
  case 0x15:
    iVar3 = FUN_0049d6d0(s_BSAMMO1_005e3cc8);
    if (iVar3 == -1) {
      pcVar6 = s_Quiver_of_NUMBER_NAME_s_005e3cd8;
    }
    else {
      pcVar6 = (char *)FUN_0049d800(s_BSAMMO1_005e3cd0);
    }
    uVar10 = 0xffffffff;
    do {
      pcVar9 = pcVar6;
      if (uVar10 == 0) break;
      uVar10 = uVar10 - 1;
      pcVar9 = pcVar6 + 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar9;
    } while (cVar1 != '\0');
    uVar10 = ~uVar10;
    pcVar6 = pcVar9 + -uVar10;
    pcVar9 = local_34;
    for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined4 *)pcVar9 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar9 = pcVar9 + 4;
    }
    for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
      *pcVar9 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar9 = pcVar9 + 1;
    }
    pcVar6 = (char *)FUN_0058ad30(local_34,s_NUMBER_005e3cf0);
    pcVar9 = (char *)FUN_0058ad30(local_34,&DAT_005e3cf8);
    local_7c = 0;
    local_78 = 0;
    if (param_1[1] != 0) {
      do {
        if (0x31 < local_78) break;
        local_7d = local_34[local_7c];
        pcVar13 = local_34 + local_7c;
        if (local_7d == '\0') break;
        if (pcVar6 == pcVar13) {
          local_68[local_78] = '\0';
          FUN_0058b100(local_68,&DAT_005e3d00,local_68,param_1[0xf]);
          cVar1 = *pcVar13;
          for (; ((cVar1 != '\0' && (cVar1 != 'R')) && ((int)local_7c < 0x32));
              local_7c = local_7c + 1) {
            cVar1 = local_34[local_7c + 1];
          }
          uVar10 = 0xffffffff;
          pcVar13 = local_68;
          do {
            if (uVar10 == 0) break;
            uVar10 = uVar10 - 1;
            cVar1 = *pcVar13;
            pcVar13 = pcVar13 + 1;
          } while (cVar1 != '\0');
          local_78 = ~uVar10 - 1;
        }
        else if (pcVar9 == pcVar13) {
          pcVar13 = (char *)param_1[1];
          uVar10 = 0xffffffff;
          iVar3 = 0;
          pcVar14 = pcVar13;
          do {
            if (uVar10 == 0) break;
            uVar10 = uVar10 - 1;
            cVar1 = *pcVar14;
            pcVar14 = pcVar14 + 1;
          } while (cVar1 != '\0');
          if (0 < (int)(~uVar10 - 1)) {
            do {
              pcVar14 = pcVar13 + iVar3;
              iVar3 = iVar3 + 1;
              uVar10 = 0xffffffff;
              local_68[local_78] = *pcVar14;
              local_78 = local_78 + 1;
              pcVar14 = pcVar13;
              do {
                if (uVar10 == 0) break;
                uVar10 = uVar10 - 1;
                cVar1 = *pcVar14;
                pcVar14 = pcVar14 + 1;
              } while (cVar1 != '\0');
            } while (iVar3 < (int)(~uVar10 - 1));
          }
          do {
            if ((local_7d == 'E') || (0x31 < (int)local_7c)) break;
            local_7d = local_34[local_7c + 1];
            local_7c = local_7c + 1;
          } while (local_7d != '\0');
        }
        else {
          local_68[local_78] = local_7d;
          local_78 = local_78 + 1;
        }
        local_7c = local_7c + 1;
      } while ((int)local_7c < 0x32);
      local_68[local_78] = '\0';
      uVar10 = 0xffffffff;
      pcVar6 = local_68;
      do {
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      pcVar9 = (char *)FUN_00482ef0(~uVar10);
      uVar10 = 0xffffffff;
      pcVar6 = local_68;
      do {
        pcVar13 = pcVar6;
        if (uVar10 == 0) break;
        uVar10 = uVar10 - 1;
        pcVar13 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar13;
      } while (cVar1 != '\0');
      uVar10 = ~uVar10;
      pcVar6 = pcVar13 + -uVar10;
      pcVar13 = pcVar9;
      for (uVar11 = uVar10 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
        *(undefined4 *)pcVar13 = *(undefined4 *)pcVar6;
        pcVar6 = pcVar6 + 4;
        pcVar13 = pcVar13 + 4;
      }
      for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
        *pcVar13 = *pcVar6;
        pcVar6 = pcVar6 + 1;
        pcVar13 = pcVar13 + 1;
      }
      param_1[1] = pcVar9;
    }
    param_1[0xd] = 0;
    *param_1 = 0;
  }
LAB_0052ef97:
  if (((*(int *)(iVar2 + 0x34) == 0) || (*(uint *)(iVar2 + 0x24) <= uVar4)) ||
     (*(int *)(*(int *)(iVar2 + 0x34) + uVar4 * 4) == 0)) {
    iVar3 = 0;
  }
  else {
    iVar3 = *(int *)(*(int *)(iVar2 + 0x34) + uVar4 * 4);
    if (iVar3 == 0) {
      iVar3 = *(int *)(iVar2 + 0x38);
    }
  }
  uVar5 = FUN_00446b10(*(undefined4 *)(iVar3 + 8),1);
  param_1[0x10] = uVar5;
  if ((param_3 & 2) != 0) {
    uVar5 = __ftol();
    param_1[0xe] = uVar5;
  }
  return 1;
}


