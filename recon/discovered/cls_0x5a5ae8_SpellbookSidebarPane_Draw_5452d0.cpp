// FUN_005452d0 @ 005452d0 size=3133

void __thiscall FUN_005452d0(int *param_1,int *param_2,undefined4 param_3)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 extraout_ECX_04;
  char *pcVar9;
  int iVar10;
  int iVar11;
  char *pcVar12;
  size_t _Count;
  char *pcVar13;
  int *piVar14;
  undefined4 uVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  uint uStack_2e0;
  undefined4 uStack_2dc;
  int iStack_2d8;
  undefined4 uStack_2d4;
  int iStack_2d0;
  uint uVar18;
  uint uStack_2b4;
  undefined4 uStack_2b0;
  int iStack_2ac;
  int iStack_2a8;
  undefined4 uStack_2a4;
  undefined4 uStack_2a0;
  undefined4 uStack_29c;
  undefined4 uStack_298;
  undefined4 uStack_294;
  undefined4 uStack_290;
  undefined4 uStack_28c;
  undefined4 uStack_288;
  undefined4 uStack_284;
  undefined4 uStack_280;
  undefined4 uStack_27c;
  undefined4 uStack_278;
  undefined4 uStack_274;
  undefined4 uStack_270;
  undefined4 uStack_26c;
  undefined4 uStack_268;
  undefined4 uStack_264;
  int iStack_250;
  int iStack_24c;
  int iStack_248;
  int iStack_244;
  int iStack_240;
  int iStack_23c;
  int iStack_238;
  char acStack_22a [10];
  undefined1 uStack_220;
  undefined1 uStack_21f;
  undefined1 uStack_21e;
  char acStack_1d8 [8];
  undefined1 uStack_1d0;
  undefined1 uStack_1cf;
  undefined1 uStack_1ce;
  undefined1 auStack_188 [80];
  char acStack_138 [100];
  char acStack_d4 [100];
  undefined1 auStack_70 [28];
  uint uStack_54;
  uint uStack_28;
  int *piStack_1c;
  uint uStack_18;
  
  iStack_2d0 = 0;
  iStack_2d8 = param_1[100];
  param_1[0x66] = 1;
  param_1[0x67] = 1;
  uStack_2d4 = param_3;
  uStack_2dc = (char *)0x0;
  uStack_2e0 = 0;
  FUN_004bd680();
  iStack_2d0 = -0x80000000;
  uStack_2d4 = 0x7f7f;
  iStack_2d8 = 0xffff;
  uStack_2dc = (char *)((int *)param_1[0x60])[0xc];
  uStack_2e0 = 0xe5;
  (**(code **)(*(int *)param_1[0x60] + 100))();
  iVar11 = param_1[0x5f];
  if (iVar11 < 0) {
    iVar11 = -0x50 - iVar11 % 0x50;
  }
  else {
    iVar11 = -(iVar11 % 0x50);
  }
  for (; iVar11 < 0x140; iVar11 = iVar11 + 0x50) {
    iVar2 = param_1[99];
    piVar14 = (int *)param_1[0x60];
    FUN_00438d80(&uStack_2a4,0,iVar11,0,0,*(undefined4 *)(iVar2 + 4),*(undefined4 *)(iVar2 + 8));
    (**(code **)(*piVar14 + 0x5c))(&uStack_2a4,iVar2,0);
  }
  (**(code **)(*param_1 + 0x90))();
  if ((DAT_00667fcc != (int *)0x0) && (iVar11 = DAT_00667fcc[0xbb], iVar11 != 0)) {
    uVar18 = 0;
    uStack_2dc = (char *)0x0;
    iStack_2a8 = 0;
    uStack_2b0 = 0;
    if (((int *)DAT_00667fcc[0xbf] != (int *)0x0) &&
       ((DAT_00667fcc[0xbb] != 0 && (*(int *)DAT_00667fcc[0xbf] != 0)))) {
      uVar8 = 0xffffffff;
      pcVar9 = *(char **)DAT_00667fcc[0xbf];
      do {
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      _strncpy((char *)&iStack_2d8,*(char **)DAT_00667fcc[0xbf],~uVar8 - 1);
      *(undefined1 *)((int)&uStack_2dc + ~uVar8 + 3) = 0;
    }
    uVar8 = 0xffffffff;
    piVar14 = &iStack_2d8;
    do {
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      iVar2 = *piVar14;
      piVar14 = (int *)((int)piVar14 + 1);
    } while ((char)iVar2 != '\0');
    iStack_2ac = ~uVar8 - 1;
    if ((&stack0x00000000 != (undefined1 *)0x2d8) && (0 < (int)(~uVar8 - 1))) {
      iStack_2a8 = FUN_0053ed70();
      uStack_2b0 = FUN_0053ef90();
    }
    pcVar9 = *(char **)param_1[0x6a];
    pcVar13 = pcVar9;
    if (0 < iVar11) {
      do {
        pcVar12 = pcVar13 + -0x5b;
        iVar2 = param_1[0x5f];
        if ((iVar2 < (int)pcVar9) && ((int)uStack_2dc < iVar2 + 0xe5)) {
          iVar10 = iVar2 - (int)uStack_2dc;
          if ((iVar10 < (int)(pcVar13 + -0x2f)) && (0x2c < iVar10 + 0xe5)) {
            FUN_0058b100(auStack_70,s_SPELL_s_005e52e4);
            uVar16 = 0x401;
            uVar3 = FUN_00429950(0xf2,0xe7,0xff);
            uVar17 = extraout_ECX;
            FUN_00419dd0(uVar3);
            uVar15 = 0;
            uVar3 = DAT_0065abc4;
            uVar4 = FUN_0049d800(auStack_70);
            FUN_004be2b0(10,0x2c - iVar10,0x84,pcVar12,uVar4,uVar15,uVar3,uVar17,uVar16);
          }
          if (iVar10 < 0x32) {
            FUN_0046e7f0(acStack_138,100);
            iVar5 = FUN_0058ade0(acStack_138,0x20);
            if (iVar5 == 0) {
              FUN_0046e7f0(acStack_d4,100);
              uVar8 = 0xffffffff;
              pcVar13 = acStack_d4;
              do {
                if (uVar8 == 0) break;
                uVar8 = uVar8 - 1;
                cVar1 = *pcVar13;
                pcVar13 = pcVar13 + 1;
              } while (cVar1 != '\0');
              _strncpy(acStack_22a + 2,acStack_d4,~uVar8);
              if (10 < (int)(~uVar8 - 1)) {
                uStack_21e = 0;
                uStack_21f = 0x2e;
                uStack_220 = 0x2e;
              }
              uVar17 = 0x402;
              uVar3 = extraout_ECX_02;
              FUN_00444e20(0xffffffff);
              pcVar13 = acStack_22a + 2;
              iVar5 = 10 - iVar10;
            }
            else {
              uVar8 = 0xffffffff;
              pcVar13 = (char *)(iVar5 + 1);
              do {
                if (uVar8 == 0) break;
                uVar8 = uVar8 - 1;
                cVar1 = *pcVar13;
                pcVar13 = pcVar13 + 1;
              } while (cVar1 != '\0');
              uStack_2dc = (char *)(~uVar8 - 1);
              _strncpy(acStack_1d8,(char *)(iVar5 + 1),~uVar8);
              uVar8 = 0xffffffff;
              pcVar13 = acStack_138;
              do {
                if (uVar8 == 0) break;
                uVar8 = uVar8 - 1;
                cVar1 = *pcVar13;
                pcVar13 = pcVar13 + 1;
              } while (cVar1 != '\0');
              iVar5 = ~uVar8 - (int)uStack_2dc;
              _Count = iVar5 - 2;
              _strncpy(acStack_22a + 2,acStack_138,_Count);
              acStack_22a[iVar5] = '\0';
              if (10 < (int)_Count) {
                uStack_21e = 0;
                uStack_21f = 0x2e;
                uStack_220 = 0x2e;
              }
              if (10 < (int)uStack_2dc) {
                uStack_1ce = 0;
                uStack_1cf = 0x2e;
                uStack_1d0 = 0x2e;
              }
              uVar17 = 0x402;
              uVar3 = extraout_ECX_00;
              FUN_00444e20(0xffffffff);
              FUN_004be2b0(0x36,-iVar10 + 4,0x55,0x56,acStack_22a + 2,0,DAT_00667c2c,uVar3,uVar17);
              uVar17 = 0x402;
              uVar3 = extraout_ECX_01;
              FUN_00444e20(0xffffffff);
              pcVar13 = acStack_1d8;
              iVar5 = -iVar10 + 0x10;
            }
            FUN_004be2b0(0x36,iVar5,0x55,0x56,pcVar13,0,DAT_00667c2c,uVar3,uVar17);
            if ((iStack_2a8 != 0) &&
               ((iVar5 = FUN_0046d6b0(), iVar5 != 0 || (iVar5 = FUN_0046d710(), iVar5 != 0)))) {
              iVar6 = FUN_0051b7c0();
              if ((iVar6 == 0) || (param_1[0x72] != uVar18)) {
                uStack_2dc = (char *)0x0;
                iStack_2d0 = 0;
              }
              else {
                uStack_2dc = (char *)0xfffffffe;
                iStack_2d0 = -2;
              }
              iVar6 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
              iStack_2d0 = iStack_2d0 + 0xc;
              *(uint *)(iVar5 + 0x18) = (-(uint)(iVar6 != 0x10) & 0xffff8400) + 0xf81f;
              uStack_2b4 = (int)uStack_2dc - iVar10;
              FUN_004bd680(iStack_2d0,uStack_2b4,iVar5,0x100);
              uStack_2dc = *(char **)(DAT_00667fcc[0xbf] + uVar18 * 4);
              if (uStack_2dc != (char *)0x0) {
                iVar5 = -1;
                pcVar13 = uStack_2dc;
                do {
                  if (iVar5 == 0) break;
                  iVar5 = iVar5 + -1;
                  cVar1 = *pcVar13;
                  pcVar13 = pcVar13 + 1;
                } while (cVar1 != '\0');
                if (((iVar5 != -2) && (iVar5 = FUN_0053ef90(), iVar5 != 0)) &&
                   (iVar5 = FUN_0051b7c0(), iVar5 != 0)) {
                  (**(code **)(*DAT_00667fcc + 0x1d0))();
                }
              }
              uVar3 = FUN_0046d710();
              FUN_004bd680(iStack_2d0,uStack_2b4,uVar3,0x2000);
            }
          }
          if (((int)(pcVar9 + (-0xe5 - iVar2)) < 0x1e) && (iStack_2d0 = 0, 0 < iStack_2ac)) {
            uStack_2dc = (char *)0xc;
            do {
              iVar5 = 0;
              if (0 < DAT_0066deec) {
                do {
                  FUN_00474210();
                  iVar6 = FUN_0058b266();
                  iVar7 = FUN_0058b266((int)*(char *)((int)&iStack_2d8 + iStack_2d0));
                  if (iVar7 == iVar6) {
                    FUN_0058b100(&uStack_2a4,s__scandy_005e52fc);
                    uVar8 = uStack_18 | 0x2000;
                    uVar3 = FUN_0046d710((&PTR_DAT_005e4fb4)[iVar5]);
                    FUN_004bd680(uStack_2dc,pcVar12 + (0x3d - iVar10),uVar3,uVar8);
                    break;
                  }
                  iVar5 = iVar5 + 1;
                } while (iVar5 < DAT_0066deec);
              }
              iStack_2d0 = iStack_2d0 + 1;
              uStack_2dc = uStack_2dc + 0x10;
            } while (iStack_2d0 < iStack_2ac);
          }
          param_2 = piStack_1c;
          if ((int)(pcVar9 + (-0xe5 - iVar2)) < 0x2f) {
            uStack_2b4 = FUN_0053ef90();
            param_2 = piStack_1c;
            if (uStack_2b4 != 0) {
              uVar3 = FUN_0049d800(s_SPANESKILLS_005e5304);
              FUN_0058b100(auStack_188,s__s___d_005e5310,uVar3);
              uVar4 = 0x401;
              uVar17 = FUN_00429950(5,0xf4,0xf4);
              uVar3 = extraout_ECX_03;
              FUN_00419dd0(uVar17);
              FUN_004be2b0(10,pcVar12 + (0x2c - iVar10),0x84,pcVar12,auStack_188,0,DAT_0065abc4,
                           uVar3,uVar4);
              uVar3 = FUN_0049d800(s_SPANEMANA_005e5318);
              FUN_0058b100(auStack_188,s__s___d_005e5324,uVar3);
              uVar4 = 0x401;
              uVar17 = FUN_00429950(0xe9,0xff,0x25);
              uVar3 = extraout_ECX_04;
              FUN_00419dd0(uVar17);
              FUN_004be2b0(0x5a,pcVar12 + (0x2c - iVar10),0x84,pcVar12,auStack_188,0,DAT_0065abc4,
                           uVar3,uVar4);
              param_2 = piStack_1c;
            }
          }
        }
        uVar18 = uVar18 + 1;
        uStack_2dc = pcVar9;
        if (((DAT_00667fcc[0xbf] != 0) && (uVar18 < (uint)DAT_00667fcc[0xbb])) &&
           (*(int *)(DAT_00667fcc[0xbf] + uVar18 * 4) != 0)) {
          uVar8 = 0xffffffff;
          pcVar13 = *(char **)(DAT_00667fcc[0xbf] + uVar18 * 4);
          do {
            if (uVar8 == 0) break;
            uVar8 = uVar8 - 1;
            cVar1 = *pcVar13;
            pcVar13 = pcVar13 + 1;
          } while (cVar1 != '\0');
          _strncpy((char *)&iStack_2d8,*(char **)(DAT_00667fcc[0xbf] + uVar18 * 4),~uVar8 - 1);
          *(undefined1 *)((int)&uStack_2dc + ~uVar8 + 3) = 0;
        }
        uVar8 = 0xffffffff;
        piVar14 = &iStack_2d8;
        do {
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          iVar2 = *piVar14;
          piVar14 = (int *)((int)piVar14 + 1);
        } while ((char)iVar2 != '\0');
        iStack_2ac = ~uVar8 - 1;
        if ((&stack0x00000000 != (undefined1 *)0x2d8) && (0 < (int)(~uVar8 - 1))) {
          iStack_2a8 = FUN_0053ed70();
          uStack_2b0 = FUN_0053ef90();
        }
        pcVar13 = *(char **)(param_1[0x6a] + uVar18 * 4);
        pcVar9 = pcVar9 + (int)pcVar13;
      } while ((int)uVar18 < iVar11);
    }
  }
  piVar14 = (int *)param_1[0x60];
  iStack_250 = piVar14[7];
  iStack_24c = piVar14[5];
  iStack_248 = piVar14[6];
  iStack_240 = piVar14[9];
  iStack_238 = piVar14[0xb];
  iStack_244 = piVar14[8];
  iStack_23c = piVar14[10];
  (**(code **)(*piVar14 + 0x44))(piVar14[5],piVar14[6],0x94);
  iVar11 = param_1[0x60];
  uStack_284 = *(undefined4 *)(iVar11 + 4);
  uStack_280 = *(undefined4 *)(iVar11 + 8);
  uStack_2b4 = uStack_28 | 0x10000104;
  uStack_2b0 = 0;
  iStack_2ac = 0;
  iStack_2a8 = 0;
  uStack_28c = 0x12;
  uStack_288 = 0x2f;
  uStack_27c = 0;
  uStack_278 = 0;
  uStack_264 = 0;
  uStack_2a0 = 0;
  uStack_2a4 = 0;
  uStack_290 = 0;
  uStack_294 = 0;
  uStack_298 = 0;
  uStack_29c = 0;
  uStack_268 = 0x1f;
  uStack_26c = 0;
  uStack_274 = uStack_284;
  uStack_270 = uStack_280;
  (**(code **)(*param_2 + 0x5c))(&uStack_2b4,iVar11,0,0);
  piVar14 = (int *)param_1[0x60];
  (**(code **)(*piVar14 + 0x48))(uStack_270);
  (**(code **)(*piVar14 + 0x40))(uStack_270,uStack_26c);
  (**(code **)(*piVar14 + 0x44))(uStack_270,uStack_26c,uStack_268,uStack_264);
  iVar11 = param_1[0x61];
  uStack_2b0 = *(undefined4 *)(iVar11 + 4);
  iStack_2ac = *(undefined4 *)(iVar11 + 8);
  uStack_2e0 = uStack_54 | 0x10000100;
  uStack_2dc = (char *)0x0;
  iStack_2d8 = 0;
  uStack_2d4 = 0;
  uStack_2b4 = 0x2d;
  iStack_2a8 = 0;
  uStack_2a4 = 0;
  uStack_290 = 0;
  iStack_2d0 = 0;
  uStack_294 = 0x1f;
  uStack_298 = 0;
  uStack_2a0 = uStack_2b0;
  uStack_29c = iStack_2ac;
  (**(code **)(*param_2 + 0x5c))(&uStack_2e0,iVar11,0,0);
  iVar11 = param_1[0x62];
  uStack_2b0 = *(undefined4 *)(iVar11 + 4);
  iStack_2ac = *(undefined4 *)(iVar11 + 8);
  uStack_2b4 = 0;
  uStack_2a0 = 0;
  uStack_2dc = (char *)0x0;
  uStack_2e0 = 0;
  iStack_2d0 = 0;
  uStack_2d4 = 0;
  iStack_2d8 = 0;
  uStack_2a4 = 0x1f;
  iStack_2a8 = 0;
  (**(code **)(*param_2 + 0x5c))(&stack0xfffffd10,iVar11,0,0);
  if (param_2 != (int *)PTR_DAT_005d79e0) {
    FUN_00435cb0(param_2);
    return;
  }
  FUN_004aacb0(param_1[1],param_1[2],param_1[3],param_1[4],6);
  return;
}


