// FUN_005449e0_SpellbookSidebarPane_Initialize @ 005449e0 size=1120

void __fastcall FUN_005449e0_SpellbookSidebarPane_Initialize(int param_1)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  void *unaff_ESI;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1f74;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  FUN_00434e40_TButtonPane_Initialize();
  puVar1 = (undefined4 *)FUN_00482fb0(0x78);
  iVar6 = 0;
  local_4 = 0;
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)0x0;
  }
  else {
    uVar2 = FUN_0046d710(s_ScrlTop_005e5260);
    FUN_004bcb00();
    local_4 = CONCAT31(local_4._1_3_,1);
    *puVar1 = &PTR_FUN_005a3980;
    puVar1[0x1a] = 0;
    FUN_004a5ca0(uVar2,0x400);
    puVar1[0x1c] = 1;
  }
  local_4 = 0xffffffff;
  *(undefined4 **)(param_1 + 0x184) = puVar1;
  puVar1 = (undefined4 *)FUN_00482fb0(0x78);
  local_4 = 2;
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)0x0;
  }
  else {
    uVar2 = FUN_0046d710(s_ScrlBottom_005e5268);
    FUN_004bcb00();
    local_4 = CONCAT31(local_4._1_3_,3);
    *puVar1 = &PTR_FUN_005a3980;
    puVar1[0x1a] = 0;
    FUN_004a5ca0(uVar2,0x400);
    puVar1[0x1c] = 1;
  }
  local_4 = 0xffffffff;
  *(undefined4 **)(param_1 + 0x188) = puVar1;
  puVar1 = (undefined4 *)FUN_00482fb0(0x78);
  local_4 = 4;
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)0x0;
  }
  else {
    FUN_004bcb00();
    local_4 = CONCAT31(local_4._1_3_,5);
    *puVar1 = &PTR_FUN_005a3980;
    puVar1[0x1a] = 0;
    FUN_004a5740(0x94,0xe5,0x400,puVar1[4],0);
    puVar1[0x1c] = 1;
  }
  local_4 = 0xffffffff;
  *(undefined4 **)(param_1 + 0x180) = puVar1;
  puVar1 = (undefined4 *)FUN_00482fb0(0x78);
  local_4 = 6;
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)0x0;
  }
  else {
    uVar2 = FUN_0046d710(s_ScrollPaper_005e5274);
    FUN_004bcb00();
    local_4 = CONCAT31(local_4._1_3_,7);
    *puVar1 = &PTR_FUN_005a3980;
    puVar1[0x1a] = 0;
    FUN_004a5ca0(uVar2,0x400);
    puVar1[0x1c] = 1;
  }
  local_4 = 0xffffffff;
  *(undefined4 **)(param_1 + 0x18c) = puVar1;
  puVar1 = (undefined4 *)FUN_00482fb0(0x78);
  local_4 = 8;
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)0x0;
  }
  else {
    FUN_004bcb00();
    local_4 = CONCAT31(local_4._1_3_,9);
    *puVar1 = &PTR_FUN_005a3980;
    puVar1[0x1a] = 0;
    FUN_004a5740(0x84,200,0x400,puVar1[4],0);
    puVar1[0x1c] = 1;
  }
  iVar3 = *(int *)(param_1 + 0x180);
  local_4 = 0xffffffff;
  *(undefined4 **)(iVar3 + 0x5c) = puVar1;
  *(undefined4 *)(iVar3 + 0x60) = 1;
  uVar2 = FUN_0046d710(s_Scroll_005e5280);
  *(undefined4 *)(param_1 + 400) = uVar2;
  FUN_00435660(uVar2);
  (**(code **)(**(int **)(param_1 + 0x180) + 0x48))(1);
  iVar3 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
  iVar3 = (-(uint)(iVar3 != 0x10) & 0xffff8400) + 0xf81f;
  (**(code **)(**(int **)(param_1 + 0x180) + 0x1c))(iVar3);
  (**(code **)(**(int **)(param_1 + 0x184) + 0x1c))(iVar3);
  (**(code **)(**(int **)(param_1 + 0x188) + 0x1c))(iVar3);
  (**(code **)(**(int **)(param_1 + 0x18c) + 0x1c))(iVar3);
  *(undefined4 *)(param_1 + 0x17c) = 0;
  *(undefined4 *)(param_1 + 0x1cc) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x1c8) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x1d0) = 0;
  *(undefined4 *)(param_1 + 0x1a0) = 0;
  *(undefined4 *)(param_1 + 0x19c) = 1;
  *(undefined4 *)(param_1 + 0x198) = 1;
  uVar2 = FUN_0046d710(s_ScrlArwUU_005e5294);
  uVar4 = FUN_0046d710(s_ScrlArwUD_005e52a0);
  uVar5 = FUN_0046d710(s_ScrlArwUG_005e5288);
  iVar3 = FUN_00482fb0(0x148);
  if (iVar3 == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = FUN_0042c2d0(&DAT_005e52ac,0xa9,0x96,0x18,0x18,0,&LAB_005449c0,uVar4,uVar2,0,0,1,
                         0xffffffff,0,0,0,uVar5);
  }
  FUN_00436ae0(uVar2);
  if (*(int *)(param_1 + 0x88) == 0) {
    iVar3 = 0;
  }
  else {
    iVar3 = **(int **)(param_1 + 0x98);
  }
  *(uint *)(iVar3 + 0xd4) = *(uint *)(iVar3 + 0xd4) | 0x2000;
  uVar2 = FUN_0046d710(s_ScrlArwDU_005e52bc);
  uVar4 = FUN_0046d710(s_ScrlArwDD_005e52c8);
  uVar5 = FUN_0046d710(s_ScrlArwDG_005e52b0);
  iVar3 = FUN_00482fb0(0x148);
  if (iVar3 == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = FUN_0042c2d0(&DAT_005e52d4,0xa9,0xae,0x18,0x18,0,&LAB_005449d0,uVar4,uVar2,0,0,1,
                         0xffffffff,0,0,0,uVar5);
  }
  FUN_00436ae0(uVar2);
  if (1 < *(uint *)(param_1 + 0x88)) {
    iVar6 = *(int *)(*(int *)(param_1 + 0x98) + 4);
  }
  *(uint *)(iVar6 + 0xd4) = *(uint *)(iVar6 + 0xd4) | 0x2000;
  ExceptionList = unaff_ESI;
  return;
}


