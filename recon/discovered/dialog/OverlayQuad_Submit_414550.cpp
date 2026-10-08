// FUN_00414550 @ 00414550 size=2065

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4
FUN_00414550(int param_1,int param_2,int param_3,undefined4 param_4,int param_5,int param_6,
            int param_7,int param_8,int param_9,undefined4 param_10,int param_11,int param_12,
            int param_13,int param_14,int param_15,int param_16,int param_17,uint param_18)

{
  float fVar1;
  float fVar2;
  bool bVar3;
  undefined4 uVar4;
  char cVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  char *pcVar14;
  undefined4 uVar15;
  undefined2 local_90;
  undefined2 local_8e;
  undefined2 local_8c;
  undefined2 local_8a;
  undefined2 local_88;
  undefined2 local_86;
  float local_84;
  float local_80;
  float local_7c;
  float local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  float local_68;
  float local_64;
  float local_60;
  float local_5c;
  float local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  float local_8;
  float local_4;
  
  fVar1 = _DAT_005a34e4 / (float)param_11;
  local_6c = 0;
  local_74 = 0x3f800000;
  local_4c = 0;
  local_54 = 0x3f800000;
  local_2c = 0;
  fVar2 = _DAT_005a34e4 / (float)param_12;
  local_68 = (float)param_13 * fVar1 + DAT_005c61cc * fVar1;
  local_64 = (float)param_14 * fVar2 + _DAT_005c61d0 * fVar2;
  param_8 = param_1 + param_8;
  local_70 = param_10;
  local_84 = (float)(param_13 + param_15) * fVar1 + DAT_005e91d8 * fVar1 + DAT_005c61cc * fVar1;
  local_50 = param_10;
  local_30 = param_10;
  local_24 = (float)(param_14 + param_16) * fVar2 + DAT_005e91dc * fVar2 + _DAT_005c61d0 * fVar2;
  local_78 = (float)param_3 * _DAT_005a3834;
  local_80 = (float)param_1;
  local_7c = (float)param_2;
  local_60 = (float)param_8;
  param_9 = param_2 + param_9;
  local_3c = (float)param_9;
  local_10 = param_10;
  local_34 = 0x3f800000;
  local_c = 0;
  local_14 = 0x3f800000;
  local_5c = local_7c;
  local_58 = local_78;
  local_48 = local_84;
  local_44 = local_64;
  local_40 = local_80;
  local_38 = local_78;
  local_28 = local_68;
  local_20 = local_60;
  local_1c = local_3c;
  local_18 = local_78;
  local_8 = local_84;
  local_4 = local_24;
  if (param_17 != 0) {
    FUN_0043ade0(param_17,&local_80,&local_80,&local_74);
    FUN_0043ade0(param_17,&local_80,&local_80,&local_54);
    FUN_0043ade0(param_17,&local_80,&local_80,&local_34);
    FUN_0043ade0(param_17,&local_80,&local_80,&local_14);
  }
  local_90 = 0;
  local_8e = 1;
  local_8c = 2;
  local_8a = 1;
  local_88 = 3;
  local_86 = 2;
  bVar3 = false;
  if (DAT_005e8850 == 0) {
    DAT_005e8850 = 1;
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 0x24))(DAT_00668f14);
    }
    else {
      FUN_0056d260();
    }
    bVar3 = true;
  }
  if (param_5 == 0) {
    DAT_005e87e0 = 0;
    DAT_005e8988 = 0;
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 0x98))(DAT_00668f14,0,0);
    }
    else {
      FUN_0056d3a0(0,0);
    }
  }
  else {
    DAT_005e87e0 = param_5;
    if (DAT_005d7a28 == 0) {
      DAT_005e8988 = param_4;
      (**(code **)(*DAT_00668f14 + 0x98))(DAT_00668f14,0,param_5);
    }
    else {
      DAT_005e8988 = param_4;
      FUN_0056d3a0(param_5,param_4);
    }
  }
  if (param_7 == 0) {
    if (DAT_005d7a28 == 0) {
      _DAT_005e87e4 = 0;
      _DAT_005e898c = 0;
      (**(code **)(*DAT_00668f14 + 0x98))(DAT_00668f14,1,0);
    }
  }
  else if (DAT_005d7a28 == 0) {
    _DAT_005e87e4 = param_7;
    _DAT_005e898c = param_6;
    (**(code **)(*DAT_00668f14 + 0x98))(DAT_00668f14,1,param_7);
  }
  uVar4 = DAT_005e91c0;
  cVar5 = (param_6 != 0) + '\x01';
  DAT_005e91c0 = param_18 >> 0x18 & 1;
  if ((param_18 & 2) == 0) {
    if ((param_18 & 4) == 0) {
      if ((param_18 & 8) == 0) {
        if ((param_18 & 0x10) == 0) {
          iVar6 = FUN_00417d60(1,cVar5);
          if (iVar6 != 0) {
            uVar15 = 0x59f;
            pcVar14 = s_d__revenant_3DScene_cpp_005c6568;
            goto LAB_004149c4;
          }
        }
        else {
          iVar6 = FUN_00417d60(0x10,cVar5);
          if (iVar6 != 0) {
            uVar15 = 0x59b;
            pcVar14 = s_d__revenant_3DScene_cpp_005c6550;
LAB_004149c4:
            FUN_004a90d0(iVar6,pcVar14,uVar15);
          }
        }
      }
      else {
        iVar6 = FUN_00417d60(8,cVar5);
        if (iVar6 != 0) {
          uVar15 = 0x597;
          pcVar14 = s_d__revenant_3DScene_cpp_005c6538;
          goto LAB_004149c4;
        }
      }
    }
    else {
      iVar6 = FUN_00417d60(4,cVar5);
      if (iVar6 != 0) {
        uVar15 = 0x593;
        pcVar14 = s_d__revenant_3DScene_cpp_005c6520;
        goto LAB_004149c4;
      }
    }
  }
  else {
    iVar6 = FUN_00417d60(2,cVar5);
    if (iVar6 != 0) {
      uVar15 = 0x58f;
      pcVar14 = s_d__revenant_3DScene_cpp_005c6508;
      goto LAB_004149c4;
    }
  }
  if ((param_18 & 0x40) == 0) {
    iVar6 = FUN_00417060(0xe,0);
    if (iVar6 != 0) {
      uVar15 = 0x5a8;
      pcVar14 = s_d__revenant_3DScene_cpp_005c6598;
      goto LAB_00414a12;
    }
  }
  else {
    iVar6 = FUN_00417060(0xe,1);
    if (iVar6 != 0) {
      uVar15 = 0x5a4;
      pcVar14 = s_d__revenant_3DScene_cpp_005c6580;
LAB_00414a12:
      FUN_004a90d0(iVar6,pcVar14,uVar15);
    }
  }
  if ((param_18 & 0x80) == 0) {
    iVar6 = FUN_00417060(7,0);
    if (iVar6 == 0) goto LAB_00414a62;
    uVar15 = 0x5b0;
    pcVar14 = s_d__revenant_3DScene_cpp_005c65c8;
  }
  else {
    iVar6 = FUN_00417060(7,1);
    if (iVar6 == 0) goto LAB_00414a62;
    uVar15 = 0x5ac;
    pcVar14 = s_d__revenant_3DScene_cpp_005c65b0;
  }
  FUN_004a90d0(iVar6,pcVar14,uVar15);
