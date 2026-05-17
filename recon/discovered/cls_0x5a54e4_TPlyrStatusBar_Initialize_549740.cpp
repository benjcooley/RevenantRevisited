// FUN_00549740_TPlyrStatusBar_Initialize @ 00549740 size=1528

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Enum "SectionFlags": Some values do not have unique names */

undefined4 __fastcall FUN_00549740_TPlyrStatusBar_Initialize(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  undefined4 *puVar5;
  int iVar6;
  undefined4 *puVar7;
  bool bVar8;
  undefined4 uVar9;
  void *pvVar10;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2174;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  FUN_00491900_TPane_Initialize();
  if (DAT_006680c8 == 0) {
    piVar4 = (int *)FUN_00482fb0(0x78);
    local_4 = 0xc;
    if (piVar4 == (int *)0x0) {
      piVar4 = (int *)0x0;
    }
    else {
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,0xd);
      *piVar4 = (int)&PTR_FUN_005a3980;
      piVar4[0x1a] = 0;
      FUN_004a5740(0x80,0x80,&IMAGE_SECTION_HEADER_00400200,piVar4[4],0);
      piVar4[0x1c] = 1;
    }
    uVar9 = _DAT_006668d0;
    pvVar10 = (void *)0x7f7f;
    iVar6 = *piVar4;
    iVar1 = piVar4[2];
    iVar2 = piVar4[1];
    local_4 = 0xffffffff;
    *(int **)(param_1 + 100) = piVar4;
    (**(code **)(iVar6 + 100))(0,0,iVar2,iVar1,uVar9,0xffff,0x7f7f,0x80000000);
    puVar5 = (undefined4 *)FUN_00482fb0(0x78);
    if (puVar5 == (undefined4 *)0x0) {
      puVar5 = (undefined4 *)0x0;
    }
    else {
      FUN_004bcb00();
      *puVar5 = &PTR_FUN_005a3980;
      puVar5[0x1a] = 0;
      FUN_004a5740(0x28,0x28,0x200,puVar5[4],0);
      puVar5[0x1c] = 1;
    }
    *(undefined4 **)(param_1 + 0x60) = puVar5;
    puVar5 = (undefined4 *)FUN_00482fb0(0x74);
    if (puVar5 == (undefined4 *)0x0) {
      puVar5 = (undefined4 *)0x0;
    }
    else {
      FUN_004bcb00();
      *puVar5 = &PTR_FUN_005a3e7c;
      iVar6 = FUN_004bb5c0(0x80,0x40,0x488);
      if (iVar6 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
      }
    }
    *(undefined4 **)(param_1 + 0x6c) = puVar5;
    puVar5 = (undefined4 *)FUN_00482fb0(0x74);
    if (puVar5 == (undefined4 *)0x0) {
      puVar5 = (undefined4 *)0x0;
    }
    else {
      FUN_004bcb00();
      *puVar5 = &PTR_FUN_005a3e7c;
      iVar6 = FUN_004bb5c0(0x80,0x40,0x4000888);
      if (iVar6 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
      }
    }
    *(undefined4 **)(param_1 + 0x70) = puVar5;
    puVar5 = (undefined4 *)FUN_00482fb0(0x74);
    if (puVar5 == (undefined4 *)0x0) {
      puVar5 = (undefined4 *)0x0;
    }
    else {
      FUN_004bcb00();
      *puVar5 = &PTR_FUN_005a3e7c;
      iVar6 = FUN_004bb5c0(0x80,0x40,0x4000888);
      if (iVar6 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
      }
    }
    *(undefined4 **)(param_1 + 0x74) = puVar5;
    puVar5 = (undefined4 *)FUN_0046d710(&DAT_005e5724);
    puVar7 = (undefined4 *)FUN_00482fb0(0x78);
    if (puVar7 == (undefined4 *)0x0) {
      puVar7 = (undefined4 *)0x0;
    }
    else {
      uVar9 = puVar5[1];
      uVar3 = *puVar5;
      FUN_004bcb00();
      *puVar7 = &PTR_FUN_005a3980;
      puVar7[0x1a] = 0;
      FUN_004a5740(uVar3,uVar9,0x488,puVar7[4],0);
      puVar7[0x1c] = 1;
    }
    *(undefined4 **)(param_1 + 0x68) = puVar7;
  }
  else {
    piVar4 = (int *)FUN_00482fb0(0x78);
    local_4 = 0;
    if (piVar4 == (int *)0x0) {
      piVar4 = (int *)0x0;
    }
    else {
      bVar8 = DAT_0066818c != 0;
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,1);
      *piVar4 = (int)&PTR_FUN_005a3980;
      piVar4[0x1a] = 0;
      FUN_004a5740(0x80,0x80,(-(uint)bVar8 & 0xfffffe00) + 0x400 | 0x400000,piVar4[4],0);
      piVar4[0x1c] = 1;
    }
    uVar9 = _DAT_006668d0;
    pvVar10 = (void *)0x7f7f;
    iVar6 = *piVar4;
    iVar1 = piVar4[2];
    iVar2 = piVar4[1];
    local_4 = 0xffffffff;
    *(int **)(param_1 + 100) = piVar4;
    (**(code **)(iVar6 + 100))(0,0,iVar2,iVar1,uVar9,0xffff,0x7f7f,0x80000000);
    puVar5 = (undefined4 *)FUN_00482fb0(0x78);
    if (puVar5 == (undefined4 *)0x0) {
      puVar5 = (undefined4 *)0x0;
    }
    else {
      FUN_004bcb00();
      *puVar5 = &PTR_FUN_005a3980;
      puVar5[0x1a] = 0;
      FUN_004a5740(0x28,0x28,0x200,puVar5[4],0);
      puVar5[0x1c] = 1;
    }
    *(undefined4 **)(param_1 + 0x60) = puVar5;
    puVar5 = (undefined4 *)FUN_00482fb0(0x74);
    if (puVar5 == (undefined4 *)0x0) {
      puVar5 = (undefined4 *)0x0;
    }
    else {
      FUN_004bcb00();
      *puVar5 = &PTR_FUN_005a3e7c;
      iVar6 = FUN_004bb5c0(0x80,0x40,0x400);
      if (iVar6 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
      }
    }
    *(undefined4 **)(param_1 + 0x6c) = puVar5;
    puVar5 = (undefined4 *)FUN_00482fb0(0x74);
    if (puVar5 == (undefined4 *)0x0) {
      puVar5 = (undefined4 *)0x0;
    }
    else {
      bVar8 = DAT_0066818c != 0;
      FUN_004bcb00();
      *puVar5 = &PTR_FUN_005a3e7c;
      iVar6 = FUN_004bb5c0(0x80,0x40,(-(uint)bVar8 & 0xfffffc00) + 0x800);
      if (iVar6 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
      }
    }
    *(undefined4 **)(param_1 + 0x70) = puVar5;
    puVar5 = (undefined4 *)FUN_00482fb0(0x74);
    if (puVar5 == (undefined4 *)0x0) {
      puVar5 = (undefined4 *)0x0;
    }
    else {
      bVar8 = DAT_0066818c != 0;
      FUN_004bcb00();
      *puVar5 = &PTR_FUN_005a3e7c;
      iVar6 = FUN_004bb5c0(0x80,0x40,(-(uint)bVar8 & 0xfffffc00) + 0x800);
      if (iVar6 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
      }
    }
    *(undefined4 **)(param_1 + 0x74) = puVar5;
    puVar5 = (undefined4 *)FUN_0046d710(&DAT_005e571c);
    puVar7 = (undefined4 *)FUN_00482fb0(0x78);
    if (puVar7 == (undefined4 *)0x0) {
      *(undefined4 *)(param_1 + 0x68) = 0;
    }
    else {
      uVar9 = puVar5[1];
      uVar3 = *puVar5;
      bVar8 = DAT_0066818c != 0;
      FUN_004bcb00();
      *puVar7 = &PTR_FUN_005a3980;
      puVar7[0x1a] = 0;
      FUN_004a5740(uVar3,uVar9,(-(uint)bVar8 & 0xfffffe00) + 0x400,puVar7[4],0);
      puVar7[0x1c] = 1;
      *(undefined4 **)(param_1 + 0x68) = puVar7;
    }
  }
  *(undefined4 *)(param_1 + 0x94) = 0;
  *(undefined4 *)(param_1 + 0xa0) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x9c) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x98) = 0xffffffff;
  *(undefined4 *)(param_1 + 0xa8) = 0;
  *(undefined4 *)(param_1 + 0xb4) = 0xffffffff;
  *(undefined4 *)(param_1 + 0xb0) = 0xffffffff;
  *(undefined4 *)(param_1 + 0xac) = 0xffffffff;
  *(undefined4 *)(param_1 + 0xa4) = 0xffffffff;
  *(undefined4 *)(param_1 + 0xc0) = 0;
  *(undefined4 *)(param_1 + 0xbc) = 0;
  *(undefined4 *)(param_1 + 0xe0) = 0;
  *(undefined4 *)(param_1 + 0xdc) = 0;
  *(undefined4 *)(param_1 + 0xd8) = 0;
  *(undefined4 *)(param_1 + 0xd4) = 0;
  *(undefined4 *)(param_1 + 200) = 0;
  *(undefined4 *)(param_1 + 0xc4) = 0;
  *(undefined4 *)(param_1 + 0xd0) = 0;
  *(undefined4 *)(param_1 + 0xcc) = 0;
  ExceptionList = pvVar10;
  return 1;
}


