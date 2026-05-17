// FUN_0054bf70_TTextBar_Initialize @ 0054bf70 size=1110

/* WARNING: Enum "SectionFlags": Some values do not have unique names */

undefined4 __fastcall FUN_0054bf70_TTextBar_Initialize(int param_1)

{
  undefined *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  int *piVar7;
  undefined4 *puVar8;
  undefined4 uVar9;
  bool bVar10;
  undefined4 uVar11;
  void *pvVar12;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  uVar9 = DAT_005e5804;
  puVar1 = PTR_DAT_005d79e0;
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a2222;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *(int *)(param_1 + 100) = DAT_005e5800;
  uVar6 = *(undefined4 *)(puVar1 + 4);
  *(undefined4 *)(param_1 + 0x68) = uVar9;
  iVar5 = DAT_0065b020;
  *(undefined4 *)(param_1 + 0x70) = uVar6;
  iVar5 = *(int *)(iVar5 + DAT_0065abc4 * 4);
  if (iVar5 == 0) {
    iVar5 = DAT_0065b024;
  }
  iVar2 = FUN_00521c60(DAT_0065abc4);
  iVar3 = DAT_0065abc4;
  iVar4 = *(int *)(DAT_0065b020 + DAT_0065abc4 * 4);
  *(int *)(param_1 + 0x74) = (*(int *)(iVar2 + 0x54) + *(int *)(iVar5 + 0x50)) * 0xc;
  if (iVar4 == 0) {
    iVar4 = DAT_0065b024;
  }
  iVar3 = FUN_00521c60(iVar3);
  uVar9 = DAT_006668e4;
  iVar5 = DAT_006668e0;
  uVar6 = DAT_006668dc;
  iVar4 = *(int *)(iVar3 + 0x54) + *(int *)(iVar4 + 0x50);
  iVar3 = DAT_005e5800 * iVar4;
  *(int *)(param_1 + 0x78) = iVar4;
  iVar4 = DAT_006668e8;
  *(int *)(param_1 + 0x20) = iVar3;
  *(undefined4 *)(param_1 + 0x14) = uVar6;
  *(int *)(param_1 + 0x18) = (iVar4 - iVar3) + iVar5;
  *(undefined4 *)(param_1 + 0x1c) = uVar9;
  iVar5 = FUN_00491900_TPane_Initialize();
  if (iVar5 == 0) {
    ExceptionList = local_c;
    return 0;
  }
  uVar6 = FUN_00482fb0(0x450);
  *(undefined4 *)(param_1 + 0x6c) = uVar6;
  bVar10 = DAT_006680c8 == 0;
  *(undefined4 *)(param_1 + 0x60) = 0;
  if (bVar10) {
    piVar7 = (int *)FUN_00482fb0(0x78);
    local_4 = 6;
    if (piVar7 == (int *)0x0) {
      piVar7 = (int *)0x0;
    }
    else {
      iVar5 = *(int *)(DAT_0065b020 + DAT_0065abc4 * 4);
      if (iVar5 == 0) {
        iVar5 = DAT_0065b024;
      }
      iVar4 = FUN_00521c60(DAT_0065abc4);
      iVar4 = *(int *)(iVar4 + 0x54);
      iVar5 = *(int *)(iVar5 + 0x50);
      uVar6 = *(undefined4 *)(param_1 + 0x70);
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,7);
      *piVar7 = (int)&PTR_FUN_005a3980;
      piVar7[0x1a] = 0;
      FUN_004a5740(uVar6,iVar4 + iVar5,&IMAGE_SECTION_HEADER_00400200,piVar7[4],0);
      piVar7[0x1c] = 1;
    }
    iVar5 = piVar7[2];
    iVar4 = *piVar7;
    pvVar12 = (void *)0xffff;
    iVar3 = piVar7[1];
    local_4 = 0xffffffff;
    *(int **)(param_1 + 0x84) = piVar7;
    (**(code **)(iVar4 + 100))(0,0,iVar3,iVar5,0,0xffff,0x7f7f,0x80000000);
    piVar7 = (int *)FUN_00482fb0(0x74);
    if (piVar7 == (int *)0x0) {
      piVar7 = (int *)0x0;
    }
    else {
      uVar6 = *(undefined4 *)(param_1 + 0x74);
      uVar9 = *(undefined4 *)(param_1 + 0x70);
      FUN_004bcb00();
      *piVar7 = (int)&PTR_FUN_005a3e7c;
      iVar5 = FUN_004bb5c0(uVar9,uVar6,0x488);
      if (iVar5 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
      }
    }
    iVar5 = piVar7[2];
    iVar4 = *piVar7;
    iVar3 = piVar7[1];
    *(int **)(param_1 + 0x88) = piVar7;
    (**(code **)(iVar4 + 100))(0,0,iVar3,iVar5,0,0xffff,0x7f7f,0x80000000);
    puVar8 = (undefined4 *)FUN_00482fb0(0x74);
    if (puVar8 == (undefined4 *)0x0) goto LAB_0054c369;
    uVar6 = *(undefined4 *)(param_1 + 0x74);
    uVar9 = *(undefined4 *)(param_1 + 0x70);
    FUN_004bcb00();
    *puVar8 = &PTR_FUN_005a3e7c;
    uVar11 = 0x4000888;
  }
  else {
    piVar7 = (int *)FUN_00482fb0(0x78);
    local_4 = 0;
    if (piVar7 == (int *)0x0) {
      piVar7 = (int *)0x0;
    }
    else {
      iVar5 = *(int *)(DAT_0065b020 + DAT_0065abc4 * 4);
      if (iVar5 == 0) {
        iVar5 = DAT_0065b024;
      }
      iVar4 = FUN_00521c60(DAT_0065abc4);
      iVar4 = *(int *)(iVar4 + 0x54);
      iVar5 = *(int *)(iVar5 + 0x50);
      uVar6 = *(undefined4 *)(param_1 + 0x70);
      FUN_004bcb00();
      local_4 = CONCAT31(local_4._1_3_,1);
      *piVar7 = (int)&PTR_FUN_005a3980;
      piVar7[0x1a] = 0;
      FUN_004a5740(uVar6,iVar4 + iVar5,&IMAGE_SECTION_HEADER_00400200,piVar7[4],0);
      piVar7[0x1c] = 1;
    }
    iVar5 = piVar7[2];
    iVar4 = *piVar7;
    pvVar12 = (void *)0xffff;
    iVar3 = piVar7[1];
    local_4 = 0xffffffff;
    *(int **)(param_1 + 0x84) = piVar7;
    (**(code **)(iVar4 + 100))(0,0,iVar3,iVar5,0,0xffff,0x7f7f,0x80000000);
    puVar8 = (undefined4 *)FUN_00482fb0(0x74);
    if (puVar8 == (undefined4 *)0x0) {
      puVar8 = (undefined4 *)0x0;
    }
    else {
      uVar6 = *(undefined4 *)(param_1 + 0x74);
      uVar9 = *(undefined4 *)(param_1 + 0x70);
      FUN_004bcb00();
      *puVar8 = &PTR_FUN_005a3e7c;
      iVar5 = FUN_004bb5c0(uVar9,uVar6,0x400);
      if (iVar5 == 0) {
        FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
      }
    }
    *(undefined4 **)(param_1 + 0x88) = puVar8;
    puVar8 = (undefined4 *)FUN_00482fb0(0x74);
    if (puVar8 == (undefined4 *)0x0) {
LAB_0054c369:
      puVar8 = (undefined4 *)0x0;
      goto LAB_0054c36b;
    }
    uVar6 = *(undefined4 *)(param_1 + 0x74);
    uVar9 = *(undefined4 *)(param_1 + 0x70);
    FUN_004bcb00();
    *puVar8 = &PTR_FUN_005a3e7c;
    uVar11 = 0x400;
  }
  iVar5 = FUN_004bb5c0(uVar9,uVar6,uVar11);
  if (iVar5 == 0) {
    FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70,0);
  }
LAB_0054c36b:
  *(undefined4 **)(param_1 + 0x8c) = puVar8;
  *(undefined4 *)(param_1 + 0x120) = 0xffffffff;
  *(int *)(param_1 + 0x7c) = puVar8[2] - *(int *)(param_1 + 0x78);
  *(undefined4 *)(param_1 + 0xa0) = 0;
  *(undefined1 *)(param_1 + 0xd0) = 0;
  *(undefined4 *)(param_1 + 0x90) = 0;
  *(undefined4 *)(param_1 + 0x94) = 0;
  *(undefined4 *)(param_1 + 0x98) = 0;
  *(undefined4 *)(param_1 + 0x9c) = 0;
  ExceptionList = pvVar12;
  return 1;
}