LAB_00414a62:
  if (((param_18 & 0x200) != 0) && (iVar6 = FUN_00417060(4,0), iVar6 != 0)) {
    FUN_004a90d0(iVar6,s_d__revenant_3DScene_cpp_005c65e0,0x5b5);
  }
  iVar6 = FUN_00417060(0x16,1);
  if (iVar6 != 0) {
    FUN_004a90d0(iVar6,s_d__revenant_3DScene_cpp_005c65f8,0x5b8);
  }
  iVar6 = FUN_00417060(0x11,1);
  if (iVar6 != 0) {
    FUN_004a90d0(iVar6,s_d__revenant_3DScene_cpp_005c6610,0x5b9);
  }
  iVar6 = FUN_00417060(0x12,1);
  if (iVar6 != 0) {
    FUN_004a90d0(iVar6,s_d__revenant_3DScene_cpp_005c6628,0x5ba);
  }
  iVar6 = FUN_00417060(0x1a,0);
  if (iVar6 != 0) {
    FUN_004a90d0(iVar6,s_d__revenant_3DScene_cpp_005c6640,0x5bb);
  }
  iVar6 = FUN_004174b0(4,0x1c4,&local_80,4,&local_90,6,0);
  if (iVar6 != 0) {
    FUN_004a90d0(iVar6,s_d__revenant_3DScene_cpp_005c6658,0x5c5);
  }
  if ((bVar3) && (DAT_005e8850 != 0)) {
    DAT_005e8850 = 0;
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 0x28))(DAT_00668f14);
    }
    else {
      FUN_0056d330();
    }
  }
  if (param_17 != 0) {
    iVar7 = __ftol();
    iVar8 = __ftol();
    iVar6 = iVar8;
    if (iVar7 <= iVar8) {
      iVar6 = iVar7;
    }
    iVar9 = __ftol();
    param_8 = __ftol();
    iVar10 = param_8;
    if (iVar9 <= param_8) {
      iVar10 = iVar9;
    }
    if (iVar6 < iVar10) {
      param_1 = iVar8;
      if (iVar7 <= iVar8) {
        param_1 = iVar7;
      }
    }
    else {
      param_1 = param_8;
      if (iVar9 <= param_8) {
        param_1 = iVar9;
      }
    }
    param_9 = __ftol();
    iVar10 = __ftol();
    iVar6 = param_9;
    if (iVar10 < param_9) {
      iVar6 = iVar10;
    }
    iVar11 = __ftol();
    iVar12 = __ftol();
    iVar13 = iVar12;
    if (iVar11 <= iVar12) {
      iVar13 = iVar11;
    }
    if (iVar6 < iVar13) {
      param_2 = iVar10;
      if (param_9 <= iVar10) {
        param_2 = param_9;
      }
    }
    else {
      param_2 = iVar12;
      if (iVar11 <= iVar12) {
        param_2 = iVar11;
      }
    }
    iVar6 = iVar8;
    if (iVar8 <= iVar7) {
      iVar6 = iVar7;
    }
    iVar13 = param_8;
    if (param_8 <= iVar9) {
      iVar13 = iVar9;
    }
    if (iVar13 < iVar6) {
      param_8 = iVar7;
      if (iVar7 < iVar8) {
        param_8 = iVar8;
      }
    }
    else if (param_8 <= iVar9) {
      param_8 = iVar9;
    }
    iVar6 = iVar10;
    if (iVar10 <= param_9) {
      iVar6 = param_9;
    }
    iVar7 = iVar12;
    if (iVar12 <= iVar11) {
      iVar7 = iVar11;
    }
    if (iVar7 < iVar6) {
      if (param_9 < iVar10) {
        param_9 = iVar10;
      }
    }
    else {
      param_9 = iVar12;
      if (iVar12 <= iVar11) {
        param_9 = iVar11;
      }
    }
  }
  if ((param_18 & 0x400) == 0) {
    if ((param_18 & 0x800) == 0) {
      DAT_005e91c0 = uVar4;
      return 1;
    }
    FUN_004aacb0(param_1,param_2,(param_1 - param_8) + 1,(param_2 - param_9) + 1,6);
    DAT_005e91c0 = uVar4;
    return 1;
  }
  FUN_004aacb0(param_1,param_2,(param_1 - param_8) + 1,(param_2 - param_9) + 1,1);
  DAT_005e91c0 = uVar4;
  return 1;
}


