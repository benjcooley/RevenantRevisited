// FUN_00546b50 @ 00546b50 size=537

undefined4 __fastcall FUN_00546b50(int *param_1)

{
  int *piVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int *local_118;
  int local_114;
  void *pvStack_18;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1fb2;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  FUN_00434e40();
  piVar1 = (int *)FUN_00482fb0(0x78);
  local_4 = 0;
  local_118 = piVar1;
  if (piVar1 == (int *)0x0) {
    piVar1 = (int *)0x0;
  }
  else {
    local_114 = param_1[3];
    iVar4 = param_1[4];
    FUN_004bcb00();
    local_4 = CONCAT31(local_4._1_3_,1);
    *piVar1 = (int)&PTR_FUN_005a3980;
    piVar1[0x1a] = 0;
    FUN_004a5740(local_114,iVar4,0x200,piVar1[4],0);
    piVar1[0x1c] = 1;
  }
  iVar4 = *piVar1;
  local_4 = 0xffffffff;
  param_1[99] = (int)piVar1;
  (**(code **)(iVar4 + 0x40))(param_1[1],param_1[2]);
  puVar2 = (undefined4 *)FUN_00482fb0(0x6c);
  pvStack_c = (void *)0x2;
  if (puVar2 == (undefined4 *)0x0) {
    puVar2 = (undefined4 *)0x0;
  }
  else {
    uVar3 = FUN_0046d710(s_RingT_005e539c);
    FUN_004bcb00();
    pvStack_c = (void *)CONCAT31(pvStack_c._1_3_,3);
    *puVar2 = &PTR_FUN_005a3ee4;
    FUN_004a3a40(uVar3);
  }
  pvStack_c = (void *)0xffffffff;
  param_1[0x62] = (int)puVar2;
  FUN_0058b100(&local_118,&DAT_005e53b4,&DAT_0065dde8,s_statpane_def_005e53a4);
  iVar4 = FUN_004a13f0(&local_118,&DAT_005e53bc,0);
  if (iVar4 == 0) {
    FUN_00481c10(s_Unable_to_load_STATPANE_DEF_005e53c0,0);
  }
  iVar5 = FUN_004a17b0(iVar4);
  param_1[0x69] = iVar5;
  if (iVar5 < 1) {
    param_1[0x68] = 0;
    param_1[0x69] = 0;
  }
  else {
    iVar5 = FUN_00482fb0(iVar5 + 1);
    param_1[0x68] = iVar5;
    FUN_004a15a0(iVar5,param_1[0x69],1,iVar4);
    iVar4 = param_1[0x69];
    iVar5 = param_1[0x68];
    *(undefined1 *)(iVar5 + iVar4) = 0;
    iVar4 = FUN_004834e0(iVar5,iVar4);
    if (iVar4 != 0) {
      FUN_00483540(param_1[0x68],param_1[0x69]);
    }
  }
  uVar3 = DAT_00667fcc;
  param_1[0x6a] = 0;
  param_1[0x5f] = 0;
  param_1[0x61] = 0;
  param_1[100] = 0;
  param_1[0x65] = 0;
  param_1[0x6b] = 0;
  param_1[0x6c] = 0;
  param_1[0x67] = 0;
  (**(code **)(*param_1 + 0xb8))(uVar3);
  ExceptionList = pvStack_18;
  return 1;
}


