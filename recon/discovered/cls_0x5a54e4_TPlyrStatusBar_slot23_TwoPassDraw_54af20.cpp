// FUN_0054af20 @ 0054af20 size=3909

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0054af20(int param_1)

{
  char cVar1;
  bool bVar2;
  undefined *puVar3;
  int iVar4;
  undefined4 uVar5;
  char *pcVar6;
  char *pcVar7;
  undefined4 uVar8;
  int iVar9;
  int *piVar10;
  uint uVar11;
  uint uVar12;
  char *pcVar13;
  undefined4 uVar14;
  undefined4 uVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  undefined4 uVar19;
  undefined4 uVar20;
  undefined4 uVar21;
  undefined4 uVar22;
  undefined4 uVar23;
  undefined4 uStack_114;
  char *pcStack_110;
  undefined1 *puStack_10c;
  int *piStack_108;
  char *pcStack_104;
  char *pcStack_f0;
  int *piStack_e8;
  char acStack_e4 [8];
  int iStack_dc;
  undefined4 uStack_d8;
  undefined4 uStack_d4;
  undefined4 uStack_d0;
  int iStack_cc;
  undefined4 uStack_c8;
  undefined4 uStack_c4;
  undefined4 uStack_c0;
  int iStack_bc;
  undefined4 uStack_b8;
  undefined4 uStack_b4;
  undefined4 uStack_b0;
  undefined4 uStack_ac;
  undefined4 uStack_a8;
  undefined4 uStack_a4;
  undefined4 uStack_a0;
  undefined4 uStack_9c;
  undefined4 uStack_98;
  undefined2 uStack_94;
  undefined2 uStack_92;
  undefined1 auStack_80 [128];
  
  if (DAT_00667fcc == (int *)0x0) {
    return;
  }
  piVar10 = (int *)DAT_00667fcc[0x38];
  if ((piVar10 == (int *)0x0) ||
     ((*piVar10 != 3 && ((piVar10 == (int *)0x0 || (*piVar10 != 0x19)))))) {
    piStack_e8 = (int *)0x0;
  }
  else {
    piStack_e8 = (int *)piVar10[0x11];
  }
  pcStack_104 = (char *)0x54af7d;
  iVar4 = (**(code **)(*DAT_00667fcc + 0x1c0))();
  if (*(int *)(param_1 + 0x98) != iVar4) {
    pcStack_104 = (char *)0x54af97;
    pcStack_104 = (char *)(**(code **)(*DAT_00667fcc + 0x1c0))();
    puStack_10c = auStack_80;
    piStack_108 = (int *)&DAT_005e57e8;
    pcStack_110 = (char *)0x54afaa;
    FUN_0058b100();
    pcStack_104 = (char *)0x80000000;
    piStack_108 = (int *)0x7f7f;
    puStack_10c = (undefined1 *)0xffff;
    pcStack_110 = (char *)_DAT_006668d0;
    uStack_114 = 0xe;
    (**(code **)(**(int **)(param_1 + 100) + 100))(0,0,0x32);
    pcStack_104 = (char *)0x80000000;
    piStack_108 = (int *)0x401;
    puStack_10c = (undefined1 *)0xff;
    pcStack_110 = (char *)0xff;
    uStack_114 = 0xff;
    pcStack_110 = (char *)FUN_00429950();
    uStack_114 = 0x54affb;
    FUN_00419dd0();
    pcStack_110 = DAT_0065abc4;
    uStack_114 = 0;
    FUN_004be2b0(0,0,0x32,0xe,auStack_80);
  }
  pcStack_104 = (char *)0x54b027;
  iVar4 = (**(code **)(*DAT_00667fcc + 0x1d0))();
  if (*(int *)(param_1 + 0x9c) != iVar4) {
    pcStack_104 = (char *)0x54b041;
    pcStack_104 = (char *)(**(code **)(*DAT_00667fcc + 0x1d0))();
    puStack_10c = auStack_80;
    piStack_108 = (int *)&DAT_005e57ec;
    pcStack_110 = (char *)0x54b054;
    FUN_0058b100();
    pcStack_104 = (char *)0x80000000;
    piStack_108 = (int *)0x7f7f;
    puStack_10c = (undefined1 *)0xffff;
    pcStack_110 = (char *)_DAT_006668d0;
    uStack_114 = 0xe;
    (**(code **)(**(int **)(param_1 + 100) + 100))(0,0xe,0x32);
    pcStack_104 = (char *)0x80000000;
    piStack_108 = (int *)0x401;
    puStack_10c = (undefined1 *)0xff;
    pcStack_110 = (char *)0xff;
    uStack_114 = 0xff;
    pcStack_110 = (char *)FUN_00429950();
    uStack_114 = 0x54b0a6;
    FUN_00419dd0();
    pcStack_110 = DAT_0065abc4;
    uStack_114 = 0;
    FUN_004be2b0(0,0xe,0x32,0xe,auStack_80);
  }
  pcStack_104 = (char *)0x54b0d3;
  iVar4 = (**(code **)(*DAT_00667fcc + 0x1c8))();
  if (*(int *)(param_1 + 0xa0) != iVar4) {
    pcStack_104 = (char *)0x54b0ed;
    pcStack_104 = (char *)(**(code **)(*DAT_00667fcc + 0x1c8))();
    puStack_10c = auStack_80;
    piStack_108 = (int *)&DAT_005e57f0;
    pcStack_110 = (char *)0x54b100;
    FUN_0058b100();
    pcStack_104 = (char *)0x80000000;
    piStack_108 = (int *)0x7f7f;
    puStack_10c = (undefined1 *)0xffff;
    pcStack_110 = (char *)_DAT_006668d0;
    uStack_114 = 0xe;
    (**(code **)(**(int **)(param_1 + 100) + 100))(0,0x1c,0x32);
    pcStack_104 = (char *)0x80000000;
    piStack_108 = (int *)0x401;
    puStack_10c = (undefined1 *)0xff;
    pcStack_110 = (char *)0xff;
    uStack_114 = 0xff;
    pcStack_110 = (char *)FUN_00429950();
    uStack_114 = 0x54b152;
    FUN_00419dd0();
    pcStack_110 = DAT_0065abc4;
    uStack_114 = 0;
    FUN_004be2b0(0,0x1c,0x32,0xe,auStack_80);
  }
  if (*(int **)(param_1 + 0x94) == DAT_00667fcc) {
    pcStack_104 = (char *)0x54b189;
    iVar4 = (**(code **)(*DAT_00667fcc + 0x354))();
    if ((*(int *)(param_1 + 0xa4) != iVar4) ||
       ((DAT_00667fcc != (int *)0x0 && (DAT_00667fcc[0xe] != *(int *)(param_1 + 0xbc)))))
    goto LAB_0054b1ae;
    bVar2 = false;
  }
  else {
LAB_0054b1ae:
    bVar2 = true;
  }
  piVar10 = DAT_00667fcc;
  if (((DAT_0066829c != 0) && (DAT_00676828 != 0)) && (((byte)DAT_006767fc & 0x80) != 0)) {
    iVar4 = DAT_00667fcc[0x196];
    if (((*(int *)(param_1 + 0xc4) != DAT_00667fcc[0x194]) ||
        (*(int *)(param_1 + 200) != DAT_00667fcc[0x195])) ||
       ((*(int *)(param_1 + 0xcc) != iVar4 || (*(int *)(param_1 + 0xd0) != DAT_00667fcc[0x197])))) {
      bVar2 = true;
    }
    *(int *)(param_1 + 0xc4) = DAT_00667fcc[0x194];
    *(int *)(param_1 + 200) = piVar10[0x195];
    iVar9 = piVar10[0x197];
    *(int *)(param_1 + 0xcc) = iVar4;
    *(int *)(param_1 + 0xd0) = iVar9;
  }
  if (bVar2) {
    pcStack_104 = (char *)piVar10[0xe];
    puStack_10c = auStack_80;
    pcStack_110 = (char *)0x54b256;
    piStack_108 = piVar10;
    FUN_0054ae10();
    pcStack_104 = (char *)0x80000000;
    piStack_108 = (int *)0x7f7f;
    puStack_10c = (undefined1 *)0xffff;
    pcStack_110 = (char *)_DAT_006668d0;
    uStack_114 = 0x40;
    (**(code **)(**(int **)(param_1 + 100) + 100))(0x40,0,0x40);
    pcStack_104 = (char *)0x80000000;
    piStack_108 = (int *)0x402;
    puStack_10c = (undefined1 *)0xff;
    pcStack_110 = (char *)0xff;
    uStack_114 = 0xff;
    pcStack_110 = (char *)FUN_00429950();
    uStack_114 = 0x54b2a5;
    FUN_00419dd0();
    pcStack_110 = DAT_0065abc4;
    uStack_114 = 0;
    FUN_004be2b0(0x40,0,0x40,0x40,auStack_80);
    piVar10 = DAT_00667fcc;
    *(int *)(param_1 + 0xbc) = DAT_00667fcc[0xe];
  }
  iVar4 = *piVar10;
  *(int **)(param_1 + 0x94) = piVar10;
  pcStack_104 = (char *)0x54b2e1;
  uVar5 = (**(code **)(iVar4 + 0x1c0))();
  piVar10 = DAT_00667fcc;
  *(undefined4 *)(param_1 + 0x98) = uVar5;
  pcStack_104 = (char *)0x54b2f5;
  uVar5 = (**(code **)(*piVar10 + 0x1d0))();
  piVar10 = DAT_00667fcc;
  *(undefined4 *)(param_1 + 0x9c) = uVar5;
  pcStack_104 = (char *)0x54b309;
  uVar5 = (**(code **)(*piVar10 + 0x354))();
  piVar10 = DAT_00667fcc;
  *(undefined4 *)(param_1 + 0xa4) = uVar5;
  pcStack_104 = (char *)0x54b31d;
  uVar5 = (**(code **)(*piVar10 + 0x1c8))();
  *(undefined4 *)(param_1 + 0xa0) = uVar5;
  if (piStack_e8 != (int *)0x0) {
    pcStack_104 = (char *)0x54b335;
    iVar4 = (**(code **)(*piStack_e8 + 0x1c0))();
    if (*(int *)(param_1 + 0xac) != iVar4) {
      pcStack_104 = (char *)0x54b34b;
      pcStack_104 = (char *)(**(code **)(*piStack_e8 + 0x1c0))();
      puStack_10c = auStack_80;
      piStack_108 = (int *)&DAT_005e57f4;
      pcStack_110 = (char *)0x54b35e;
      FUN_0058b100();
      pcStack_104 = (char *)0x80000000;
      piStack_108 = (int *)0x7f7f;
      puStack_10c = (undefined1 *)0xffff;
      pcStack_110 = (char *)_DAT_006668d0;
      uStack_114 = 0xe;
      (**(code **)(**(int **)(param_1 + 100) + 100))(0,0x2a,0x32);
      pcStack_104 = (char *)0x80000000;
      piStack_108 = (int *)0x404;
      puStack_10c = (undefined1 *)0xff;
      pcStack_110 = (char *)0xff;
      uStack_114 = 0xff;
      pcStack_110 = (char *)FUN_00429950();
      uStack_114 = 0x54b3b0;
      FUN_00419dd0();
      pcStack_110 = DAT_0065abc4;
      uStack_114 = 0;
      FUN_004be2b0(0,0x2a,0x32,0xe,auStack_80);
    }
    pcStack_104 = (char *)0x54b3d9;
    iVar4 = (**(code **)(*piStack_e8 + 0x1d0))();
    if (*(int *)(param_1 + 0xb0) != iVar4) {
      pcStack_104 = (char *)0x54b3ef;
      pcStack_104 = (char *)(**(code **)(*piStack_e8 + 0x1d0))();
      puStack_10c = auStack_80;
      piStack_108 = (int *)&DAT_005e57f8;
      pcStack_110 = (char *)0x54b402;
      FUN_0058b100();
      pcStack_104 = (char *)0x80000000;
      piStack_108 = (int *)0x7f7f;
      puStack_10c = (undefined1 *)0xffff;
      pcStack_110 = (char *)_DAT_006668d0;
      uStack_114 = 0xe;
      (**(code **)(**(int **)(param_1 + 100) + 100))(0,0x38,0x32);
      pcStack_104 = (char *)0x80000000;
      piStack_108 = (int *)0x404;
      puStack_10c = (undefined1 *)0xff;
      pcStack_110 = (char *)0xff;
      uStack_114 = 0xff;
      pcStack_110 = (char *)FUN_00429950();
      uStack_114 = 0x54b454;
      FUN_00419dd0();
      pcStack_110 = DAT_0065abc4;
      uStack_114 = 0;
      FUN_004be2b0(0,0x38,0x32,0xe,auStack_80);
    }
    pcStack_104 = (char *)0x54b47d;
    iVar4 = (**(code **)(*piStack_e8 + 0x1c8))();
    if (*(int *)(param_1 + 0xb4) != iVar4) {
      pcStack_104 = (char *)0x54b493;
      pcStack_104 = (char *)(**(code **)(*piStack_e8 + 0x1c8))();
      puStack_10c = auStack_80;
      piStack_108 = (int *)&DAT_005e57fc;
      pcStack_110 = (char *)0x54b4a6;
      FUN_0058b100();
      pcStack_104 = (char *)0x80000000;
      piStack_108 = (int *)0x7f7f;
      puStack_10c = (undefined1 *)0xffff;
      pcStack_110 = (char *)_DAT_006668d0;
      uStack_114 = 0xe;
      (**(code **)(**(int **)(param_1 + 100) + 100))(0,0x46,0x32);
      pcStack_104 = (char *)0x80000000;
      piStack_108 = (int *)0x404;
      puStack_10c = (undefined1 *)0xff;
      pcStack_110 = (char *)0xff;
      uStack_114 = 0xff;
      pcStack_110 = (char *)FUN_00429950();
      uStack_114 = 0x54b4f8;
      FUN_00419dd0();
      pcStack_110 = DAT_0065abc4;
      uStack_114 = 0;
      FUN_004be2b0(0,0x46,0x32,0xe,auStack_80);
    }
    if ((*(int **)(param_1 + 0xa8) != piStack_e8) || (piStack_e8[0xe] != *(int *)(param_1 + 0xc0)))
    {
      pcStack_104 = (char *)0x80000000;
      piStack_108 = (int *)0x7f7f;
      puStack_10c = (undefined1 *)0xffff;
      pcStack_110 = (char *)_DAT_006668d0;
      uStack_114 = 0x40;
      (**(code **)(**(int **)(param_1 + 100) + 100))(0x40,0x40,0x40);
      pcVar13 = (char *)piStack_e8[0xe];
      uVar11 = 0xffffffff;
      iVar4 = 0;
      pcStack_f0 = (char *)0x0;
      pcVar6 = pcVar13;
      do {
        if (uVar11 == 0) break;
        uVar11 = uVar11 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      if (0 < (int)(~uVar11 - 1)) {
        do {
          cVar1 = pcVar13[iVar4];
          if (((('`' < cVar1) && (cVar1 < '{')) || (('@' < cVar1 && (cVar1 < '[')))) ||
             (('/' < cVar1 && (cVar1 < ':')))) {
            acStack_e4[(int)pcStack_f0] = cVar1;
            pcStack_f0 = (char *)((int)pcStack_f0 + 1);
          }
          uVar11 = 0xffffffff;
          iVar4 = iVar4 + 1;
          pcVar6 = pcVar13;
          do {
            if (uVar11 == 0) break;
            uVar11 = uVar11 - 1;
            cVar1 = *pcVar6;
            pcVar6 = pcVar6 + 1;
          } while (cVar1 != '\0');
        } while (iVar4 < (int)(~uVar11 - 1));
      }
      pcStack_104 = acStack_e4;
      acStack_e4[(int)pcStack_f0] = '\0';
      piStack_108 = (int *)0x54b5c2;
      pcVar6 = (char *)FUN_0049d800();
      pcStack_104 = acStack_e4;
      piStack_108 = (int *)0x54b5d3;
      pcVar7 = (char *)FUN_0049d800();
      uVar11 = 0xffffffff;
      do {
        if (uVar11 == 0) break;
        uVar11 = uVar11 - 1;
        cVar1 = *pcVar7;
        pcVar7 = pcVar7 + 1;
      } while (cVar1 != '\0');
      pcStack_104 = (char *)~uVar11;
      piStack_108 = (int *)0x54b5e4;
      pcStack_104 = (char *)FUN_00482ef0();
      uVar11 = 0xffffffff;
      do {
        pcVar7 = pcVar6;
        if (uVar11 == 0) break;
        uVar11 = uVar11 - 1;
        pcVar7 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar7;
      } while (cVar1 != '\0');
      uVar11 = ~uVar11;
      pcVar6 = pcVar7 + -uVar11;
      pcVar7 = pcStack_104;
      for (uVar12 = uVar11 >> 2; uVar12 != 0; uVar12 = uVar12 - 1) {
        *(undefined4 *)pcVar7 = *(undefined4 *)pcVar6;
        pcVar6 = pcVar6 + 4;
        pcVar7 = pcVar7 + 4;
      }
      for (uVar11 = uVar11 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
        *pcVar7 = *pcVar6;
        pcVar6 = pcVar6 + 1;
        pcVar7 = pcVar7 + 1;
      }
      pcStack_f0 = pcStack_104;
      if ((pcStack_104 != (char *)0x0) && (*pcStack_104 == '[')) {
        piStack_108 = (int *)0x54b61d;
        FUN_004830f0();
        uVar11 = 0xffffffff;
        pcVar6 = pcVar13;
        do {
          if (uVar11 == 0) break;
          uVar11 = uVar11 - 1;
          cVar1 = *pcVar6;
          pcVar6 = pcVar6 + 1;
        } while (cVar1 != '\0');
        piStack_108 = (int *)~uVar11;
        puStack_10c = (undefined1 *)0x54b632;
        pcStack_f0 = (char *)FUN_00482ef0();
        uVar11 = 0xffffffff;
        do {
          pcVar6 = pcVar13;
          if (uVar11 == 0) break;
          uVar11 = uVar11 - 1;
          pcVar6 = pcVar13 + 1;
          cVar1 = *pcVar13;
          pcVar13 = pcVar6;
        } while (cVar1 != '\0');
        uVar11 = ~uVar11;
        pcVar13 = pcVar6 + -uVar11;
        pcVar6 = pcStack_f0;
        for (uVar12 = uVar11 >> 2; uVar12 != 0; uVar12 = uVar12 - 1) {
          *(undefined4 *)pcVar6 = *(undefined4 *)pcVar13;
          pcVar13 = pcVar13 + 4;
          pcVar6 = pcVar6 + 4;
        }
        for (uVar11 = uVar11 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
          *pcVar6 = *pcVar13;
          pcVar13 = pcVar13 + 1;
          pcVar6 = pcVar6 + 1;
        }
      }
      puStack_10c = auStack_80;
      piStack_108 = piStack_e8;
      pcStack_110 = "h";
      pcStack_104 = pcStack_f0;
      FUN_0054ae10();
      pcStack_104 = (char *)0x80000000;
      piStack_108 = (int *)0x402;
      puStack_10c = (undefined1 *)0xff;
      pcStack_110 = (char *)0xff;
      uStack_114 = 0xff;
      pcStack_110 = (char *)FUN_00429950();
      uStack_114 = 0x54b69c;
      FUN_00419dd0();
      pcStack_110 = DAT_0065abc4;
      uStack_114 = 0;
      FUN_004be2b0(0x40,0x40,0x40,0x40,auStack_80);
      pcStack_104 = pcStack_f0;
      piStack_108 = (int *)0x54b6c6;
      FUN_004830f0();
      *(int *)(param_1 + 0xc0) = piStack_e8[0xe];
    }
    iVar4 = piStack_e8[0x10];
    iVar9 = *piStack_e8;
    *(int **)(param_1 + 0xa8) = piStack_e8;
    *(int *)(param_1 + 0xb8) = iVar4;
    pcStack_104 = (char *)0x54b6eb;
    uVar5 = (**(code **)(iVar9 + 0x1c0))();
    iVar4 = *piStack_e8;
    *(undefined4 *)(param_1 + 0xac) = uVar5;
    pcStack_104 = (char *)0x54b6fb;
    uVar5 = (**(code **)(iVar4 + 0x1d0))();
    *(undefined4 *)(param_1 + 0xb0) = uVar5;
    pcStack_104 = (char *)0x54b70b;
    uVar5 = (**(code **)(*piStack_e8 + 0x1c8))();
    *(undefined4 *)(param_1 + 0xb4) = uVar5;
  }
  puVar3 = PTR_DAT_005d79e0;
  if ((DAT_00667fcc != (int *)0x0) && (*(int *)(param_1 + 0xd4) != 0)) {
    iVar4 = (*(int *)(param_1 + 0xd4) * 0xff) / 6;
    if (DAT_006680c8 != 0) {
      if (iVar4 < 0x81) goto LAB_0054ba7b;
      iVar9 = *(int *)(param_1 + 0x70);
      pcStack_104 = (char *)0x100;
      piStack_108 = *(int **)(iVar9 + 8);
      puStack_10c = *(undefined1 **)(iVar9 + 4);
      pcStack_110 = (char *)0x0;
      uStack_114 = 0;
      FUN_00438d80(acStack_e4,4,4);
      pcStack_110 = acStack_e4;
      pcStack_104 = (char *)0x0;
      piStack_108 = (int *)0x0;
      uStack_114 = 0x54b79b;
      puStack_10c = (undefined1 *)iVar9;
      (**(code **)(*(int *)puVar3 + 0x5c))();
      uStack_114 = 1;
      uVar23 = 6;
      uVar22 = 0x77;
      uVar21 = 0x11;
      uVar20 = 0x7d;
      uVar19 = 0x31;
      uVar18 = 2;
      uVar17 = 1;
      uVar16 = 2;
      uVar15 = 0xf;
      uVar14 = 0x44;
      uVar5 = (**(code **)(*DAT_00667fcc + 0x1d8))(0x44,0xf,2,1,2,0x31,0x7d,0x11,0x77,6);
      uVar8 = (**(code **)(*DAT_00667fcc + 0x1c0))(uVar5);
      FUN_0054a5d0(uVar8,uVar5,uVar14,uVar15,uVar16,uVar17,uVar18,uVar19,uVar20,uVar21,uVar22,uVar23
                  );
      uStack_114 = 1;
      uVar23 = 4;
      uVar22 = 0x45;
      uVar21 = 0xc;
      uVar20 = 0x4d;
      uVar19 = 0x42;
      uVar18 = 2;
      uVar17 = 0x13;
      uVar16 = 2;
      uVar15 = 0x1f;
      uVar14 = 0x44;
      uVar5 = (**(code **)(*DAT_00667fcc + 0x1e8))(0x44,0x1f,2,0x13,2,0x42,0x4d,0xc,0x45,4);
      uVar8 = (**(code **)(*DAT_00667fcc + 0x1d0))(uVar5);
      FUN_0054a5d0(uVar8,uVar5,uVar14,uVar15,uVar16,uVar17,uVar18,uVar19,uVar20,uVar21,uVar22,uVar23
                  );
      uStack_114 = 1;
      uVar19 = 4;
      uVar18 = 0x2d;
      uVar17 = 0xc;
      uVar16 = 0x35;
      uVar15 = 0x51;
      uVar14 = 2;
      uVar8 = 0x22;
      uVar5 = (**(code **)(*DAT_00667fcc + 0x1e0))(0x44,0x2c,2,0x22,2,0x51,0x35,0xc,0x2d,4);
      uVar5 = (**(code **)(*DAT_00667fcc + 0x1c8))(uVar5);
      FUN_0054a5d0(uVar5,uVar8,uVar14,uVar15,uVar16,uVar17,uVar18,uVar19);
    }
    if (0x80 < iVar4) {
      uStack_b0 = 0xe;
      uStack_a0 = 0xe;
      puStack_10c = *(undefined1 **)(param_1 + 100);
      pcStack_104 = (char *)0x0;
      piStack_108 = (int *)0x0;
      uStack_b4 = 0x32;
      uStack_a4 = 0x32;
      pcStack_110 = acStack_e4;
      acStack_e4[0] = '\0';
      acStack_e4[1] = '\x01';
      acStack_e4[2] = '\0';
      acStack_e4[3] = '\0';
      acStack_e4[4] = '\0';
      acStack_e4[5] = '\0';
      acStack_e4[6] = '\0';
      acStack_e4[7] = '\0';
      iStack_dc = 0;
      uStack_d8 = 0;
      iStack_bc = 0x47;
      uStack_b8 = 7;
      uStack_ac = 0;
      uStack_a8 = 0;
      uStack_92 = 0;
      uStack_94 = 0;
      uStack_d0 = 0;
      uStack_d4 = 0;
      uStack_c0 = 0;
      uStack_c4 = 0;
      uStack_c8 = 0;
      iStack_cc = 0;
      uStack_98 = 0x1f;
      uStack_9c = 0;
      uStack_114 = 0x54b8e7;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))();
      uStack_c4 = 0x32;
      uStack_b4 = 0x32;
      uStack_114 = 0;
      uStack_c0 = 0xe;
      uStack_b8 = 0xe;
      uStack_b0 = 0xe;
      iStack_cc = 0x47;
      uStack_c8 = 0x17;
      iStack_bc = 0;
      uStack_a4 = 0;
      acStack_e4[4] = '\0';
      acStack_e4[5] = '\0';
      acStack_e4[6] = '\0';
      acStack_e4[7] = '\0';
      acStack_e4[0] = '\0';
      acStack_e4[1] = '\0';
      acStack_e4[2] = '\0';
      acStack_e4[3] = '\0';
      uStack_d0 = 0;
      uStack_d4 = 0;
      uStack_d8 = 0;
      iStack_dc = 0;
      uStack_a8 = 0x1f;
      uStack_ac = 0;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
                (&stack0xffffff0c,*(undefined4 *)(param_1 + 100),0);
      pcStack_104 = (char *)0x100;
      iStack_dc = 0x47;
      uStack_d8 = 0x24;
      uStack_d4 = 0x32;
      uStack_d0 = 0xe;
      iStack_cc = 0;
      uStack_c8 = 0x1c;
      uStack_c4 = 0x32;
      uStack_c0 = 0xe;
      uStack_b4 = 0;
      acStack_e4[4] = '\0';
      acStack_e4[5] = '\0';
      acStack_e4[6] = '\0';
      acStack_e4[7] = '\0';
      acStack_e4[0] = '\0';
      acStack_e4[1] = '\0';
      acStack_e4[2] = '\0';
      acStack_e4[3] = '\0';
      uStack_b8 = 0x1f;
      iStack_bc = 0;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
                (&pcStack_104,*(undefined4 *)(param_1 + 100),0,0);
      acStack_e4[0] = '@';
      acStack_e4[1] = '\0';
      acStack_e4[2] = '\0';
      acStack_e4[3] = '\0';
      acStack_e4[4] = '@';
      acStack_e4[5] = '\0';
      acStack_e4[6] = '\0';
      acStack_e4[7] = '\0';
      iStack_dc = 0x40;
      uStack_d4 = 0x40;
      uStack_d0 = 0x40;
      uStack_114 = 0x100;
      pcStack_110 = (char *)0x0;
      puStack_10c = (undefined1 *)0x0;
      piStack_108 = (int *)0x0;
      uStack_d8 = 0;
      uStack_c4 = 0;
      pcStack_104 = (char *)0x0;
      uStack_c8 = 0x1f;
      iStack_cc = 0;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))(&uStack_114,*(undefined4 *)(param_1 + 100),0,0)
      ;
    }
  }
