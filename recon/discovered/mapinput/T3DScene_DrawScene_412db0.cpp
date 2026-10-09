// FUN_00412db0 @ 00412db0 size=5250

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00412db0(int *param_1)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 *puVar5;
  int *piVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  undefined *puVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  short *psVar14;
  int iVar15;
  uint uVar16;
  int iVar17;
  float *pfVar18;
  uint uVar19;
  bool bVar20;
  short local_1d8 [4];
  undefined2 local_1d0;
  undefined2 local_1ce;
  undefined2 local_1cc;
  undefined2 local_1ca;
  undefined2 local_1c8;
  undefined4 local_19c;
  undefined4 local_198;
  undefined4 local_194;
  undefined4 local_190;
  undefined4 local_18c;
  undefined4 local_188;
  undefined4 local_184;
  undefined4 local_180;
  int *local_17c;
  float local_160;
  undefined4 local_12c;
  float local_120 [4];
  undefined4 local_110;
  float local_10c;
  float local_108;
  float local_104;
  undefined4 local_100;
  undefined4 local_d4;
  undefined4 local_d0;
  float local_c4;
  undefined4 local_a8;
  undefined4 local_a4;
  undefined4 local_a0;
  undefined4 local_9c;
  undefined4 local_90;
  undefined4 local_8c;
  undefined4 local_88;
  undefined4 local_84;
  undefined4 local_80;
  uint local_74;
  int local_6c;
  uint local_68;
  uint local_60;
  undefined4 local_5c;
  int local_58;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
  undefined1 local_40 [8];
  int local_38;
  uint local_34;
  undefined2 local_30;
  undefined2 local_2e;
  undefined2 local_2c;
  undefined2 local_2a;
  undefined2 local_28;
  undefined2 local_26;
  undefined2 local_24;
  undefined2 local_22;
  undefined2 local_20;
  undefined1 *local_1c;
  void *local_14;
  undefined1 *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &DAT_005a3818;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  local_1c = &stack0xfffffc28;
  local_60 = (uint)(DAT_005e8850 == 0);
  if (DAT_0066a8f8 == (int *)0x0) {
    return 0;
  }
  if (DAT_0066a2f0 == (int *)0x0) {
    return 0;
  }
  if (*param_1 == 0) {
    return 0;
  }
  if (DAT_005d7a10 == 0) {
    return 1;
  }
  ExceptionList = &local_14;
  iVar11 = (**(code **)(*DAT_0066a8f8 + 0x60))(DAT_0066a8f8);
  if ((iVar11 != 0) && (iVar11 = (**(code **)(*DAT_0066a8f8 + 0x6c))(DAT_0066a8f8), iVar11 != 0)) {
    ExceptionList = local_14;
    return 0;
  }
  iVar11 = (**(code **)(*DAT_0066a2f0 + 0x60))(DAT_0066a2f0);
  if ((iVar11 != 0) && (iVar11 = (**(code **)(*DAT_0066a2f0 + 0x6c))(DAT_0066a2f0), iVar11 != 0)) {
    ExceptionList = local_14;
    return 0;
  }
  DAT_005e91d4 = (int *)0x0;
  if (DAT_00668154 == 0) {
    DAT_005e91c8 = 1;
    DAT_005e91cc = DAT_00668510;
    DAT_005e91d0 = DAT_00668514;
  }
  if (DAT_006680e4 != 0) {
    DAT_005e91c8 = 0;
  }
  if (DAT_005e91c8 == 0) {
    DAT_005e91d0 = 0;
    DAT_005e91cc = 0;
  }
  else {
    local_54 = DAT_005e91cc + -2;
    local_50 = DAT_005e91d0 + -2;
    local_4c = DAT_005e91cc + 2;
    local_48 = DAT_005e91d0 + 2;
    iVar12 = (**(code **)(**(int **)(PTR_DAT_005d79e0 + 0x8c) + 0x2c))();
    iVar17 = DAT_005e91d0;
    iVar11 = DAT_005e91cc;
    if (iVar12 != 0) {
      piVar2 = *(int **)(PTR_DAT_005d79e0 + 0x8c);
      iVar15 = piVar2[4] * DAT_005e91d0 + DAT_005e91cc;
      if ((((DAT_005e91d0 < 0x1e0) && (-1 < DAT_005e91d0)) && (DAT_005e91cc < 0x280)) &&
         (-1 < DAT_005e91cc)) {
        local_30 = *(undefined2 *)(iVar12 + iVar15 * 2);
        *(undefined2 *)(iVar12 + iVar15 * 2) = 0xf81f;
      }
      iVar17 = iVar17 + 1;
      if (iVar17 < 0x1e0) {
        if (((-1 < iVar17) && (iVar11 + -1 < 0x280)) && (-1 < iVar11 + -1)) {
          local_2c = *(undefined2 *)(iVar12 + -2 + (piVar2[4] + iVar15) * 2);
          *(undefined2 *)(iVar12 + -2 + (piVar2[4] + iVar15) * 2) = 0xf81f;
        }
        if (((iVar17 < 0x1e0) && (-1 < iVar17)) && ((iVar11 + 1 < 0x280 && (-1 < iVar11 + 1)))) {
          local_28 = *(undefined2 *)(iVar12 + 2 + (piVar2[4] + iVar15) * 2);
          *(undefined2 *)(iVar12 + 2 + (piVar2[4] + iVar15) * 2) = 0xf81f;
        }
      }
      iVar17 = DAT_005e91d0 + -1;
      if (iVar17 < 0x1e0) {
        if (((-1 < iVar17) && (iVar11 + -1 < 0x280)) && (-1 < iVar11 + -1)) {
          local_2e = *(undefined2 *)(iVar12 + -2 + (iVar15 - piVar2[4]) * 2);
          *(undefined2 *)(iVar12 + -2 + (iVar15 - piVar2[4]) * 2) = 0xf81f;
          iVar11 = DAT_005e91cc;
        }
        if (((iVar17 < 0x1e0) && (-1 < iVar17)) && ((iVar11 + 1 < 0x280 && (-1 < iVar11 + 1)))) {
          local_2a = *(undefined2 *)(iVar12 + 2 + (iVar15 - piVar2[4]) * 2);
          *(undefined2 *)(iVar12 + 2 + (iVar15 - piVar2[4]) * 2) = 0xf81f;
        }
      }
      if (((DAT_005e91d0 + -2 < 0x1e0) && (-1 < DAT_005e91d0 + -2)) &&
         ((iVar11 < 0x280 && (-1 < iVar11)))) {
        local_20 = *(undefined2 *)(iVar12 + (iVar15 + piVar2[4] * -2) * 2);
        *(undefined2 *)(iVar12 + (iVar15 + piVar2[4] * -2) * 2) = 0xf81f;
      }
      iVar17 = DAT_005e91d0;
      if ((((DAT_005e91d0 + 2 < 0x1e0) && (-1 < DAT_005e91d0 + 2)) && (iVar11 < 0x280)) &&
         (-1 < iVar11)) {
        local_24 = *(undefined2 *)(iVar12 + (iVar15 + piVar2[4] * 2) * 2);
        *(undefined2 *)(iVar12 + (iVar15 + piVar2[4] * 2) * 2) = 0xf81f;
      }
      if (iVar17 < 0x1e0) {
        if (((-1 < iVar17) && (iVar11 + -2 < 0x280)) && (-1 < iVar11 + -2)) {
          local_26 = *(undefined2 *)(iVar12 + -4 + iVar15 * 2);
          *(undefined2 *)(iVar12 + -4 + iVar15 * 2) = 0xf81f;
        }
        if (((iVar17 < 0x1e0) && (-1 < iVar17)) && ((iVar11 + 2 < 0x280 && (-1 < iVar11 + 2)))) {
          local_22 = *(undefined2 *)(iVar12 + 4 + iVar15 * 2);
          *(undefined2 *)(iVar12 + 4 + iVar15 * 2) = 0xf81f;
        }
      }
      (**(code **)(*piVar2 + 0x30))();
    }
  }
  FUN_00412150(DAT_006663d8,DAT_006663d4,DAT_00667c30,DAT_0065c5c4);
  local_5c = 1;
  (**(code **)(*DAT_006699d0 + 0x24))(DAT_006699d0,local_40,&local_5c);
  if (DAT_005e91c4 != 0) {
    if ((DAT_006680f4 == 0) || (DAT_006671f0 != 0)) {
      local_a8 = DAT_005e887c;
      local_a4 = DAT_005e8878;
      local_a0 = DAT_005e892c;
      local_9c = DAT_005e8884;
      (**(code **)(*DAT_006699d0 + 0x30))(DAT_006699d0,1,&local_a8,3);
    }
    else {
      uVar3 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x1c);
      uVar4 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x14);
      local_90 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x18);
      local_8c = *(undefined4 *)(PTR_DAT_005d79e0 + 0x20);
      local_88 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x24);
      local_84 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x28);
      local_80 = *(undefined4 *)(PTR_DAT_005d79e0 + 0x2c);
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x44))
                (DAT_005e887c,DAT_005e8878,DAT_005e892c,DAT_005e8884);
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x40))(0,0);
      puVar10 = PTR_DAT_005d79e0;
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x48))(uVar3);
      (**(code **)(*(int *)puVar10 + 0x40))(uVar4,local_90);
      (**(code **)(*(int *)puVar10 + 0x44))(local_8c,local_88,local_84,local_80);
    }
  }
  if (DAT_005e8850 == 0) {
    DAT_005e8850 = 1;
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 0x24))(DAT_00668f14);
    }
    else {
      FUN_0056d260();
    }
  }
  iVar11 = FUN_00417060(9,2 - (uint)(DAT_005e91b0 != 0));
  if (iVar11 != 0) {
    FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c630c,0x3ca);
  }
  iVar11 = FUN_00417060(0x1a,DAT_005c61a0);
  if (iVar11 != 0) {
    FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c6324,0x3cb);
  }
  iVar11 = FUN_00417060(0x1b,DAT_005c61a4);
  if (iVar11 != 0) {
    FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c633c,0x3cc);
  }
  iVar11 = 1;
  if (DAT_006671f0 != 0) goto LAB_0041338d;
  if (DAT_005d7a28 == 0) {
    if ((DAT_005e91c0 != 0) && (DAT_006697c0 != 0)) {
      iVar11 = 2;
      goto LAB_0041338d;
    }
    if ((DAT_005e91c0 != 0) && (iVar11 = 4, DAT_006697bc != 0)) goto LAB_0041338d;
  }
  iVar11 = 8;
