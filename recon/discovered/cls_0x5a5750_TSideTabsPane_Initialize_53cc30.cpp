// FUN_0053cc30_TSideTabsPane_Initialize @ 0053cc30 size=1838

undefined4 __fastcall FUN_0053cc30_TSideTabsPane_Initialize(int *param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 *puVar4;
  int *piVar5;
  bool bVar6;
  void *local_14;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1d44;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  FUN_00434e40();
  FUN_00429950(0xff,0);
  FUN_004384a0();
  iVar3 = FUN_0046d710();
  param_1[0x5f] = iVar3;
  iVar3 = FUN_0046d710();
  param_1[0x60] = iVar3;
  iVar3 = FUN_0046d710();
  param_1[0x61] = iVar3;
  if (DAT_006680c8 == 0) {
    puVar4 = (undefined4 *)FUN_00482fb0();
    local_4 = 6;
    if (puVar4 == (undefined4 *)0x0) {
      puVar4 = (undefined4 *)0x0;
    }
    else {
      uVar1 = ((undefined4 *)param_1[0x5f])[1];
      uVar2 = *(undefined4 *)param_1[0x5f];
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,7);
      *puVar4 = &PTR_FUN_005a3e7c;
      iVar3 = FUN_004bb5c0(uVar2,uVar1);
      if (iVar3 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    local_4 = 0xffffffff;
    param_1[0x62] = (int)puVar4;
    puVar4 = (undefined4 *)FUN_00482fb0();
    local_4 = 8;
    if (puVar4 == (undefined4 *)0x0) {
      puVar4 = (undefined4 *)0x0;
    }
    else {
      uVar1 = ((undefined4 *)param_1[0x5f])[1];
      uVar2 = *(undefined4 *)param_1[0x5f];
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,9);
      *puVar4 = &PTR_FUN_005a3e7c;
      iVar3 = FUN_004bb5c0(uVar2,uVar1);
      if (iVar3 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    local_4 = 0xffffffff;
    param_1[99] = (int)puVar4;
    puVar4 = (undefined4 *)FUN_00482fb0();
    local_4 = 10;
    if (puVar4 == (undefined4 *)0x0) {
      puVar4 = (undefined4 *)0x0;
    }
    else {
      uVar1 = ((undefined4 *)param_1[0x5f])[1];
      uVar2 = *(undefined4 *)param_1[0x5f];
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,0xb);
      *puVar4 = &PTR_FUN_005a3e7c;
      iVar3 = FUN_004bb5c0(uVar2,uVar1);
      if (iVar3 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    param_1[100] = (int)puVar4;
  }
  else {
    puVar4 = (undefined4 *)FUN_00482fb0();
    local_4 = 0;
    if (puVar4 == (undefined4 *)0x0) {
      puVar4 = (undefined4 *)0x0;
    }
    else {
      uVar1 = ((undefined4 *)param_1[0x5f])[1];
      uVar2 = *(undefined4 *)param_1[0x5f];
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,1);
      *puVar4 = &PTR_FUN_005a3e7c;
      iVar3 = FUN_004bb5c0(uVar2,uVar1);
      if (iVar3 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    local_4 = 0xffffffff;
    param_1[0x62] = (int)puVar4;
    puVar4 = (undefined4 *)FUN_00482fb0();
    local_4 = 2;
    if (puVar4 == (undefined4 *)0x0) {
      puVar4 = (undefined4 *)0x0;
    }
    else {
      uVar1 = *(undefined4 *)param_1[0x5f];
      uVar2 = ((undefined4 *)param_1[0x5f])[1];
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,3);
      *puVar4 = &PTR_FUN_005a3e7c;
      iVar3 = FUN_004bb5c0(uVar1,uVar2);
      if (iVar3 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    local_4 = 0xffffffff;
    param_1[99] = (int)puVar4;
    puVar4 = (undefined4 *)FUN_00482fb0();
    local_4 = 4;
    if (puVar4 == (undefined4 *)0x0) {
      puVar4 = (undefined4 *)0x0;
    }
    else {
      uVar1 = ((undefined4 *)param_1[0x5f])[1];
      uVar2 = *(undefined4 *)param_1[0x5f];
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,5);
      *puVar4 = &PTR_FUN_005a3e7c;
      iVar3 = FUN_004bb5c0(uVar2,uVar1);
      if (iVar3 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70);
      }
    }
    param_1[100] = (int)puVar4;
  }
  local_4 = 0xffffffff;
  bVar6 = DAT_0065d1b8 != 2;
  iVar3 = FUN_00482fb0();
  local_4 = 0xc;
  if (iVar3 != 0) {
    FUN_0042c600(&DAT_005e4720,3,2,0x24,0x23,0,0,0,0,0xffffffff,bVar6 - 1 & 0x10000 | 0x180010,1);
  }
  local_4 = 0xffffffff;
  FUN_00436790();
  iVar3 = FUN_00436900();
  param_1[0x67] = iVar3;
  bVar6 = DAT_0065d1b8 != 1;
  iVar3 = FUN_00482fb0();
  local_4 = 0xd;
  if (iVar3 != 0) {
    FUN_0042c600(s_Stats_005e4728,3,0x27,0x24,0x23,0,0,0,0,0xffffffff,
                 (-(uint)bVar6 & 0xffff0000) + 0x10000 | 0x180010,1);
  }
  local_4 = 0xffffffff;
  FUN_00436790();
  iVar3 = FUN_00436900();
  param_1[0x66] = iVar3;
  bVar6 = DAT_0065d1b8 != 0;
  iVar3 = FUN_00482fb0();
  local_4 = 0xe;
  if (iVar3 != 0) {
    FUN_0042c600(s_Equip_005e4730,3,0x4c,0x24,0x23,0,0,0,0,0xffffffff,
                 (-(uint)bVar6 & 0xffff0000) + 0x10000 | 0x180010,1);
  }
  local_4 = 0xffffffff;
  FUN_00436790();
  iVar3 = FUN_00436900();
  param_1[0x65] = iVar3;
  bVar6 = DAT_0065d1bc != 2;
  iVar3 = FUN_00482fb0();
  local_4 = 0xf;
  if (iVar3 != 0) {
    FUN_0042c600(s_Spell_005e4738,3,0x71,0x24,0x23,0,0,0,0,0xffffffff,bVar6 - 1 & 0x10000 | 0x180010
                 ,2);
  }
  local_4 = 0xffffffff;
  FUN_00436790();
  iVar3 = FUN_00436900();
  param_1[0x69] = iVar3;
  bVar6 = DAT_0065d1bc != 0;
  iVar3 = FUN_00482fb0();
  local_4 = 0x10;
  if (iVar3 != 0) {
    FUN_0042c600(&DAT_005e4740,3,0x96,0x24,0x23,0,0,0,0,0xffffffff,
                 (-(uint)bVar6 & 0xffff0000) + 0x10000 | 0x180010,2);
  }
  local_4 = 0xffffffff;
  FUN_00436790();
  iVar3 = FUN_00436900();
  param_1[0x68] = iVar3;
  bVar6 = DAT_0065d1bc != 1;
  iVar3 = FUN_00482fb0();
  local_4 = 0x11;
  if (iVar3 != 0) {
    FUN_0042c600(&DAT_005e4744,3,0xbb,0x24,0x23,0,0,0,0,0xffffffff,
                 (-(uint)bVar6 & 0xffff0000) + 0x10000 | 0x180010,2);
  }
  local_4 = 0xffffffff;
  FUN_00436790();
  iVar3 = FUN_00436900();
  param_1[0x6a] = iVar3;
  param_1[0x18] = param_1[0x18] | 2;
  iVar3 = 0;
  if (0 < param_1[0x22]) {
    piVar5 = param_1 + 0x6b;
    do {
      piVar5[6] = 0;
      *piVar5 = 0;
      iVar3 = iVar3 + 1;
      piVar5 = piVar5 + 1;
    } while (iVar3 < param_1[0x22]);
  }
  iVar3 = DAT_00667c60 - DAT_0065be60;
  param_1[5] = (*(int *)(PTR_DAT_005d79e0 + 4) - DAT_0066614c) - DAT_0065be5c;
  param_1[6] = iVar3;
  (**(code **)(*param_1 + 0x20))(*(undefined4 *)param_1[0x5f]);
  ExceptionList = local_14;
  return 1;
}


