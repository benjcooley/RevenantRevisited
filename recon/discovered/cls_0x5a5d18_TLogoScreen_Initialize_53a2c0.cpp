// FUN_0053a2c0_TLogoScreen_Initialize @ 0053a2c0 size=952

undefined4 __fastcall FUN_0053a2c0_TLogoScreen_Initialize(int param_1)

{
  uint *puVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  undefined *puVar4;
  undefined4 uVar5;
  undefined4 *puVar6;
  int iVar7;
  void *unaff_EBX;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_005a1be2;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  *(undefined4 *)(param_1 + 0x54) = 0;
  FUN_0051eda0_TPlayerManager_Clear();
  FUN_00446cb0();
  DAT_0066fcac = FUN_0047f670(s_menus_dat_005e43dc);
  FUN_0046d710();
  FUN_0043a020();
  iVar7 = *(int *)(param_1 + 0x70);
  uVar5 = FUN_00444e20();
  FUN_00419dd0(uVar5);
  (**(code **)(iVar7 + 0x30))(8);
  *(int *)(param_1 + 0x40) = param_1 + 0x70;
  *(int *)(param_1 + 0x44) = param_1 + 0x70;
  puVar6 = (undefined4 *)FUN_00482fb0(0x17c);
  puVar4 = PTR_DAT_005d79e0;
  if (puVar6 == (undefined4 *)0x0) {
    puVar6 = (undefined4 *)0x0;
  }
  else {
    *puVar6 = &PTR_FUN_005a4494;
    uVar5 = *(undefined4 *)(puVar4 + 8);
    uVar2 = *(undefined4 *)(puVar4 + 4);
    puVar6[3] = uVar2;
    puVar6[7] = uVar2;
    puVar6[1] = 0;
    puVar6[5] = 0;
    puVar6[2] = 0;
    puVar6[6] = 0;
    puVar6[4] = uVar5;
    puVar6[8] = uVar5;
    puVar6[0x12] = 0;
    puVar6[0x13] = 0;
    puVar6[0x14] = 0;
    puVar6[0x15] = 0;
    FUN_0041c7f0(0x10,0x10);
    *puVar6 = &PTR_FUN_005a4510;
  }
  puVar3 = (undefined4 *)*puVar6;
  *(undefined4 **)(param_1 + 0x88) = puVar6;
  (*(code *)*puVar3)();
  puVar1 = (uint *)(*(int *)(param_1 + 0x88) + 0x60);
  *puVar1 = *puVar1 | 2;
  iVar7 = FUN_00482fb0(0x148);
  if (iVar7 == 0) {
    uVar5 = 0;
  }
  else {
    uVar5 = FUN_0042c400(DAT_0066fcac,s_MenuNewGame_005e43f0,0,0,0,0,0xffffffff,0x10,0xffffffff,0);
  }
  FUN_00436790(uVar5);
  iVar7 = FUN_00482fb0(0x148);
  if (iVar7 == 0) {
    uVar5 = 0;
  }
  else {
    uVar5 = FUN_0042c400(DAT_0066fcac,s_MenuLoadGame_005e43fc,0,0,0,0,0xffffffff,0x10,0xffffffff,0);
  }
  FUN_00436790(uVar5);
  iVar7 = FUN_00482fb0(0x148);
  if (iVar7 == 0) {
    uVar5 = 0;
  }
  else {
    uVar5 = FUN_0042c400(DAT_0066fcac,s_MenuMulti_005e440c,0,0,0,0,0xffffffff,0x10,0xffffffff,0);
  }
  FUN_00436790(uVar5);
  iVar7 = FUN_00482fb0(0x148);
  if (iVar7 == 0) {
    uVar5 = 0;
  }
  else {
    uVar5 = FUN_0042c400(DAT_0066fcac,s_MenuOptions_005e4418,0,0,0,0,0xffffffff,0x10,0xffffffff,0);
  }
  FUN_00436790(uVar5);
  iVar7 = FUN_00482fb0(0x148);
  if (iVar7 == 0) {
    uVar5 = 0;
  }
  else {
    uVar5 = FUN_0042c400(DAT_0066fcac,s_MenuExit_005e4424,0,0,0,0,0xffffffff,0x10,0xffffffff,0);
  }
  FUN_00436790(uVar5);
  iVar7 = FUN_00436900(0);
  *(undefined1 **)(iVar7 + 0x80) = &LAB_0053a1f0;
  *(uint *)(iVar7 + 0x14) = *(uint *)(iVar7 + 0x14) | 0x40;
  iVar7 = FUN_00436900(1);
  *(undefined1 **)(iVar7 + 0x80) = &LAB_0053a220;
  *(uint *)(iVar7 + 0x14) = *(uint *)(iVar7 + 0x14) | 0x40;
  iVar7 = FUN_00436900(2);
  *(undefined1 **)(iVar7 + 0x80) = &LAB_0053a240;
  *(uint *)(iVar7 + 0x14) = *(uint *)(iVar7 + 0x14) | 0x40;
  iVar7 = FUN_00436900(3);
  *(undefined1 **)(iVar7 + 0x80) = &LAB_0053a260;
  *(uint *)(iVar7 + 0x14) = *(uint *)(iVar7 + 0x14) | 0x40;
  iVar7 = FUN_00436900(4);
  *(uint *)(iVar7 + 0x14) = *(uint *)(iVar7 + 0x14) | 0x40;
  uVar5 = *(undefined4 *)(param_1 + 0x88);
  *(undefined1 **)(iVar7 + 0x80) = &LAB_0053a2a0;
  FUN_0048ed90(uVar5,0xffffffff);
  ExceptionList = unaff_EBX;
  return 1;
}