LAB_0054ba7b:
  if (piStack_e8 == (int *)0x0) {
    piStack_108 = *(int **)(param_1 + 0xb8);
    pcStack_104 = (char *)0x0;
    puStack_10c = (undefined1 *)0x54ba91;
    piStack_e8 = (int *)FUN_00452690_TMapPane_GetInstance();
    if (piStack_e8 == (int *)0x0) goto LAB_0054be15;
  }
  puVar3 = PTR_DAT_005d79e0;
  if (*(int *)(param_1 + 0xdc) != 0) {
    iVar4 = (*(int *)(param_1 + 0xdc) * 0xff) / 6;
    if (DAT_006680c8 != 0) {
      if (iVar4 < 0x81) goto LAB_0054be15;
      iVar9 = *(int *)(param_1 + 0x74);
      pcStack_104 = (char *)0x100;
      piStack_108 = *(int **)(iVar9 + 8);
      puStack_10c = *(undefined1 **)(iVar9 + 4);
      pcStack_110 = (char *)0x0;
      uStack_114 = 0;
      FUN_00438d80(acStack_e4,*(int *)(param_1 + 0xc) + -0x88,4);
      pcStack_110 = acStack_e4;
      pcStack_104 = (char *)0x0;
      piStack_108 = (int *)0x0;
      uStack_114 = 0x54bb25;
      puStack_10c = (undefined1 *)iVar9;
      (**(code **)(*(int *)puVar3 + 0x5c))();
      uStack_114 = 0xffffffff;
      uVar22 = 6;
      uVar21 = 0x77;
      uVar20 = 0x11;
      uVar19 = 0x7d;
      uVar18 = 0x31;
      uVar17 = 2;
      uVar16 = 1;
      uVar15 = 2;
      iVar9 = *(int *)(param_1 + 0xc) + -0xc1;
      uVar14 = 0xf;
      uVar5 = (**(code **)(*piStack_e8 + 0x1d8))(iVar9,0xf,2,1,2,0x31,0x7d,0x11,0x77,6);
      uVar8 = (**(code **)(*piStack_e8 + 0x1c0))(uVar5);
      FUN_0054a5d0(uVar8,uVar5,iVar9,uVar14,uVar15,uVar16,uVar17,uVar18,uVar19,uVar20,uVar21,uVar22)
      ;
      uStack_114 = 0xffffffff;
      uVar22 = 4;
      uVar21 = 0x45;
      uVar20 = 0xc;
      uVar19 = 0x4d;
      uVar18 = 0x42;
      uVar17 = 2;
      uVar16 = 0x13;
      uVar15 = 2;
      iVar9 = *(int *)(param_1 + 0xc) + -0x91;
      uVar14 = 0x1f;
      uVar5 = (**(code **)(*piStack_e8 + 0x1e8))(iVar9,0x1f,2,0x13,2,0x42,0x4d,0xc,0x45,4);
      uVar8 = (**(code **)(*piStack_e8 + 0x1d0))(uVar5);
      FUN_0054a5d0(uVar8,uVar5,iVar9,uVar14,uVar15,uVar16,uVar17,uVar18,uVar19,uVar20,uVar21,uVar22)
      ;
      uStack_114 = 0xffffffff;
      uVar19 = 4;
      uVar18 = 0x2d;
      uVar17 = 0xc;
      uVar16 = 0x35;
      uVar15 = 0x51;
      uVar14 = 2;
      uVar8 = 0x22;
      uVar5 = (**(code **)(*piStack_e8 + 0x1e0))
                        (*(int *)(param_1 + 0xc) + -0x79,0x2c,2,0x22,2,0x51,0x35,0xc,0x2d,4);
      uVar5 = (**(code **)(*piStack_e8 + 0x1c8))(uVar5);
      FUN_0054a5d0(uVar5,uVar8,uVar14,uVar15,uVar16,uVar17,uVar18,uVar19);
    }
    if (0x80 < iVar4) {
      iStack_bc = *(int *)(param_1 + 0xc) + -0x80;
      pcStack_104 = (char *)0x0;
      uStack_b0 = 0xe;
      uStack_a0 = 0xe;
      puStack_10c = *(undefined1 **)(param_1 + 100);
      piStack_108 = (int *)0x0;
      uStack_b4 = 0x32;
      uStack_a4 = 0x32;
      pcStack_110 = acStack_e4;
      acStack_e4[0] = '\0';
      acStack_e4[1] = '\x01';
      acStack_e4[2] = '\0';
      acStack_e4[3] = '\0';
      acStack_e4[4] = '\0';
      acStack_e4[5] = '\0';
      acStack_e4[6] = '\0';
      acStack_e4[7] = '\0';
      iStack_dc = 0;
      uStack_d8 = 0;
      uStack_b8 = 7;
      uStack_ac = 0;
      uStack_a8 = 0x2a;
      uStack_92 = 0;
      uStack_94 = 0;
      uStack_d0 = 0;
      uStack_d4 = 0;
      uStack_c0 = 0;
      uStack_c4 = 0;
      uStack_c8 = 0;
      iStack_cc = 0;
      uStack_98 = 0x1f;
      uStack_9c = 0;
      uStack_114 = 0x54bc75;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))();
      iStack_cc = *(int *)(param_1 + 0xc) + -0x80;
      uStack_114 = 0;
      uStack_c4 = 0x32;
      uStack_b4 = 0x32;
      uStack_c0 = 0xe;
      uStack_b0 = 0xe;
      uStack_c8 = 0x17;
      iStack_bc = 0;
      uStack_b8 = 0x38;
      uStack_a4 = 0;
      acStack_e4[4] = '\0';
      acStack_e4[5] = '\0';
      acStack_e4[6] = '\0';
      acStack_e4[7] = '\0';
      acStack_e4[0] = '\0';
      acStack_e4[1] = '\0';
      acStack_e4[2] = '\0';
      acStack_e4[3] = '\0';
      uStack_d0 = 0;
      uStack_d4 = 0;
      uStack_d8 = 0;
      iStack_dc = 0;
      uStack_a8 = 0x1f;
      uStack_ac = 0;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
                (&stack0xffffff0c,*(undefined4 *)(param_1 + 100),0);
      iStack_dc = *(int *)(param_1 + 0xc) + -0x80;
      pcStack_104 = (char *)0x100;
      uStack_d8 = 0x24;
      uStack_d4 = 0x32;
      uStack_d0 = 0xe;
      uStack_c0 = 0xe;
      uStack_c4 = 0x32;
      iStack_cc = 0;
      uStack_c8 = 0x46;
      uStack_b4 = 0;
      acStack_e4[4] = '\0';
      acStack_e4[5] = '\0';
      acStack_e4[6] = '\0';
      acStack_e4[7] = '\0';
      acStack_e4[0] = '\0';
      acStack_e4[1] = '\0';
      acStack_e4[2] = '\0';
      acStack_e4[3] = '\0';
      uStack_b8 = 0x1f;
      iStack_bc = 0;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
                (&pcStack_104,*(undefined4 *)(param_1 + 100),0,0);
      acStack_e4[0] = '@';
      acStack_e4[1] = '\0';
      acStack_e4[2] = '\0';
      acStack_e4[3] = '\0';
      acStack_e4[4] = '@';
      acStack_e4[5] = '\0';
      acStack_e4[6] = '\0';
      acStack_e4[7] = '\0';
      iStack_dc = 0x40;
      uStack_d8 = 0x40;
      uStack_d4 = 0x40;
      uStack_d0 = 0x40;
      uStack_114 = 0x100;
      pcStack_110 = (char *)0x0;
      puStack_10c = (undefined1 *)0x0;
      piStack_108 = (int *)0x0;
      uStack_c4 = 0;
      pcStack_104 = (char *)0x0;
      uStack_c8 = 0x1f;
      iStack_cc = 0;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))(&uStack_114,*(undefined4 *)(param_1 + 100),0,0)
      ;
    }
  }
LAB_0054be15:
  if ((DAT_005d7a18 == 0) && (DAT_00667fcc != (int *)0x0)) {
    pcStack_104 = (char *)0x1;
    if (piStack_e8 == (int *)0x0) {
      piStack_108 = *(int **)(param_1 + 0x10);
      pcStack_110 = *(char **)(param_1 + 8);
      uStack_114 = *(undefined4 *)(param_1 + 4);
      puStack_10c = (undefined1 *)(*(int *)(param_1 + 0xc) >> 1);
    }
    else {
      piStack_108 = *(int **)(param_1 + 0x10);
      puStack_10c = *(undefined1 **)(param_1 + 0xc);
      pcStack_110 = *(char **)(param_1 + 8);
      uStack_114 = *(undefined4 *)(param_1 + 4);
    }
    FUN_004aacb0();
  }
  return;
}


