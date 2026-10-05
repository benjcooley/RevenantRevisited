// FUN_00533f10 @ 00533f10 size=1228

undefined4 * __thiscall
FUN_00533f10(undefined4 *param_1,int param_2,undefined4 *param_3,int param_4,undefined4 param_5,
            undefined4 param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9,
            undefined4 param_10,int param_11,undefined4 *param_12,undefined4 *param_13,
            undefined4 param_14)

{
  char cVar1;
  char *pcVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  undefined4 *puVar8;
  uint uVar9;
  int iVar10;
  bool bVar11;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1a34;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  *param_1 = param_2;
  param_1[2] = param_4;
  param_1[1] = *(undefined4 *)((int)param_3 + 0x40);
  param_1[3] = param_9;
  param_1[6] = param_5;
  param_1[4] = param_10;
  param_1[8] = param_7;
  param_1[0xc] = param_7;
  param_1[9] = param_8;
  param_1[0xd] = param_8;
  param_1[5] = param_14;
  param_1[7] = param_6;
  param_1[10] = 400;
  param_1[0xb] = 0;
  param_1[0x11] = 0;
  param_1[0x10] = 0;
  param_1[0xf] = 0;
  param_1[0xe] = 0;
  param_1[0x14] = 0;
  if (7 < param_11) {
    param_11 = 8;
  }
  param_1[0x17] = param_11;
  param_4 = 0;
  param_2 = 0;
  if (0 < param_11) {
    param_3 = param_12;
    puVar8 = param_1 + 0x18;
    piVar7 = param_1 + 0x21;
    do {
      if (param_1[2] == 3) {
        pcVar2 = (char *)FUN_0049d800(*param_3);
        uVar9 = 0xffffffff;
        do {
          if (uVar9 == 0) break;
          uVar9 = uVar9 - 1;
          cVar1 = *pcVar2;
          pcVar2 = pcVar2 + 1;
        } while (cVar1 != '\0');
        uVar3 = FUN_00482fb0(~uVar9 + 2);
        uVar4 = FUN_0049d800(*param_3);
        FUN_0058b100(uVar3,&DAT_005e3f44,uVar4);
        uVar4 = FUN_0059b6bc(uVar3);
        *puVar8 = uVar4;
        FUN_004830f0(uVar3);
      }
      else {
        uVar3 = FUN_0059b6bc(*param_3);
        *puVar8 = uVar3;
      }
      iVar5 = DAT_0065c134;
      iVar10 = DAT_0065b024;
      iVar6 = DAT_0065b020;
      piVar7[-1] = 0x32;
      *piVar7 = param_4;
      iVar6 = *(int *)(iVar6 + iVar5 * 4);
      piVar7[1] = 399;
      if (iVar6 == 0) {
        iVar6 = iVar10;
      }
      iVar10 = *(int *)(iVar6 + 0x54);
      iVar6 = *(int *)(iVar6 + 0x50);
      iVar5 = FUN_004acb80(iVar5,*puVar8,0x15e,0x401,0,10000,0);
      puVar8 = puVar8 + 1;
      param_4 = *piVar7 + -1 + iVar5 * (iVar10 + iVar6);
      piVar7[2] = param_4;
      param_1[0xb] = param_4 + 1;
      param_4 = param_4 + 0xb;
      param_3 = param_3 + 1;
      param_2 = param_2 + 1;
      piVar7 = piVar7 + 4;
    } while (param_2 < (int)param_1[0x17]);
  }
  iVar6 = param_1[0xb];
  if (iVar6 < 0x2c) {
    param_1[0xb] = 0x2c;
    iVar10 = 0;
    iVar6 = (0x2c - iVar6) / 2;
    if (0 < (int)param_1[0x17]) {
      piVar7 = param_1 + 0x23;
      do {
        piVar7[-2] = piVar7[-2] + iVar6;
        iVar10 = iVar10 + 1;
        *piVar7 = *piVar7 + iVar6;
        piVar7 = piVar7 + 4;
      } while (iVar10 < (int)param_1[0x17]);
    }
  }
  param_2 = 0;
  if (0 < (int)param_1[0x17]) {
    puVar8 = param_1 + 0x50;
    param_3 = param_13;
    piVar7 = param_1 + 0x21;
    do {
      if (param_1[2] == 3) {
        iVar6 = FUN_00482fb0(0x148);
        local_4 = 0;
        if (iVar6 == 0) {
          uVar3 = 0;
        }
        else {
          uVar3 = FUN_0042c600(*param_3,param_1[8] + param_1[6] + piVar7[-1],
                               param_1[9] + *piVar7 + param_1[7],(piVar7[1] - piVar7[-1]) + 1,
                               (piVar7[2] - *piVar7) + 1,0,0,0,0,0xffffffff,0x100010,0xffffffff,0);
        }
        local_4 = 0xffffffff;
        uVar3 = FUN_00436790(uVar3);
        uVar3 = FUN_00436900(uVar3);
        puVar8[-0x10] = uVar3;
        puVar8[-8] = 0;
        *puVar8 = 0;
      }
      else {
        puVar8[-0x10] = 0;
        *puVar8 = 0;
        puVar8[-8] = 0;
      }
      param_3 = param_3 + 1;
      param_2 = param_2 + 1;
      piVar7 = piVar7 + 4;
      puVar8 = puVar8 + 1;
    } while (param_2 < (int)param_1[0x17]);
  }
  if (DAT_006680c8 == 0) {
    puVar8 = (undefined4 *)FUN_00482fb0(0x74);
    local_4 = 5;
    if (puVar8 == (undefined4 *)0x0) {
      puVar8 = (undefined4 *)0x0;
    }
    else {
      uVar3 = param_1[0xb];
      uVar4 = param_1[10];
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,6);
      *puVar8 = &PTR_FUN_005a3e7c;
      iVar6 = FUN_004bb5c0(uVar4,uVar3,0x888);
      if (iVar6 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
      }
    }
    local_4 = 0xffffffff;
    param_1[0x12] = puVar8;
    param_1[0x13] = 0;
  }
  else {
    piVar7 = (int *)FUN_00482fb0(0x74);
    local_4 = 1;
    if (piVar7 == (int *)0x0) {
      piVar7 = (int *)0x0;
    }
    else {
      uVar3 = param_1[10];
      uVar4 = param_1[0xb];
      bVar11 = DAT_0066818c != 0;
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,2);
      *piVar7 = (int)&PTR_FUN_005a3e7c;
      iVar6 = FUN_004bb5c0(uVar3,uVar4,(-(uint)bVar11 & 0xfffffc00) + 0x800);
      if (iVar6 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
      }
    }
    iVar6 = *piVar7;
    local_4 = 0xffffffff;
    param_1[0x12] = piVar7;
    (**(code **)(iVar6 + 0x1c))(1);
    piVar7 = (int *)FUN_00482fb0(0x74);
    puStack_8 = (undefined1 *)0x3;
    if (piVar7 == (int *)0x0) {
      piVar7 = (int *)0x0;
    }
    else {
      uVar3 = param_1[10];
      uVar4 = param_1[0xb];
      bVar11 = DAT_0066818c != 0;
      FUN_004bcb00();
      puStack_8 = (undefined1 *)CONCAT31(puStack_8._1_3_,4);
      *piVar7 = (int)&PTR_FUN_005a3e7c;
      iVar6 = FUN_004bb5c0(uVar3,uVar4,(-(uint)bVar11 & 0xfffffc00) + 0x800);
      if (iVar6 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
      }
    }
    iVar6 = *piVar7;
    puStack_8 = (undefined1 *)0xffffffff;
    param_1[0x13] = piVar7;
    (**(code **)(iVar6 + 0x1c))(1);
  }
  param_1[0x16] = 0xc;
  param_1[0x15] = 0;
  FUN_00534470();
  ExceptionList = pvStack_c;
  return param_1;
}


