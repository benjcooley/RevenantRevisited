// Decompiled methods and structure for class: cls_0x411eb0

/*
/OOAnalyzer/cls_0x411eb0
pack(disabled)
Structure cls_0x411eb0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 00411eb0

undefined4 __thiscall OOAnalyzer::cls_0x411eb0::meth_0x411eb0(cls_0x411eb0 *this)

{
  int iVar1;
  int **ppiVar2;
  int *piVar3;
  int *piVar4;
  int *apiStack_78 [2];
  
  iVar1 = DAT_00669ad8;
  if ((this->mbr_0x0 == 0) && (DAT_00668150 == 0)) {
    this->mbr_0x0 = 1;
    if (iVar1 == 0) {
      if (DAT_005d7a28 != 0) {
        FUN_0056cb60();
      }
    }
    else {
      DAT_005d7a28 = 0;
    }
    iVar1 = (**(code **)(*DAT_00668eb0 + 0x18))();
    if (iVar1 != 0) {
      apiStack_78[1] = (int *)0x411f20;
      FUN_004a90d0(iVar1);
    }
    apiStack_78[1] = (int *)0x411f37;
    iVar1 = (**(code **)*DAT_006699d0)();
    if (iVar1 != 0) {
      apiStack_78[1] = (int *)0x1b2;
      apiStack_78[0] = (int *)s_d__revenant_3DScene_cpp_005c6234;
      FUN_004a90d0(iVar1);
    }
    apiStack_78[1] = DAT_006699d0;
    apiStack_78[0] = DAT_00668f14;
    iVar1 = (**(code **)(*DAT_00668f14 + 0x14))();
    if (iVar1 != 0) {
      FUN_004a90d0(iVar1);
    }
    iVar1 = (**(code **)(*DAT_00668f14 + 0x30))();
    if (iVar1 != 0) {
      FUN_004a90d0(iVar1);
    }
    meth_0x412150(this,0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4));
    ppiVar2 = apiStack_78;
    for (iVar1 = 0x14; iVar1 != 0; iVar1 = iVar1 + -1) {
      *ppiVar2 = (int *)0x0;
      ppiVar2 = ppiVar2 + 1;
    }
    apiStack_78[0] = (int *)0x50;
    iVar1 = (**(code **)(*DAT_00668eb0 + 0x14))(DAT_00668eb0,&DAT_005e8918,0);
    if (iVar1 != 0) {
      FUN_004a90d0(iVar1);
    }
    iVar1 = (**(code **)(*DAT_005e8918 + 0xc))(DAT_005e8918,&stack0xffffff7c);
    if (iVar1 != 0) {
      FUN_004a90d0(iVar1);
    }
    piVar3 = DAT_00668f14;
    iVar1 = (**(code **)(*DAT_005e8918 + 0x14))(DAT_005e8918,DAT_00668f14,&DAT_005e8800);
    if (iVar1 != 0) {
      FUN_004a90d0(iVar1);
    }
    piVar4 = DAT_005e8800;
    iVar1 = (**(code **)(*piVar3 + 0x20))(piVar3);
    if (iVar1 != 0) {
      FUN_004a90d0(iVar1);
    }
    if (piVar4 != (int *)0x0) {
      (**(code **)(*piVar4 + 8))(piVar4);
      piVar4 = (int *)0x0;
    }
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 100))(DAT_00668f14,2,&DAT_005e8798,piVar3,piVar4);
    }
    else {
      FUN_0056d5f0(2,(undefined4 *)&DAT_005e8798);
    }
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 100))(DAT_00668f14,3,0x5e88c8);
    }
    else {
      FUN_0056d5f0(3,(undefined4 *)&UNK_005e88c4.field_0x4);
    }
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 100))(DAT_00668f14,1,&DAT_005e8798);
    }
    else {
      FUN_0056d5f0(1,(undefined4 *)&DAT_005e8798);
    }
    DAT_005e8934 = 0;
    (**(code **)(*DAT_00668f14 + 0x20))(DAT_00668f14,&LAB_00416460,this);
    meth_0x412490(this);
  }
  return 1;
}



// Function at 00412150

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall
OOAnalyzer::cls_0x411eb0::meth_0x412150(cls_0x411eb0 *this,int param_1,int param_2,int param_3)

{
  undefined4 uVar1;
  int iVar2;
  int *piVar3;
  int in_stack_00000010;
  int local_2c [5];
  float local_18;
  float local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  if ((this->mbr_0x0 != 0) && (DAT_00668150 == 0)) {
    if (DAT_005d7a28 != 0) {
      uVar1 = FUN_0056ccd0(param_1,param_2,param_3,in_stack_00000010);
      return uVar1;
    }
    DAT_005e887c = param_1;
    piVar3 = local_2c;
    for (iVar2 = 0xb; iVar2 != 0; iVar2 = iVar2 + -1) {
      *piVar3 = 0;
      piVar3 = piVar3 + 1;
    }
    local_2c[1] = param_1;
    local_2c[0] = 0x2c;
    local_2c[2] = param_2;
    local_2c[3] = param_3;
    local_10 = 0x3f800000;
    local_c = 0x3f800000;
    DAT_005e8878 = param_2;
    DAT_005e892c = param_3;
    DAT_005e8884 = in_stack_00000010;
    local_18 = (_DAT_005a37f8 / SQRT((float)_DAT_005a3800)) * (float)_DAT_005a37f0;
    local_14 = local_18;
    iVar2 = (**(code **)(*DAT_006699d0 + 0x14))(DAT_006699d0,local_2c);
    if (iVar2 != 0) {
      FUN_004a90d0(iVar2);
    }
  }
  return 1;
}



// Function at 00412490

undefined4 __thiscall OOAnalyzer::cls_0x411eb0::meth_0x412490(cls_0x411eb0 *this)

{
  void **ppvVar1;
  int **ppiVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  int **ppiVar6;
  int **ppiVar7;
  int *local_5c [4];
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059c7eb;
  local_c = ExceptionList;
  if (this->mbr_0x0 != 0) {
    uVar4 = 0;
    iVar5 = DAT_005e88a0;
    uVar3 = DAT_005e8890;
    ExceptionList = &local_c;
    ppvVar1 = &local_c;
    if (0 < (int)DAT_005e8890) {
      do {
        ExceptionList = ppvVar1;
        if (((iVar5 != 0) && (uVar4 < uVar3)) && (*(int *)(iVar5 + uVar4 * 4) != 0)) {
          meth_0x4162c0(this);
          iVar5 = DAT_005e88a0;
          uVar3 = DAT_005e8890;
        }
        uVar4 = uVar4 + 1;
        ppvVar1 = (void **)ExceptionList;
      } while ((int)uVar4 < (int)uVar3);
    }
    iVar5 = 0;
    if (0 < DAT_005e8860) {
      do {
        if ((-1 < iVar5) && (ppiVar2 = *(int ***)(DAT_005e8870 + iVar5 * 4), ppiVar2 != (int **)0x0)
           ) {
          FUN_00418590(ppiVar2,1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&DAT_005e8860);
        iVar5 = iVar5 + 1;
      } while (iVar5 < DAT_005e8860);
    }
    DAT_005e8860 = 0;
    DAT_005e8864._0_4_ = 0;
    meth_0x412150(this,0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4));
    ppiVar2 = local_5c;
    for (iVar5 = 0x14; iVar5 != 0; iVar5 = iVar5 + -1) {
      *ppiVar2 = (int *)0x0;
      ppiVar2 = ppiVar2 + 1;
    }
    local_5c[0] = (int *)0x50;
    local_5c[1] = (int *)0x3;
    local_5c[2] = (int *)0x3f800000;
    local_5c[3] = (int *)0x3f800000;
    local_4c = 0x3f800000;
    local_48 = 0x3f800000;
    local_44 = 0;
    local_40 = 0;
    local_3c = 0x43480000;
    local_38 = 0;
    local_34 = 0xbf480000;
    local_30 = 0xbf200000;
    local_2c = 0x49742400;
    local_28 = 0;
    local_24 = 0x3f800000;
    local_20 = 0;
    local_1c = 0;
    local_18 = 0;
    local_14 = 0;
    local_10 = 1;
    ppiVar2 = (int **)FUN_00482fb0(0x60);
    local_4 = 0;
    if (ppiVar2 != (int **)0x0) {
      ppiVar6 = local_5c;
      ppiVar7 = ppiVar2;
      for (iVar5 = 0x14; ppiVar7 = ppiVar7 + 1, iVar5 != 0; iVar5 = iVar5 + -1) {
        *ppiVar7 = *ppiVar6;
        ppiVar6 = ppiVar6 + 1;
      }
      ppiVar2[0x16] = (int *)0x1;
      ppiVar2[0x17] = (int *)0x1;
      FUN_004118e0(ppiVar2);
    }
    local_4 = 0xffffffff;
    DAT_005e8980 = cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&DAT_005e8860);
    ExceptionList = local_c;
    return 1;
  }
  return 0;
}



// Function at 00412cd0

void __thiscall OOAnalyzer::cls_0x411eb0::meth_0x412cd0(cls_0x411eb0 *this)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  if ((((DAT_005e91c4 == 0) && (DAT_005e91bc == 0)) && (DAT_006680e8 == 0)) && (DAT_005d7a10 != 0))
  {
    if (DAT_005d79f8 == 0) {
LAB_00412d67:
      local_10 = DAT_00666904;
      local_8 = DAT_006668e4 + -1 + DAT_00666904;
      local_c = DAT_00666908;
      local_4 = DAT_006668e8 + -1 + DAT_00666908;
      FUN_00412680(&local_10);
    }
    else {
      iVar4 = 0;
      iVar3 = DAT_005e8890;
      if (0 < DAT_005e8890) {
        do {
          piVar1 = *(int **)(DAT_005e88a0 + iVar4 * 4);
          if ((piVar1 != (int *)0x0) &&
             (iVar2 = (**(code **)(*piVar1 + 0x38))(), iVar3 = DAT_005e8890, iVar2 != 0))
          goto LAB_00412d67;
          iVar4 = iVar4 + 1;
        } while (iVar4 < iVar3);
      }
      iVar4 = 0;
      if (0 < iVar3) {
        do {
          piVar1 = *(int **)(DAT_005e88a0 + iVar4 * 4);
          if (piVar1 != (int *)0x0) {
            (**(code **)(*piVar1 + 0x3c))();
            iVar3 = DAT_005e8890;
          }
          iVar4 = iVar4 + 1;
        } while (iVar4 < iVar3);
        return;
      }
    }
  }
  return;
}



// Function at 00412db0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall OOAnalyzer::cls_0x411eb0::meth_0x412db0(cls_0x411eb0 *this)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 *puVar5;
  int **ppiVar6;
  int *piVar7;
  float fVar8;
  int *piVar9;
  undefined *puVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  cls_0x5a7e38 *this_00;
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
  undefined local_40 [8];
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
  undefined *local_1c;
  void *local_14;
  undefined *puStack_10;
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
  if (this->mbr_0x0 == 0) {
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
  meth_0x412150((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),DAT_006663d8,DAT_006663d4,DAT_00667c30);
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
  iVar11 = meth_0x417060(this,9);
  if (iVar11 != 0) {
    FUN_004a90d0(iVar11);
  }
  iVar11 = meth_0x417060(this,0x1a);
  if (iVar11 != 0) {
    FUN_004a90d0(iVar11);
  }
  iVar11 = meth_0x417060(this,0x1b);
  if (iVar11 != 0) {
    FUN_004a90d0(iVar11);
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
    iVar11 = meth_0x417060(this,0x22);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11);
    }
    iVar11 = meth_0x417060(this,0x23);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11);
    }
    iVar11 = meth_0x417060(this,0x24);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11);
    }
    iVar11 = meth_0x417060(this,0x25);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11);
    }
    iVar11 = meth_0x417060(this,0x26);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11);
    }
    iVar11 = meth_0x417060(this,0x1c);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11);
    }
    pfVar18 = local_120;
    for (iVar11 = 0x14; iVar11 != 0; iVar11 = iVar11 + -1) {
      *pfVar18 = 0.0;
      pfVar18 = pfVar18 + 1;
    }
    local_120[0] = 1.121039e-43;
    local_120[1] = (float)local_34 * _DAT_005a3534;
    local_120[2] = (float)local_68 * _DAT_005a3534;
    local_120[3] = (float)local_74 * _DAT_005a3534;
    local_110 = local_100;
    local_d4 = 1;
    local_10c = local_120[1];
    local_108 = local_120[2];
    local_104 = local_120[3];
    iVar11 = (**(code **)(*DAT_005e8918 + 0xc))(DAT_005e8918,local_120);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11);
    }
    iVar11 = (**(code **)(*DAT_005e8918 + 0x14))(DAT_005e8918,DAT_00668f14,&DAT_005e8800);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11);
    }
    iVar11 = (**(code **)(*DAT_006699d0 + 0x20))(DAT_006699d0,DAT_005e8800);
    if (iVar11 != 0) {
      FUN_004a90d0(iVar11);
    }
  }
  if (DAT_005c61b8 == 0) {
    if ((-1 < DAT_005e8980) &&
       (puVar5 = *(undefined4 **)(DAT_005e8870 + DAT_005e8980 * 4), puVar5[0x15] != 0)) {
      if (DAT_005d7a28 == 0) {
        (**(code **)(*DAT_006699d0 + 0x38))(DAT_006699d0,*puVar5);
      }
      else {
        FUN_0056d120((int)(puVar5 + 1));
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
        FUN_004a90d0(iVar11);
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
        FUN_004a90d0(iVar11);
      }
    }
    else {
      FUN_0056d570((iVar11 << 8 | uVar19) << 8 | uVar13);
    }
    fVar8 = _DAT_005a36dc;
    if ((DAT_005c61b4 != 0) && (DAT_005d7a18 != 0)) {
      fVar8 = _DAT_005a3810;
    }
    local_160 = (float)_DAT_005c61bc * fVar8 * _DAT_005a350c;
    if ((-1 < DAT_005e8980) &&
       (puVar5 = *(undefined4 **)(DAT_005e8870 + DAT_005e8980 * 4), puVar5[0x15] == 0)) {
      if (DAT_005d7a28 == 0) {
        iVar11 = (**(code **)(*DAT_006699d0 + 0x34))(DAT_006699d0,*puVar5);
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11);
        }
      }
      else {
        FUN_0056cf00((int)(puVar5 + 1));
      }
      puVar5[0x15] = 1;
    }
    piVar2 = (int *)((float)local_74 * local_160 * _DAT_005a3534);
    if ((float)_DAT_005a34e4 <= (float)piVar2) {
      piVar2 = _DAT_005a34e4;
    }
    piVar7 = (int *)((float)local_68 * local_160 * _DAT_005a3534);
    if ((float)_DAT_005a34e4 <= (float)piVar7) {
      piVar7 = _DAT_005a34e4;
    }
    piVar9 = (int *)((float)local_34 * local_160 * _DAT_005a3534);
    if ((float)_DAT_005a34e4 <= (float)piVar9) {
      piVar9 = _DAT_005a34e4;
    }
    ppiVar6 = *(int ***)(DAT_005e8870 + DAT_005e8980 * 4);
    ppiVar6[3] = piVar9;
    ppiVar6[4] = piVar7;
    ppiVar6[5] = piVar2;
    ppiVar6[6] = (int *)0x3f800000;
    (**(code **)(**ppiVar6 + 0x10))(*ppiVar6,ppiVar6 + 1);
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
            if ((DAT_0066829c != 0) && ((*(short *)(piVar2 + 1) == 0xb && (piVar2 != DAT_00667fcc)))
               ) {
              this_00 = (cls_0x5a7e38 *)(**(code **)(*piVar2 + 0x24))();
              cls_0x5a7e38::meth_0x4da1c0(this_00);
            }
            if ((*(short *)(piVar2 + 1) == 0xb) || (*(short *)(piVar2 + 1) == 0xc)) {
              iVar17 = (**(code **)(*piVar2 + 0x24))();
              FUN_004da750(iVar17);
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
         (iVar11 = meth_0x417060(this,0x1c), iVar11 != 0)) {
        FUN_004a90d0(iVar11);
      }
      meth_0x412150((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0,0,
                    *(undefined4 *)(PTR_DAT_005d79e0 + 4));
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
        bVar20 = *(short *)(piVar2 + 1) == 0x19;
        if ((**(int **)(*(int *)(iVar11 + 4) + 0x54) != 1) &&
           (iVar17 = (**(code **)(*piVar2 + 0x134))(), iVar17 == 0)) {
          bVar20 = true;
        }
        if (((((bVar20 != (local_6c == 1)) || (piVar2[0x19] != 0)) ||
             ((piVar7 = *(int **)(*(int *)(iVar11 + 4) + 0x54), *piVar7 == 1 &&
              (((*(ushort *)(piVar2 + 0x17) == 0xffff ||
                ((uint)piVar7[1] <= (uint)*(ushort *)(piVar2 + 3))) ||
               (*(ushort *)((int)piVar7 + (uint)*(ushort *)(piVar2 + 3) * 0x4c + 0x32) <=
                *(ushort *)(piVar2 + 0x17))))))) ||
            (((piVar2[2] & 0x180U) != 0 && (DAT_00668154 == 0)))) ||
           (*(ushort *)((int)piVar2 + 0xe) != _DAT_00666970)) goto LAB_00413f8c;
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
        iVar11 = meth_0x417060(this,8);
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11);
        }
        iVar11 = meth_0x417060(this,0x1d);
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11);
        }
        iVar11 = meth_0x417060(this,4);
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11);
        }
        iVar11 = meth_0x417060(this,0x11);
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11);
        }
        iVar11 = meth_0x417060(this,0x12);
        if (iVar11 != 0) {
          FUN_004a90d0(iVar11);
        }
        meth_0x417d60(this,1);
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



// Function at 00414550

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall
OOAnalyzer::cls_0x411eb0::meth_0x414550
          (cls_0x411eb0 *this,int param_1,int param_2,int param_3,int *param_4,int param_5,
          undefined4 param_6,int param_7,int param_8,int param_9,undefined4 param_10,int param_11,
          int param_12,int param_13,int param_14,int param_15,int param_16,float *param_17)

{
  float fVar1;
  float fVar2;
  bool bVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  longlong lVar15;
  uint in_stack_00000048;
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
  float local_74;
  undefined4 local_70;
  undefined4 local_6c;
  float local_68;
  float local_64;
  float local_60;
  float local_5c;
  float local_58;
  float local_54;
  undefined4 local_50;
  undefined4 local_4c;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  undefined4 local_30;
  undefined4 local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  undefined4 local_10;
  undefined4 local_c;
  float local_8;
  float local_4;
  
  fVar1 = _DAT_005a34e4 / (float)param_11;
  local_6c = 0;
  local_74 = 1.0;
  local_4c = 0;
  local_54 = 1.0;
  local_2c = 0;
  fVar2 = _DAT_005a34e4 / (float)param_12;
  local_68 = (float)param_13 * fVar1 + DAT_005c61cc * fVar1;
  local_64 = (float)param_14 * fVar2 + _DAT_005c61d0 * fVar2;
  iVar14 = param_1 + param_8;
  local_70 = param_10;
  local_84 = (float)(param_13 + param_15) * fVar1 + DAT_005e91d8 * fVar1 + DAT_005c61cc * fVar1;
  local_50 = param_10;
  local_30 = param_10;
  local_24 = (float)(param_14 + param_16) * fVar2 + DAT_005e91dc * fVar2 + _DAT_005c61d0 * fVar2;
  local_78 = (float)param_3 * _DAT_005a3834;
  local_80 = (float)param_1;
  local_7c = (float)param_2;
  local_60 = (float)iVar14;
  iVar12 = param_2 + param_9;
  local_3c = (float)iVar12;
  local_10 = param_10;
  local_34 = 1.0;
  local_c = 0;
  local_14 = 1.0;
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
  if (param_17 != (float *)0x0) {
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
    DAT_005e8988 = (int *)0x0;
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 0x98))(DAT_00668f14,0,0);
    }
    else {
      FUN_0056d3a0(0,(int *)0x0);
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
      DAT_005e87e4._0_4_ = 0;
      DAT_005e898c._0_4_ = 0;
      (**(code **)(*DAT_00668f14 + 0x98))(DAT_00668f14,1,0);
    }
  }
  else if (DAT_005d7a28 == 0) {
    DAT_005e87e4._0_4_ = param_7;
    DAT_005e898c._0_4_ = param_6;
    (**(code **)(*DAT_00668f14 + 0x98))(DAT_00668f14,1,param_7);
  }
  uVar4 = DAT_005e91c0;
  DAT_005e91c0 = in_stack_00000048 >> 0x18 & 1;
  if ((in_stack_00000048 & 2) == 0) {
    if ((in_stack_00000048 & 4) == 0) {
      if ((in_stack_00000048 & 8) == 0) {
        if ((in_stack_00000048 & 0x10) == 0) {
          iVar5 = meth_0x417d60(this,1);
        }
        else {
          iVar5 = meth_0x417d60(this,0x10);
        }
      }
      else {
        iVar5 = meth_0x417d60(this,8);
      }
    }
    else {
      iVar5 = meth_0x417d60(this,4);
    }
  }
  else {
    iVar5 = meth_0x417d60(this,2);
  }
  if (iVar5 != 0) {
    FUN_004a90d0(iVar5);
  }
  if ((in_stack_00000048 & 0x40) == 0) {
    iVar5 = meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0xe);
  }
  else {
    iVar5 = meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0xe);
  }
  if (iVar5 != 0) {
    FUN_004a90d0(iVar5);
  }
  if ((in_stack_00000048 & 0x80) == 0) {
    iVar5 = meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),7);
  }
  else {
    iVar5 = meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),7);
  }
  if (iVar5 != 0) {
    FUN_004a90d0(iVar5);
  }
  if (((in_stack_00000048 & 0x200) != 0) &&
     (iVar5 = meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),4), iVar5 != 0)) {
    FUN_004a90d0(iVar5);
  }
  iVar5 = meth_0x417060((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),0x16);
  if (iVar5 != 0) {
    FUN_004a90d0(iVar5);
  }
  iVar5 = meth_0x417060(this,0x11);
  if (iVar5 != 0) {
    FUN_004a90d0(iVar5);
  }
  iVar5 = meth_0x417060(this,0x12);
  if (iVar5 != 0) {
    FUN_004a90d0(iVar5);
  }
  iVar5 = meth_0x417060(this,0x1a);
  if (iVar5 != 0) {
    FUN_004a90d0(iVar5);
  }
  iVar5 = FUN_004174b0(4,0x1c4,(int)&local_80,4,(int)&local_90,6,0);
  if (iVar5 != 0) {
    FUN_004a90d0(iVar5);
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
  if (param_17 != (float *)0x0) {
    lVar15 = __ftol();
    iVar5 = (int)lVar15;
    lVar15 = __ftol();
    iVar6 = (int)lVar15;
    iVar12 = iVar6;
    if (iVar5 <= iVar6) {
      iVar12 = iVar5;
    }
    lVar15 = __ftol();
    iVar7 = (int)lVar15;
    lVar15 = __ftol();
    iVar14 = (int)lVar15;
    iVar8 = iVar14;
    if (iVar7 <= iVar14) {
      iVar8 = iVar7;
    }
    if (iVar12 < iVar8) {
      param_1 = iVar6;
      if (iVar5 <= iVar6) {
        param_1 = iVar5;
      }
    }
    else {
      param_1 = iVar14;
      if (iVar7 <= iVar14) {
        param_1 = iVar7;
      }
    }
    lVar15 = __ftol();
    iVar12 = (int)lVar15;
    lVar15 = __ftol();
    iVar9 = (int)lVar15;
    iVar8 = iVar12;
    if (iVar9 < iVar12) {
      iVar8 = iVar9;
    }
    lVar15 = __ftol();
    iVar10 = (int)lVar15;
    lVar15 = __ftol();
    iVar11 = (int)lVar15;
    iVar13 = iVar11;
    if (iVar10 <= iVar11) {
      iVar13 = iVar10;
    }
    if (iVar8 < iVar13) {
      param_2 = iVar9;
      if (iVar12 <= iVar9) {
        param_2 = iVar12;
      }
    }
    else {
      param_2 = iVar11;
      if (iVar10 <= iVar11) {
        param_2 = iVar10;
      }
    }
    iVar8 = iVar6;
    if (iVar6 <= iVar5) {
      iVar8 = iVar5;
    }
    iVar13 = iVar14;
    if (iVar14 <= iVar7) {
      iVar13 = iVar7;
    }
    if (iVar13 < iVar8) {
      iVar14 = iVar5;
      if (iVar5 < iVar6) {
        iVar14 = iVar6;
      }
    }
    else if (iVar14 <= iVar7) {
      iVar14 = iVar7;
    }
    iVar5 = iVar9;
    if (iVar9 <= iVar12) {
      iVar5 = iVar12;
    }
    iVar6 = iVar11;
    if (iVar11 <= iVar10) {
      iVar6 = iVar10;
    }
    if (iVar6 < iVar5) {
      if (iVar12 < iVar9) {
        iVar12 = iVar9;
      }
    }
    else {
      iVar12 = iVar11;
      if (iVar11 <= iVar10) {
        iVar12 = iVar10;
      }
    }
  }
  if ((in_stack_00000048 & 0x400) == 0) {
    if ((in_stack_00000048 & 0x800) == 0) {
      DAT_005e91c0 = uVar4;
      return 1;
    }
    FUN_004aacb0_BlitEffect_Iterate(param_1,param_2,(param_1 - iVar14) + 1,(param_2 - iVar12) + 1);
    DAT_005e91c0 = uVar4;
    return 1;
  }
  FUN_004aacb0_BlitEffect_Iterate(param_1,param_2,(param_1 - iVar14) + 1,(param_2 - iVar12) + 1);
  DAT_005e91c0 = uVar4;
  return 1;
}



// Function at 00414d70

undefined4 __thiscall
OOAnalyzer::cls_0x411eb0::meth_0x414d70
          (cls_0x411eb0 *this,undefined param_1,undefined param_2,undefined4 param_3,int *param_4,
          int *param_5)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  uint uVar13;
  uint uVar14;
  int iVar15;
  undefined4 uVar16;
  int *piVar17;
  int *piVar18;
  uint uVar19;
  undefined3 in_stack_00000005;
  undefined3 in_stack_00000009;
  int in_stack_00000018;
  int in_stack_0000001c;
  undefined4 in_stack_00000020;
  int in_stack_00000024;
  int in_stack_00000028;
  int in_stack_0000002c;
  int in_stack_00000030;
  undefined4 in_stack_00000034;
  int iStack_58;
  int iStack_54;
  int iStack_48;
  int iStack_44;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  
  uVar16 = 0;
  if (param_4 != (int *)0x0) {
    if (param_5 == (int *)0x0) goto LAB_00414db1;
    iVar4 = (**(code **)(*param_4 + 8))();
    iVar5 = (**(code **)(*param_5 + 8))();
    if (iVar4 != iVar5) {
      return 0;
    }
  }
  if ((param_5 != (int *)0x0) && (param_4 == (int *)0x0)) {
    return 0;
  }
LAB_00414db1:
  if (((0 < in_stack_00000018) && (0 < in_stack_0000001c)) &&
     ((param_4 == (int *)0x0 || ((0 < in_stack_0000002c && (0 < in_stack_00000030)))))) {
    iStack_54 = 0;
    if (param_4 == (int *)0x0) {
      uVar10 = 0;
    }
    else {
      iVar4 = (**(code **)(*param_4 + 8))();
      if (iVar4 == 0x10) {
        if ((param_5 != (int *)0x0) && (param_4[0x1a] != param_5[0x1a])) {
          return 0;
        }
        iStack_48 = 0;
        if (param_4[0x1a] < 1) {
          return 0;
        }
        iStack_44 = 0;
        do {
          piVar18 = (int *)(iStack_44 + param_4[0x1b]);
          piVar1 = *(int **)(param_4[0x1c] + iStack_48 * 4);
          if (param_5 == (int *)0x0) {
            piVar17 = (int *)0x0;
          }
          else {
            piVar17 = *(int **)(param_5[0x1c] + iStack_48 * 4);
          }
          if (piVar1 == (int *)0x0) {
            uStack_24 = 0;
            uStack_28 = 0;
          }
          else {
            uStack_24 = (**(code **)(*piVar1 + 0xc))();
            uStack_28 = (**(code **)(*piVar1 + 0x10))();
          }
          uVar16 = 0;
          if (piVar17 == (int *)0x0) {
            uStack_2c = 0;
          }
          else {
            uStack_2c = (**(code **)(*piVar17 + 0xc))();
            uVar16 = (**(code **)(*piVar17 + 0x10))();
          }
          iVar4 = *piVar18;
          uVar13 = (uint)param_4[0x19] >> 0x1d & 2;
          if ((((iVar4 <= in_stack_00000024 + in_stack_0000002c + -1) &&
               (iVar5 = piVar18[2], in_stack_00000024 <= iVar5)) &&
              (iVar2 = piVar18[1], iVar2 <= in_stack_00000028 + in_stack_00000030 + -1)) &&
             (iVar3 = piVar18[3], in_stack_00000028 <= iVar3)) {
            uVar19 = (in_stack_00000024 - iVar4) - uVar13;
            uVar19 = ((int)uVar19 < 1) - 1 & uVar19;
            uVar14 = (in_stack_00000028 - iVar2) - uVar13;
            uVar14 = ((int)uVar14 < 1) - 1 & uVar14;
            iVar15 = (((iVar5 + uVar13 * -2) - iVar4) - uVar19) + 1;
            iStack_54 = (((in_stack_00000024 - iVar4) - uVar19) - uVar13) + in_stack_0000002c;
            if (iVar15 <= iStack_54) {
              iStack_54 = iVar15;
            }
            iVar15 = (((iVar3 + uVar13 * -2) - iVar2) - uVar14) + 1;
            iStack_58 = (((in_stack_00000028 - iVar2) - uVar14) - uVar13) + in_stack_00000030;
            if (iVar15 <= iStack_58) {
              iStack_58 = iVar15;
            }
            uVar6 = (iVar4 - in_stack_00000024) + uVar13;
            iVar15 = (int)((uVar6 & ((int)uVar6 < 1) - 1) * in_stack_00000018) / in_stack_0000002c;
            uVar6 = (iVar2 - in_stack_00000028) + uVar13;
            iVar7 = (int)((uVar6 & ((int)uVar6 < 1) - 1) * in_stack_0000001c) / in_stack_00000030;
            if ((int)((iVar5 - uVar13) + 1) < in_stack_00000024 + in_stack_0000002c) {
              iVar8 = (iStack_54 * in_stack_00000018) / in_stack_0000002c;
            }
            else {
              iVar8 = in_stack_00000018 - iVar15;
            }
            if ((int)((iVar3 - uVar13) + 1) < in_stack_00000028 + in_stack_00000030) {
              iVar9 = (iStack_58 * in_stack_0000001c) / in_stack_00000030;
            }
            else {
              iVar9 = in_stack_0000001c - iVar7;
            }
            iStack_54 = meth_0x414550(this,iVar15 + _param_1,iVar7 + _param_2,param_3,uStack_24,
                                      uStack_28,uStack_2c,uVar16,iVar8,iVar9,in_stack_00000020,
                                      (iVar5 - iVar4) + 1,(iVar3 - iVar2) + 1,uVar19,uVar14,
                                      iStack_54,iStack_58,in_stack_00000034);
          }
          iStack_48 = iStack_48 + 1;
          iStack_44 = iStack_44 + 0x10;
        } while (iStack_48 < param_4[0x1a]);
        return iStack_54;
      }
      uVar10 = (**(code **)(*param_4 + 0xc))();
      uVar16 = (**(code **)(*param_4 + 0x10))();
    }
    if (param_5 == (int *)0x0) {
      uVar11 = 0;
      uVar12 = 0;
    }
    else {
      uVar11 = (**(code **)(*param_4 + 0xc))();
      uVar12 = (**(code **)(*param_4 + 0x10))();
    }
    uVar16 = meth_0x414550(this,_param_1,_param_2,param_3,uVar10,uVar16,uVar11,uVar12,
                           in_stack_00000018,in_stack_0000001c,in_stack_00000020,param_4[1],
                           param_4[2],in_stack_00000024,in_stack_00000028,in_stack_0000002c,
                           in_stack_00000030,in_stack_00000034);
    return uVar16;
  }
  return 0;
}



// Function at 00415790

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall
OOAnalyzer::cls_0x411eb0::meth_0x415790
          (cls_0x411eb0 *this,undefined param_1,uint param_2,int *param_3)

{
  int **ppiVar1;
  undefined4 uVar2;
  int iVar3;
  int **ppiVar4;
  int **ppiVar5;
  undefined3 in_stack_00000005;
  int *in_stack_00000010;
  int *local_5c [2];
  float local_54;
  float local_50;
  float local_4c;
  undefined4 local_48;
  float local_44;
  float local_40;
  float local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  float local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059c800;
  local_c = ExceptionList;
  if (this->mbr_0x0 != 0) {
    ppiVar1 = local_5c;
    for (iVar3 = 0x14; iVar3 != 0; iVar3 = iVar3 + -1) {
      *ppiVar1 = (int *)0x0;
      ppiVar1 = ppiVar1 + 1;
    }
    local_54 = (float)(param_2 >> 0x10 & 0xff) * _DAT_005a3534;
    local_5c[0] = (int *)0x50;
    local_5c[1] = (int *)0x1;
    local_48 = 0x3f800000;
    local_38 = 0;
    local_34 = 0;
    local_30 = 0;
    local_28 = 0x3d8f5c29;
    local_24 = 0x3dcccccd;
    local_20 = 0x3f4ccccd;
    local_50 = (float)(param_2 >> 8 & 0xff) * _DAT_005a3534;
    local_1c = 0x3f800000;
    local_18 = 0;
    local_14 = 0;
    local_10 = 1;
    local_4c = (float)(param_2 & 0xff) * _DAT_005a3534;
    local_44 = (float)*_param_1;
    local_40 = (float)_param_1[1];
    local_3c = (float)_param_1[2];
    local_2c = (float)(int)param_3 * (float)_DAT_005a3838;
    ExceptionList = &local_c;
    ppiVar1 = (int **)FUN_00482fb0(0x60);
    local_4 = 0;
    if (ppiVar1 != (int **)0x0) {
      ppiVar1[0x17] = in_stack_00000010;
      ppiVar4 = local_5c;
      ppiVar5 = ppiVar1;
      for (iVar3 = 0x14; ppiVar5 = ppiVar5 + 1, iVar3 != 0; iVar3 = iVar3 + -1) {
        *ppiVar5 = *ppiVar4;
        ppiVar4 = ppiVar4 + 1;
      }
      ppiVar1[0x16] = param_3;
      FUN_004118e0(ppiVar1);
    }
    local_4 = 0xffffffff;
    uVar2 = cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&DAT_005e8860);
    ExceptionList = local_c;
    return uVar2;
  }
  return 0xffffffff;
}



// Function at 00415920

undefined4 __thiscall OOAnalyzer::cls_0x411eb0::meth_0x415920(cls_0x411eb0 *this)

{
  int **ppiVar1;
  int *piVar2;
  int in_stack_00000004;
  
  if (((this->mbr_0x0 != 0) && (-1 < in_stack_00000004)) && (in_stack_00000004 < DAT_005e8860)) {
    ppiVar1 = *(int ***)(DAT_005e8870 + in_stack_00000004 * 4);
    if ((ppiVar1 != (int **)0x0) && (ppiVar1 != (int **)0x0)) {
      if (ppiVar1[0x15] != (int *)0x0) {
        if (DAT_005d7a28 == 0) {
          (**(code **)(*DAT_006699d0 + 0x38))(DAT_006699d0,*ppiVar1);
        }
        else {
          FUN_0056d120((int)(ppiVar1 + 1));
        }
        ppiVar1[0x15] = (int *)0x0;
      }
      piVar2 = *ppiVar1;
      if (piVar2 != (int *)0x0) {
        (**(code **)(*piVar2 + 8))(piVar2);
        *ppiVar1 = (int *)0x0;
      }
      FUN_004830f0(ppiVar1);
    }
    cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&DAT_005e8860);
    return 1;
  }
  return 0;
}



// Function at 00415b40

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

bool __thiscall
OOAnalyzer::cls_0x411eb0::meth_0x415b40
          (cls_0x411eb0 *this,int param_1,int param_2,int param_3,int *param_4,int *param_5)

{
  int iVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  int iVar6;
  int *piVar7;
  float fVar8;
  int iVar9;
  int *piVar10;
  int *in_stack_00000018;
  float local_4;
  
  piVar7 = param_5;
  iVar6 = DAT_005e8860;
  fVar3 = _DAT_005a3840;
  if (this->mbr_0x0 != 0) {
    iVar9 = 0;
    local_4 = 10000.0;
    *param_4 = -1;
    *param_5 = -1;
    param_5 = (int *)0x461c4000;
    *in_stack_00000018 = -1;
    piVar10 = DAT_005e8870;
    if (0 < iVar6) {
      do {
        fVar8 = local_4;
        if (*piVar10 != 0) {
          iVar1 = *piVar10;
          fVar4 = (float)param_1 - *(float *)(iVar1 + 0x1c);
          fVar5 = (float)param_2 - *(float *)(iVar1 + 0x20);
          fVar2 = (float)param_3 - *(float *)(iVar1 + 0x24);
          fVar4 = SQRT(fVar5 * fVar5 + fVar4 * fVar4);
          fVar2 = SQRT(fVar2 * fVar2 + fVar4 * fVar4) / ((float)_DAT_005c61c4 * _DAT_005a350c);
          if (local_4 <= fVar2) {
            if (fVar3 <= fVar2) {
              if (fVar2 < (float)param_5) {
                *in_stack_00000018 = iVar9;
                param_5 = (int *)fVar2;
              }
            }
            else {
              iVar1 = *piVar7;
              *piVar7 = iVar9;
              *in_stack_00000018 = iVar1;
              param_5 = (int *)fVar3;
              fVar3 = fVar2;
            }
          }
          else {
            *in_stack_00000018 = *piVar7;
            *piVar7 = *param_4;
            *param_4 = iVar9;
            param_5 = (int *)fVar3;
            fVar8 = fVar2;
            fVar3 = local_4;
          }
        }
        local_4 = fVar8;
        iVar9 = iVar9 + 1;
        piVar10 = piVar10 + 1;
      } while (iVar9 < iVar6);
    }
    return -1 < *param_4;
  }
  return false;
}



// Function at 00415c70

undefined4 __thiscall
OOAnalyzer::cls_0x411eb0::meth_0x415c70
          (cls_0x411eb0 *this,int param_1,int param_2,int param_3,int param_4,undefined4 *param_5,
          undefined4 *param_6)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  undefined4 *in_stack_0000001c;
  int local_c [3];
  
  if (this->mbr_0x0 == 0) {
    return 0;
  }
  if ((DAT_005c61b8 != 0) &&
     (puVar1 = *(undefined4 **)(DAT_005e8870 + DAT_005e8980 * 4), puVar1[0x15] == 0)) {
    if (DAT_005d7a28 == 0) {
      iVar2 = (**(code **)(*DAT_006699d0 + 0x34))(DAT_006699d0,*puVar1);
      if (iVar2 != 0) {
        FUN_004a90d0(iVar2);
      }
    }
    else {
      FUN_0056cf00((int)(puVar1 + 1));
    }
    puVar1[0x15] = 1;
  }
  if ((param_1 != 0) && (DAT_005d7a18 != 0)) {
    meth_0x415b40(this,param_2,param_3,param_4,local_c,local_c + 1);
    iVar2 = DAT_005e8870;
    iVar5 = 0;
    piVar4 = local_c;
    *param_5 = *(undefined4 *)(DAT_005e8870 + local_c[0] * 4);
    *param_6 = *(undefined4 *)(iVar2 + local_c[1] * 4);
    *in_stack_0000001c = *(undefined4 *)(iVar2 + local_c[2] * 4);
    while( true ) {
      iVar3 = param_1;
      if (2 < param_1) {
        iVar3 = 3;
      }
      if (iVar3 <= iVar5) break;
      if (-1 < *piVar4) {
        FUN_00411cb0(*(int ***)(iVar2 + *piVar4 * 4),(float)param_2,(float)param_3,(float)param_4);
        iVar2 = DAT_005e8870;
      }
      iVar5 = iVar5 + 1;
      piVar4 = piVar4 + 1;
    }
  }
  return 1;
}



// Function at 00415db0

void __thiscall OOAnalyzer::cls_0x411eb0::meth_0x415db0(cls_0x411eb0 *this,int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  int in_stack_00000008;
  
  if ((((this->mbr_0x0 != 0) && (-1 < param_1)) && (param_1 < DAT_005e8860)) &&
     (*(int *)(DAT_005e8870 + param_1 * 4) != 0)) {
    puVar1 = *(undefined4 **)(DAT_005e8870 + param_1 * 4);
    if (in_stack_00000008 == 0) {
      if (puVar1[0x15] != 0) {
        if (DAT_005d7a28 != 0) {
          FUN_0056d120((int)(puVar1 + 1));
          puVar1[0x15] = 0;
          return;
        }
        (**(code **)(*DAT_006699d0 + 0x38))(DAT_006699d0,*puVar1);
        puVar1[0x15] = 0;
      }
    }
    else if (puVar1[0x15] == 0) {
      if (DAT_005d7a28 != 0) {
        FUN_0056cf00((int)(puVar1 + 1));
        puVar1[0x15] = 1;
        return;
      }
      iVar2 = (**(code **)(*DAT_006699d0 + 0x34))(DAT_006699d0,*puVar1);
      if (iVar2 != 0) {
        FUN_004a90d0(iVar2);
      }
      puVar1[0x15] = 1;
      return;
    }
  }
  return;
}



// Function at 00415f40

void __thiscall
OOAnalyzer::cls_0x411eb0::meth_0x415f40
          (cls_0x411eb0 *this,int param_1,int param_2,int param_3,int param_4)

{
  dword dVar1;
  int iVar2;
  float10 fVar3;
  float *in_stack_00000014;
  
  dVar1 = this->mbr_0x0;
  *in_stack_00000014 = 0.0;
  if (((dVar1 != 0) && (-1 < param_1)) && (param_1 < DAT_005e8860)) {
    iVar2 = *(int *)(DAT_005e8870 + param_1 * 4);
    if (iVar2 != 0) {
      fVar3 = FUN_00411b70(iVar2,(float)param_2,(float)param_3,(float)param_4);
      *in_stack_00000014 = (float)fVar3;
    }
  }
  return;
}



// Function at 004160c0

void __thiscall
OOAnalyzer::cls_0x411eb0::meth_0x4160c0
          (cls_0x411eb0 *this,int param_1,int param_2,int param_3,int param_4,float *param_5,
          float *param_6)

{
  dword dVar1;
  int iVar2;
  float10 fVar3;
  float *in_stack_0000001c;
  
  dVar1 = this->mbr_0x0;
  *param_6 = 0.0;
  *in_stack_0000001c = 0.0;
  *param_5 = 0.0;
  if (((dVar1 != 0) && (-1 < param_1)) && (param_1 < DAT_005e8860)) {
    iVar2 = *(int *)(DAT_005e8870 + param_1 * 4);
    if (iVar2 != 0) {
      fVar3 = FUN_00411b70(iVar2,(float)param_2,(float)param_3,(float)param_4);
      *param_5 = (float)(fVar3 * (float10)*(float *)(iVar2 + 0xc));
      *param_6 = (float)(fVar3 * (float10)*(float *)(iVar2 + 0x10));
      *in_stack_0000001c = (float)(fVar3 * (float10)*(float *)(iVar2 + 0x14));
    }
  }
  return;
}



// Function at 00416150

void __thiscall OOAnalyzer::cls_0x411eb0::meth_0x416150(cls_0x411eb0 *this)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  
  if ((this->mbr_0x0 != 0) && (iVar3 = 0, iVar2 = DAT_005e8870, 0 < DAT_005e8860)) {
    do {
      puVar1 = *(undefined4 **)(iVar2 + iVar3 * 4);
      if ((puVar1 != (undefined4 *)0x0) && (puVar1[0x15] != 0)) {
        if (DAT_005d7a28 == 0) {
          (**(code **)(*DAT_006699d0 + 0x38))(DAT_006699d0,*puVar1);
        }
        else {
          FUN_0056d120((int)(puVar1 + 1));
        }
        iVar2 = DAT_005e8870;
        puVar1[0x15] = 0;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < DAT_005e8860);
  }
  return;
}



// Function at 004162c0

undefined4 __thiscall OOAnalyzer::cls_0x411eb0::meth_0x4162c0(cls_0x411eb0 *this)

{
  uint in_stack_00000004;
  
  if (((this->mbr_0x0 != 0) && (in_stack_00000004 < DAT_005e8890)) &&
     (*(int *)(DAT_005e88a0 + in_stack_00000004 * 4) != 0)) {
    cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&DAT_005e8890);
    return 1;
  }
  return 0;
}



// Function at 00417060

undefined4 __thiscall OOAnalyzer::cls_0x411eb0::meth_0x417060(cls_0x411eb0 *this,int param_1)

{
  undefined4 uVar1;
  int in_stack_00000008;
  
  DAT_005e8790 = 0;
  if (DAT_005d7a28 != 0) {
    uVar1 = FUN_0056d400(param_1,in_stack_00000008);
    return uVar1;
  }
  if (param_1 != 0x15) {
    uVar1 = (**(code **)(*DAT_00668f14 + 0x58))(DAT_00668f14,param_1);
    return uVar1;
  }
  DAT_005e8938 = in_stack_00000008;
  switch(in_stack_00000008) {
  case 1:
  case 7:
    (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,0,2,2);
    FUN_00417390(0,3,0);
    FUN_00417390(0,1,2);
    FUN_00417390(0,5,2);
    FUN_00417390(0,6,0);
    FUN_00417390(0,4,2);
    return 0;
  default:
    if (DAT_005e91c0 != 0) {
      if (DAT_006697c0 != 0) {
        uVar1 = 6;
        goto LAB_004171d0;
      }
      uVar1 = 5;
      if (DAT_006697bc != 0) goto LAB_004171d0;
    }
    uVar1 = 4;
LAB_004171d0:
    (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,0,2,2);
    FUN_00417390(0,3,0);
    FUN_00417390(0,1,uVar1);
    FUN_00417390(0,5,2);
    FUN_00417390(0,6,0);
    FUN_00417390(0,4,4);
    return 0;
  case 3:
    (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,0,2,2);
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,0,3,0);
    }
    uVar1 = 0xd;
    break;
  case 8:
    (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,0,2,2);
    FUN_00417390(0,3,0);
    uVar1 = 7;
  }
  FUN_00417390(0,1,uVar1);
  FUN_00417390(0,5,2);
  FUN_00417390(0,6,0);
  FUN_00417390(0,4,3);
  return 0;
}



// Function at 004178e0

undefined4 __thiscall OOAnalyzer::cls_0x411eb0::meth_0x4178e0(cls_0x411eb0 *this)

{
  DAT_005e88c0 = DAT_005e88b4;
  if (DAT_005d7a28 == 0) {
    (**(code **)(*DAT_00668f14 + 0x54))(DAT_00668f14,0xe,&DAT_005e8874);
  }
  else {
    FUN_0056d4b0(0xe,&DAT_005e8874);
  }
  if (DAT_005d7a28 == 0) {
    (**(code **)(*DAT_00668f14 + 0x54))(DAT_00668f14,7,&DAT_005e884c);
  }
  else {
    FUN_0056d4b0(7,&DAT_005e884c);
  }
  if (DAT_005d7a28 == 0) {
    (**(code **)(*DAT_00668f14 + 0x54))(DAT_00668f14,0x16,&DAT_005e87d8);
  }
  else {
    FUN_0056d4b0(0x16,&DAT_005e87d8);
  }
  if (DAT_005d7a28 == 0) {
    (**(code **)(*DAT_00668f14 + 0x54))(DAT_00668f14,0x13,&DAT_005e88bc);
  }
  else {
    FUN_0056d4b0(0x13,&DAT_005e88bc);
  }
  if (DAT_005d7a28 == 0) {
    (**(code **)(*DAT_00668f14 + 0x54))(DAT_00668f14,0x14,&DAT_005e885c);
  }
  else {
    FUN_0056d4b0(0x14,&DAT_005e885c);
  }
  if (DAT_005d7a28 != 0) {
    FUN_0056d4b0(0x15,&DAT_005e8908);
    return 0;
  }
  (**(code **)(*DAT_00668f14 + 0x9c))(DAT_00668f14,0,2,&DAT_005e873c);
  if (DAT_005d7a28 == 0) {
    (**(code **)(*DAT_00668f14 + 0x9c))(DAT_00668f14,0,3,&DAT_005e8738);
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 0x9c))(DAT_00668f14,0,1,&DAT_005e88a8);
    }
  }
  FUN_004173c0(0,5,&DAT_005e8854);
  FUN_004173c0(0,6,&DAT_005e8858);
  FUN_004173c0(0,4,&DAT_005e893c);
  if (DAT_006695a8 != 0) {
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 0x9c))(DAT_00668f14,1,2,&DAT_005e8910);
    }
    FUN_004173c0(1,3,&DAT_005e890c);
    FUN_004173c0(1,1,&DAT_005e888c);
    FUN_004173c0(1,5,&DAT_005e88b0);
    FUN_004173c0(1,6,&DAT_005e88ac);
    FUN_004173c0(1,4,&DAT_005e8888);
  }
  return 0;
}



// Function at 00417b00

undefined4 __thiscall OOAnalyzer::cls_0x411eb0::meth_0x417b00(cls_0x411eb0 *this)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 unaff_EDI;
  int *piVar5;
  
  DAT_005e88b4 = DAT_005e88c0;
  meth_0x417060(this,0xe);
  meth_0x417060(this,7);
  meth_0x417060(this,0x16);
  meth_0x417060(this,0x13);
  meth_0x417060(this,0x14);
  uVar4 = DAT_005e88a8;
  uVar3 = DAT_005e8858;
  uVar2 = DAT_005e8854;
  uVar1 = DAT_005e8738;
  if (DAT_005d7a28 == 0) {
    (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,0,2,DAT_005e873c);
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,0,3,uVar1);
      if (DAT_005d7a28 == 0) {
        (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,0,1,uVar4);
        if (DAT_005d7a28 == 0) {
          (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,0,5,uVar2);
          if (DAT_005d7a28 == 0) {
            (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,0,6,uVar3);
            if (DAT_005d7a28 == 0) {
              (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,0,4,unaff_EDI);
            }
          }
        }
      }
    }
    uVar4 = DAT_005e890c;
    uVar3 = DAT_005e88b0;
    uVar2 = DAT_005e88ac;
    uVar1 = DAT_005e888c;
    if ((DAT_006695a8 != 0) && (DAT_005d7a28 == 0)) {
      (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,1,2,DAT_005e8910);
      if (DAT_005d7a28 == 0) {
        (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,1,3,uVar4);
        if (DAT_005d7a28 == 0) {
          (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,1,1,uVar1);
          if (DAT_005d7a28 == 0) {
            piVar5 = DAT_00668f14;
            (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,1,5,uVar3);
            if (DAT_005d7a28 == 0) {
              (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,1,6,uVar2);
              if (DAT_005d7a28 == 0) {
                (**(code **)(*DAT_00668f14 + 0xa0))(DAT_00668f14,1,4,piVar5);
              }
            }
          }
        }
      }
    }
  }
  else {
    meth_0x417060(this,0x15);
  }
  DAT_005e8790 = DAT_005e88b4;
  DAT_005e8740 = DAT_005e91c0;
  return 0;
}



// Function at 00417d60

undefined4 __thiscall OOAnalyzer::cls_0x411eb0::meth_0x417d60(cls_0x411eb0 *this,uint param_1)

{
  uint uVar1;
  undefined4 uVar2;
  int in_stack_00000008;
  
  if ((param_1 == 0) || ((param_1 == DAT_005e8790 && (DAT_005e91c0 == DAT_005e8740)))) {
    return 0;
  }
  DAT_005e88b4 = param_1;
  uVar1 = param_1;
  if (DAT_0066818c != 0) {
    if ((param_1 & 4) == 0) {
      if ((param_1 & 0x10) == 0) {
        if ((param_1 & 0x20) == 0) goto LAB_00417dbc;
        uVar1 = param_1 & 0xffffffdf;
      }
      else {
        uVar1 = param_1 & 0xffffffef;
      }
      uVar1 = uVar1 | 8;
    }
    else {
      uVar1 = param_1 & 0xfffffffb | 2;
    }
  }
LAB_00417dbc:
  if ((uVar1 & 1) == 0) {
    if ((uVar1 & 2) == 0) {
      if ((uVar1 & 4) == 0) {
        if ((uVar1 & 8) == 0) {
          if ((uVar1 & 0x10) == 0) {
            if ((uVar1 & 0x20) == 0) {
              DAT_005e8740 = DAT_005e91c0;
              DAT_005e8790 = param_1;
              return 0;
            }
            meth_0x417060(this,0xe);
            meth_0x417060(this,7);
            meth_0x417060(this,0x16);
            meth_0x417060(this,0x13);
            meth_0x417060(this,0x14);
            if (DAT_005d7a28 != 0) {
              meth_0x417060(this,0x15);
              DAT_005e8740 = DAT_005e91c0;
              DAT_005e8790 = DAT_005e88b4;
              return 0;
            }
            FUN_00417390(0,2,2);
            FUN_00417390(0,3,0);
            FUN_00417390(0,1,2);
            FUN_00417390(0,5,2);
            FUN_00417390(0,6,0);
            FUN_00417390(0,4,2);
            if (in_stack_00000008 < 2) goto LAB_00418508;
            if (DAT_005e91c0 != 0) {
              if (DAT_006697c0 != 0) {
                uVar2 = 6;
                goto joined_r0x004184bb;
              }
              uVar2 = 5;
              if (DAT_006697bc != 0) goto joined_r0x004184bb;
            }
            uVar2 = 4;
            goto joined_r0x004184bb;
          }
          meth_0x417060(this,0xe);
          meth_0x417060(this,7);
          meth_0x417060(this,0x16);
          meth_0x417060(this,0x13);
          meth_0x417060(this,0x14);
          if (DAT_005d7a28 != 0) {
            meth_0x417060(this,0x15);
            DAT_005e8740 = DAT_005e91c0;
            DAT_005e8790 = DAT_005e88b4;
            return 0;
          }
          if (DAT_005e91c0 == 0) {
LAB_00418320:
            uVar2 = 4;
          }
          else if (DAT_006697c0 == 0) {
            uVar2 = 5;
            if (DAT_006697bc == 0) goto LAB_00418320;
          }
          else {
            uVar2 = 6;
          }
          FUN_00417390(0,2,2);
          FUN_00417390(0,3,0);
          FUN_00417390(0,1,uVar2);
          FUN_00417390(0,5,2);
          FUN_00417390(0,6,0);
          FUN_00417390(0,4,2);
          if (in_stack_00000008 < 2) goto LAB_00418508;
          if (DAT_005e91c0 != 0) {
            if (DAT_006697c0 != 0) {
              uVar2 = 6;
              goto joined_r0x004184bb;
            }
            goto joined_r0x004183a0;
          }
        }
        else {
          meth_0x417060(this,0xe);
          meth_0x417060(this,7);
          meth_0x417060(this,0x16);
          meth_0x417060(this,0x13);
          meth_0x417060(this,0x14);
          if (DAT_005d7a28 != 0) {
            meth_0x417060(this,0x15);
            DAT_005e8740 = DAT_005e91c0;
            DAT_005e8790 = DAT_005e88b4;
            return 0;
          }
          FUN_00417390(0,2,2);
          FUN_00417390(0,3,0);
          FUN_00417390(0,1,2);
          FUN_00417390(0,5,2);
          FUN_00417390(0,6,0);
          FUN_00417390(0,4,2);
          if (in_stack_00000008 < 2) goto LAB_00418508;
          if (DAT_005e91c0 != 0) {
            if (DAT_006697c0 != 0) {
              uVar2 = 6;
              goto joined_r0x004184bb;
            }
            goto joined_r0x004183a0;
          }
        }
      }
      else {
        meth_0x417060(this,0xe);
        meth_0x417060(this,7);
        meth_0x417060(this,0x16);
        meth_0x417060(this,0x13);
        meth_0x417060(this,0x14);
        if (DAT_005d7a28 != 0) {
          meth_0x417060(this,0x15);
          DAT_005e8740 = DAT_005e91c0;
          DAT_005e8790 = DAT_005e88b4;
          return 0;
        }
        if (DAT_005e91c0 == 0) {
LAB_004180f2:
          uVar2 = 4;
        }
        else if (DAT_006697c0 == 0) {
          uVar2 = 5;
          if (DAT_006697bc == 0) goto LAB_004180f2;
        }
        else {
          uVar2 = 6;
        }
        FUN_00417390(0,2,2);
        FUN_00417390(0,3,0);
        FUN_00417390(0,1,uVar2);
        FUN_00417390(0,5,2);
        FUN_00417390(0,6,0);
        FUN_00417390(0,4,4);
        if (in_stack_00000008 < 2) goto LAB_00418508;
        if (DAT_005e91c0 != 0) {
          if (DAT_006697c0 != 0) {
            uVar2 = 6;
            goto joined_r0x004184bb;
          }
joined_r0x004183a0:
          uVar2 = 5;
          if (DAT_006697bc != 0) goto joined_r0x004184bb;
        }
      }
      uVar2 = 4;
joined_r0x004184bb:
      if (DAT_006695a8 == 0) {
        DAT_005e8740 = DAT_005e91c0;
        DAT_005e8790 = DAT_005e88b4;
        return 0;
      }
      FUN_00417390(1,2,2);
      FUN_00417390(1,3,1);
      FUN_00417390(1,1,uVar2);
      FUN_00417390(1,5,2);
      FUN_00417390(1,6,1);
      FUN_00417390(1,4,4);
      DAT_005e8740 = DAT_005e91c0;
      DAT_005e8790 = DAT_005e88b4;
      return 0;
    }
    meth_0x417060(this,0xe);
    meth_0x417060(this,7);
    meth_0x417060(this,0x16);
    meth_0x417060(this,0x13);
    meth_0x417060(this,0x14);
    if (DAT_005d7a28 != 0) {
      meth_0x417060(this,0x15);
      DAT_005e8740 = DAT_005e91c0;
      DAT_005e8790 = DAT_005e88b4;
      return 0;
    }
    FUN_00417390(0,2,2);
    FUN_00417390(0,3,0);
    FUN_00417390(0,1,2);
    FUN_00417390(0,5,2);
    FUN_00417390(0,6,0);
    FUN_00417390(0,4,2);
  }
  else {
    meth_0x417060(this,0xe);
    meth_0x417060(this,7);
    meth_0x417060(this,0x16);
    meth_0x417060(this,0x13);
    meth_0x417060(this,0x14);
    if (DAT_005d7a28 != 0) {
      meth_0x417060(this,0x15);
      DAT_005e8740 = DAT_005e91c0;
      DAT_005e8790 = DAT_005e88b4;
      return 0;
    }
    if (DAT_005e91c0 == 0) {
LAB_00417e5a:
      uVar2 = 4;
    }
    else if (DAT_006697c0 == 0) {
      uVar2 = 5;
      if (DAT_006697bc == 0) goto LAB_00417e5a;
    }
    else {
      uVar2 = 6;
    }
    FUN_00417390(0,2,2);
    FUN_00417390(0,3,0);
    FUN_00417390(0,1,uVar2);
    FUN_00417390(0,5,2);
    FUN_00417390(0,6,0);
    FUN_00417390(0,4,4);
  }
  if (in_stack_00000008 < 2) {
    if (DAT_006695a8 == 0) {
      DAT_005e8740 = DAT_005e91c0;
      DAT_005e8790 = DAT_005e88b4;
      return 0;
    }
    FUN_00417390(1,2,2);
    FUN_00417390(1,3,1);
    FUN_00417390(1,1,1);
    FUN_00417390(1,5,2);
    FUN_00417390(1,6,1);
    FUN_00417390(1,4,1);
    DAT_005e8740 = DAT_005e91c0;
    DAT_005e8790 = DAT_005e88b4;
    return 0;
  }
LAB_00418508:
  FUN_00417720(1,2);
  DAT_005e8740 = DAT_005e91c0;
  DAT_005e8790 = DAT_005e88b4;
  return 0;
}