LAB_0041338d:
  local_38 = (DAT_005c61c0 * DAT_005e87dc * iVar11) / 100;
  iVar11 = local_38 * (DAT_005e8848 >> 0x10 & 0xff);
  local_34 = (int)(iVar11 + (iVar11 >> 0x1f & 0xffU)) >> 8;
  if (0xfe < (int)local_34) {
    local_34 = 0xff;
  }
  iVar11 = local_38 * (DAT_005e8848 >> 8 & 0xff);
  local_68 = (int)(iVar11 + (iVar11 >> 0x1f & 0xffU)) >> 8;
  if (0xfe < (int)local_68) {
    local_68 = 0xff;
  }
  iVar11 = local_38 * (DAT_005e8848 & 0xff);
  uVar13 = (int)(iVar11 + (iVar11 >> 0x1f & 0xffU)) >> 8;
  if (0xfe < (int)uVar13) {
    uVar13 = 0xff;
  }
  local_74 = uVar13;
  if ((DAT_005d7a28 == 0) && (DAT_006671f0 != 0)) {
    local_12c = 0x43fa0000;
    local_c4 = (float)local_38 * _DAT_005a36dc + _DAT_005a3814;
    local_d0 = 0x3e4ccccd;
    iVar11 = FUN_00417060(0x22,((local_34 | 0xffffff00) << 8 | local_68) << 8 | uVar13);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c6354,0x3ec);
    }
    iVar11 = FUN_00417060(0x23,3);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c636c,0x3ed);
    }
    iVar11 = FUN_00417060(0x24,local_12c);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c6384,0x3ee);
    }
    iVar11 = FUN_00417060(0x25,local_c4);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c639c,0x3ef);
    }
    iVar11 = FUN_00417060(0x26,local_d0);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c63b4,0x3f0);
    }
    iVar11 = FUN_00417060(0x1c,1);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c63cc,0x3f1);
    }
    pfVar18 = local_120;
    for (iVar11 = 0x14; iVar11 != 0; iVar11 = iVar11 + -1) {
      *pfVar18 = 0.0;
      pfVar18 = pfVar18 + 1;
    }
    local_120[0] = 1.12104e-43;
    local_120[1] = (float)(int)local_34 * _DAT_005a3534;
    local_120[2] = (float)(int)local_68 * _DAT_005a3534;
    local_120[3] = (float)(int)local_74 * _DAT_005a3534;
    local_110 = local_100;
    local_d4 = 1;
    local_10c = local_120[1];
    local_108 = local_120[2];
    local_104 = local_120[3];
    iVar11 = (**(code **)(*DAT_005e8918 + 0xc))(DAT_005e8918,local_120);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c63e4,0x3fc);
    }
    iVar11 = (**(code **)(*DAT_005e8918 + 0x14))(DAT_005e8918,DAT_00668f14,&DAT_005e8800);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c63fc,0x3fd);
    }
    iVar11 = (**(code **)(*DAT_006699d0 + 0x20))(DAT_006699d0,DAT_005e8800);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c6414,0x3fe);
    }
  }
  if (DAT_005c61b8 == 0) {
    if ((-1 < DAT_005e8980) &&
       (puVar5 = *(undefined4 **)(DAT_005e8870 + DAT_005e8980 * 4), puVar5[0x15] != 0)) {
      if (DAT_005d7a28 == 0) {
        (**(code **)(*DAT_006699d0 + 0x38))(DAT_006699d0,*puVar5);
      }
      else {
        FUN_0056d120(puVar5 + 1);
      }
      puVar5[0x15] = 0;
    }
    uVar19 = local_34;
    if (0xfe < (int)local_34) {
      uVar19 = 0xff;
    }
    uVar16 = local_68;
    if (0xfe < (int)local_68) {
      uVar16 = 0xff;
    }
    if (0xfe < (int)uVar13) {
      uVar13 = 0xff;
    }
    if (DAT_005d7a28 == 0) {
      iVar11 = (**(code **)(*DAT_00668f14 + 0x60))
                         (DAT_00668f14,2,((uVar19 | 0xffffff00) << 8 | uVar16) << 8 | uVar13);
      if (iVar11 != 0) {
        FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c6444,0x42a);
      }
    }
    else {
      FUN_0056d570(((uVar19 | 0xffffff00) << 8 | uVar16) << 8 | uVar13);
    }
  }
  else {
    iVar17 = 100 - _DAT_005c61bc;
    iVar11 = (int)(iVar17 * local_34) / 100;
    if (0xfe < iVar11) {
      iVar11 = 0xff;
    }
    uVar19 = (int)(iVar17 * local_68) / 100;
    if (0xfe < (int)uVar19) {
      uVar19 = 0xff;
    }
    uVar13 = (int)(iVar17 * uVar13) / 100;
    if (0xfe < (int)uVar13) {
      uVar13 = 0xff;
    }
    if (DAT_005d7a28 == 0) {
      iVar11 = (**(code **)(*DAT_00668f14 + 0x60))
                         (DAT_00668f14,2,(iVar11 << 8 | uVar19) << 8 | uVar13);
      if (iVar11 != 0) {
        FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c642c,0x40e);
      }
    }
    else {
      FUN_0056d570((iVar11 << 8 | uVar19) << 8 | uVar13);
    }
    fVar7 = _DAT_005a36dc;
    if ((DAT_005c61b4 != 0) && (DAT_005d7a18 != 0)) {
      fVar7 = _DAT_005a3810;
    }
    local_160 = (float)_DAT_005c61bc * fVar7 * _DAT_005a350c;
    if ((-1 < DAT_005e8980) &&
       (puVar5 = *(undefined4 **)(DAT_005e8870 + DAT_005e8980 * 4), puVar5[0x15] == 0)) {
      if (DAT_005d7a28 == 0) {
        iVar11 = (**(code **)(*DAT_006699d0 + 0x34))(DAT_006699d0,*puVar5);
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c6204,0x178);
        }
      }
      else {
        FUN_0056cf00(puVar5 + 1);
      }
      puVar5[0x15] = 1;
    }
    fVar7 = (float)(int)local_74 * local_160 * _DAT_005a3534;
    if (_DAT_005a34e4 <= fVar7) {
      fVar7 = _DAT_005a34e4;
    }
    fVar8 = (float)(int)local_68 * local_160 * _DAT_005a3534;
    if (_DAT_005a34e4 <= fVar8) {
      fVar8 = _DAT_005a34e4;
    }
    fVar9 = (float)(int)local_34 * local_160 * _DAT_005a3534;
    if (_DAT_005a34e4 <= fVar9) {
      fVar9 = _DAT_005a34e4;
    }
    puVar5 = *(undefined4 **)(DAT_005e8870 + DAT_005e8980 * 4);
    puVar5[3] = fVar9;
    puVar5[4] = fVar8;
    puVar5[5] = fVar7;
    puVar5[6] = 0x3f800000;
    (**(code **)(*(int *)*puVar5 + 0x10))((int *)*puVar5,puVar5 + 1);
  }
  local_6c = 0;
  do {
    if (1 < local_6c) {
      if ((DAT_005e91c8 != 0) &&
         (iVar12 = (**(code **)(**(int **)(PTR_DAT_005d79e0 + 0x8c) + 0x2c))(),
         iVar17 = DAT_005e91d0, iVar11 = DAT_005e91cc, iVar12 != 0)) {
        piVar2 = *(int **)(PTR_DAT_005d79e0 + 0x8c);
        iVar15 = piVar2[4] * DAT_005e91d0 + DAT_005e91cc;
        if ((DAT_005e91d0 < 0x1e0) &&
           (((-1 < DAT_005e91d0 && (DAT_005e91cc < 0x280)) && (-1 < DAT_005e91cc)))) {
          *(undefined2 *)(iVar12 + iVar15 * 2) = local_30;
        }
        iVar1 = iVar17 + 1;
        if (iVar1 < 0x1e0) {
          if (((-1 < iVar1) && (iVar11 + -1 < 0x280)) && (-1 < iVar11 + -1)) {
            *(undefined2 *)(iVar12 + -2 + (piVar2[4] + iVar15) * 2) = local_2c;
            iVar11 = DAT_005e91cc;
            iVar17 = DAT_005e91d0;
          }
          if (((iVar1 < 0x1e0) && (-1 < iVar1)) && ((iVar11 + 1 < 0x280 && (-1 < iVar11 + 1)))) {
            *(undefined2 *)(iVar12 + 2 + (piVar2[4] + iVar15) * 2) = local_28;
            iVar17 = DAT_005e91d0;
          }
        }
        iVar1 = iVar17 + -1;
        if (iVar1 < 0x1e0) {
          if (((-1 < iVar1) && (iVar11 + -1 < 0x280)) && (-1 < iVar11 + -1)) {
            *(undefined2 *)(iVar12 + -2 + (iVar15 - piVar2[4]) * 2) = local_2e;
            iVar11 = DAT_005e91cc;
            iVar17 = DAT_005e91d0;
          }
          if (((iVar1 < 0x1e0) && (-1 < iVar1)) && ((iVar11 + 1 < 0x280 && (-1 < iVar11 + 1)))) {
            *(undefined2 *)(iVar12 + 2 + (iVar15 - piVar2[4]) * 2) = local_2a;
            iVar17 = DAT_005e91d0;
          }
        }
        if (((iVar17 + -2 < 0x1e0) && (-1 < iVar17 + -2)) && ((iVar11 < 0x280 && (-1 < iVar11)))) {
          *(undefined2 *)(iVar12 + (iVar15 + piVar2[4] * -2) * 2) = local_20;
          iVar17 = DAT_005e91d0;
        }
        if ((((iVar17 + 2 < 0x1e0) && (-1 < iVar17 + 2)) && (iVar11 < 0x280)) && (-1 < iVar11)) {
          *(undefined2 *)(iVar12 + (iVar15 + piVar2[4] * 2) * 2) = local_24;
          iVar17 = DAT_005e91d0;
        }
        if (iVar17 < 0x1e0) {
          if (((-1 < iVar17) && (iVar11 + -2 < 0x280)) && (-1 < iVar11 + -2)) {
            *(undefined2 *)(iVar12 + -4 + iVar15 * 2) = local_26;
          }
          if (((iVar17 < 0x1e0) && (-1 < iVar17)) && ((iVar11 + 2 < 0x280 && (-1 < iVar11 + 2)))) {
            *(undefined2 *)(iVar12 + 4 + iVar15 * 2) = local_22;
          }
        }
        (**(code **)(*piVar2 + 0x30))();
      }
      iVar11 = 0;
      if (0 < DAT_005e8890) {
        do {
          iVar17 = *(int *)(DAT_005e88a0 + iVar11 * 4);
          if ((iVar17 != 0) && (piVar2 = *(int **)(iVar17 + 4), piVar2 != (int *)0x0)) {
            if ((DAT_0066829c != 0) && (((short)piVar2[1] == 0xb && (piVar2 != DAT_00667fcc)))) {
              (**(code **)(*piVar2 + 0x24))();
              FUN_004da1c0();
            }
            if (((short)piVar2[1] == 0xb) || ((short)piVar2[1] == 0xc)) {
              (**(code **)(*piVar2 + 0x24))();
              FUN_004da750();
            }
          }
          iVar11 = iVar11 + 1;
        } while (iVar11 < DAT_005e8890);
      }
      if ((local_60 != 0) && (DAT_005e8850 != 0)) {
        DAT_005e8850 = 0;
        if (DAT_005d7a28 == 0) {
          (**(code **)(*DAT_00668f14 + 0x28))(DAT_00668f14);
        }
        else {
          FUN_0056d330();
        }
      }
      if (((DAT_005d7a28 == 0) && (DAT_006671f0 != 0)) &&
         (iVar11 = FUN_00417060(0x1c,0), iVar11 != 0)) {
        FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c64f0,0x4ef);
      }
      FUN_00412150(0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),*(undefined4 *)(PTR_DAT_005d79e0 + 8));
      ExceptionList = local_14;
      return 1;
    }
    local_58 = 0;
    while (local_58 < DAT_005e8890) {
      iVar11 = *(int *)(DAT_005e88a0 + local_58 * 4);
      if ((iVar11 == 0) ||
         (piVar2 = *(int **)(iVar11 + 4), local_17c = piVar2, piVar2 == (int *)0x0)) {
LAB_00413f8c:
        local_58 = local_58 + 1;
      }
      else {
        iVar11 = *(int *)(iVar11 + 8);
        bVar20 = (short)piVar2[1] == 0x19;
        if ((**(int **)(*(int *)(iVar11 + 4) + 0x54) != 1) &&
           (iVar17 = (**(code **)(*piVar2 + 0x134))(), iVar17 == 0)) {
          bVar20 = true;
        }
        if (((((bVar20 != (local_6c == 1)) || (piVar2[0x19] != 0)) ||
             ((piVar6 = *(int **)(*(int *)(iVar11 + 4) + 0x54), *piVar6 == 1 &&
              (((*(ushort *)(piVar2 + 0x17) == 0xffff ||
                ((uint)piVar6[1] <= (uint)*(ushort *)(piVar2 + 3))) ||
               (*(ushort *)((int)piVar6 + (uint)*(ushort *)(piVar2 + 3) * 0x4c + 0x32) <=
                *(ushort *)(piVar2 + 0x17))))))) ||
            (((piVar2[2] & 0x180U) != 0 && (DAT_00668154 == 0)))) ||
           (*(ushort *)((int)piVar2 + 0xe) != DAT_00666970)) goto LAB_00413f8c;
        DAT_005e874c = 9999;
        DAT_005e8748 = 9999;
        DAT_005e8754 = -9999;
        DAT_005e8750 = -9999;
        local_19c = 2;
        local_198 = 0;
        local_184 = 0x461c3c00;
        local_18c = 0x461c3c00;
        local_194 = 0x461c3c00;
        local_180 = 0xc61c3c00;
        local_188 = 0xc61c3c00;
        local_190 = 0xc61c3c00;
        if (DAT_005d7a28 == 0) {
          (**(code **)(*DAT_00668f14 + 0x78))(DAT_00668f14,&local_19c);
        }
        else {
          FUN_0056d360(&local_19c);
        }
        iVar11 = FUN_00417060(8,3);
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c645c,0x475);
        }
        iVar11 = FUN_00417060(0x1d,DAT_005c61a8);
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c6474,0x476);
        }
        iVar11 = FUN_00417060(4,DAT_005e91b8);
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c648c,0x477);
        }
        iVar11 = FUN_00417060(0x11,(DAT_005c61b0 != 0) + '\x01');
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c64a4,0x478);
        }
        iVar11 = FUN_00417060(0x12,(DAT_005c61b0 != 0) + '\x01');
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11,s_d__revenant_3DScene_cpp_005c64bc,0x479);
        }
        FUN_00417d60(1,1);
        local_8 = 0;
        (**(code **)(**(int **)(DAT_005e88a0 + local_58 * 4) + 0x40))();
        local_8 = 0xffffffff;
        if ((DAT_005e91c8 == 0) || ((local_6c != 0 && (DAT_00668154 == 0)))) goto LAB_00413f8c;
        if (DAT_005d7a28 == 0) {
          if ((((DAT_005e8750 < local_54) || (local_4c < DAT_005e8748)) || (DAT_005e8754 < local_50)
              ) || (local_48 < DAT_005e874c)) {
            bVar20 = false;
          }
          else {
            bVar20 = true;
          }
          if (!bVar20) goto LAB_00413f8c;
        }
        if ((local_60 != 0) && (DAT_005e8850 != 0)) {
          DAT_005e8850 = 0;
          if (DAT_005d7a28 == 0) {
            (**(code **)(*DAT_00668f14 + 0x28))(DAT_00668f14);
          }
          else {
            FUN_0056d330();
          }
        }
        iVar12 = (**(code **)(**(int **)(PTR_DAT_005d79e0 + 0x8c) + 0x2c))();
        iVar17 = DAT_005e91d0;
        iVar11 = DAT_005e91cc;
        if (iVar12 != 0) {
          piVar2 = *(int **)(PTR_DAT_005d79e0 + 0x8c);
          iVar15 = piVar2[4] * DAT_005e91d0 + DAT_005e91cc;
          if (((DAT_005e91d0 < 0x1e0) && (-1 < DAT_005e91d0)) &&
             ((DAT_005e91cc < 0x280 && (-1 < DAT_005e91cc)))) {
            local_1d8[0] = *(short *)(iVar12 + iVar15 * 2);
            *(undefined2 *)(iVar12 + iVar15 * 2) = 0xf81f;
          }
          iVar17 = iVar17 + 1;
          if (iVar17 < 0x1e0) {
            if (((-1 < iVar17) && (iVar11 + -1 < 0x280)) && (-1 < iVar11 + -1)) {
              local_1d8[2] = *(undefined2 *)(iVar12 + -2 + (piVar2[4] + iVar15) * 2);
              *(undefined2 *)(iVar12 + -2 + (piVar2[4] + iVar15) * 2) = 0xf81f;
            }
            if (((iVar17 < 0x1e0) && (-1 < iVar17)) && ((iVar11 + 1 < 0x280 && (-1 < iVar11 + 1))))
            {
              local_1d0 = *(undefined2 *)(iVar12 + 2 + (piVar2[4] + iVar15) * 2);
              *(undefined2 *)(iVar12 + 2 + (piVar2[4] + iVar15) * 2) = 0xf81f;
            }
          }
          iVar17 = DAT_005e91d0 + -1;
          if (iVar17 < 0x1e0) {
            if (((-1 < iVar17) && (iVar11 + -1 < 0x280)) && (-1 < iVar11 + -1)) {
              local_1d8[1] = *(undefined2 *)(iVar12 + -2 + (iVar15 - piVar2[4]) * 2);
              *(undefined2 *)(iVar12 + -2 + (iVar15 - piVar2[4]) * 2) = 0xf81f;
            }
            if ((((iVar17 < 0x1e0) && (-1 < iVar17)) && (iVar11 + 1 < 0x280)) && (-1 < iVar11 + 1))
            {
              local_1d8[3] = *(undefined2 *)(iVar12 + 2 + (iVar15 - piVar2[4]) * 2);
              *(undefined2 *)(iVar12 + 2 + (iVar15 - piVar2[4]) * 2) = 0xf81f;
            }
          }
          if (((DAT_005e91d0 + -2 < 0x1e0) && (-1 < DAT_005e91d0 + -2)) &&
             ((iVar11 < 0x280 && (-1 < iVar11)))) {
            local_1c8 = *(undefined2 *)(iVar12 + (iVar15 + piVar2[4] * -2) * 2);
            *(undefined2 *)(iVar12 + (iVar15 + piVar2[4] * -2) * 2) = 0xf81f;
          }
          iVar17 = DAT_005e91d0;
          if (((DAT_005e91d0 + 2 < 0x1e0) && (-1 < DAT_005e91d0 + 2)) &&
             ((iVar11 < 0x280 && (-1 < iVar11)))) {
            local_1cc = *(undefined2 *)(iVar12 + (iVar15 + piVar2[4] * 2) * 2);
            *(undefined2 *)(iVar12 + (iVar15 + piVar2[4] * 2) * 2) = 0xf81f;
          }
          if (iVar17 < 0x1e0) {
            if (((-1 < iVar17) && (iVar11 + -2 < 0x280)) && (-1 < iVar11 + -2)) {
              local_1ce = *(undefined2 *)(iVar12 + -4 + iVar15 * 2);
              *(undefined2 *)(iVar12 + -4 + iVar15 * 2) = 0xf81f;
            }
            if (((iVar17 < 0x1e0) && (-1 < iVar17)) && ((iVar11 + 2 < 0x280 && (-1 < iVar11 + 2))))
            {
              local_1ca = *(undefined2 *)(iVar12 + 4 + iVar15 * 2);
              *(undefined2 *)(iVar12 + 4 + iVar15 * 2) = 0xf81f;
            }
          }
          (**(code **)(*piVar2 + 0x30))();
          iVar11 = 0;
          psVar14 = local_1d8;
          do {
            if (*psVar14 != -0x7e1) {
              DAT_005e91d4 = local_17c;
              break;
            }
            iVar11 = iVar11 + 1;
            psVar14 = psVar14 + 1;
          } while (iVar11 < 9);
        }
        if (DAT_005e8850 != 0) goto LAB_00413f8c;
        DAT_005e8850 = 1;
        if (DAT_005d7a28 == 0) {
          (**(code **)(*DAT_00668f14 + 0x24))(DAT_00668f14);
          goto LAB_00413f8c;
        }
        FUN_0056d260();
        local_58 = local_58 + 1;
      }
    }
    local_6c = local_6c + 1;
  } while( true );
}


