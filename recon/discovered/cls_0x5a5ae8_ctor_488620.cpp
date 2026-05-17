// FUN_00488620_SpellbookSidebarPane_ctor @ 00488620 size=487

undefined4 * __fastcall FUN_00488620_SpellbookSidebarPane_ctor(undefined4 *param_1)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  undefined4 *puVar4;
  tagMSG tStack_90;
  CHAR local_74 [104];
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d6ab;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  param_1[1] = 0x1c4;
  param_1[5] = 0x1c4;
  param_1[3] = 0xbc;
  param_1[7] = 0xbc;
  *param_1 = &PTR_FUN_005a4494;
  param_1[2] = 0;
  param_1[6] = 0;
  param_1[4] = 0x132;
  param_1[8] = 0x132;
  param_1[0x12] = 0;
  param_1[0x13] = 0;
  param_1[0x14] = 0;
  param_1[0x15] = 0;
  FUN_0041c7f0(0x10,0x10);
  *param_1 = &PTR_FUN_005a45c8;
  iVar2 = DAT_00667fbc;
  DAT_0065ba08 = DAT_0065ba08 + 0x100;
  local_4 = 0;
  *(undefined2 *)(param_1 + 0x69) = 0;
  *(undefined2 *)((int)param_1 + 0x1a6) = 0x40;
  param_1[0x6a] = 0;
  if (iVar2 < DAT_0065ba08) {
    DAT_00667fbc = DAT_0065ba08;
  }
  puVar1 = _malloc(0x100);
  puVar4 = puVar1;
  for (iVar2 = 0x40; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = 0;
    puVar4 = puVar4 + 1;
  }
  if (puVar1 == (undefined4 *)0x0) {
    if (DAT_0065b9f8 != 0) {
                    /* WARNING: Subroutine does not return */
      FUN_0058d065(1);
    }
    DAT_0065b9f8 = 1;
    FUN_0058b100(local_74,s_OUT_OF_MEMORY___Increase_your_vi_005d7cd8,0);
    FUN_004820b0(local_74);
    FUN_004820b0(s_Press_any_key_to_exit_005d7bdc);
    DAT_00667fd0 = 0;
    FUN_0049e410();
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x6c))();
    FUN_004a7460();
    FUN_00448c90();
    iVar2 = PeekMessageA(&tStack_90,(HWND)0x0,0,0,1);
    while (iVar2 != 0) {
      TranslateMessage(&tStack_90);
      DispatchMessageA(&tStack_90);
      iVar2 = PeekMessageA(&tStack_90,(HWND)0x0,0,0,1);
    }
    ShowWindow(DAT_0065b8ec,0);
    MessageBoxA((HWND)0x0,local_74,s_FATAL_ERROR_005d7bf4,0x10);
                    /* WARNING: Subroutine does not return */
    FUN_0058d065(1);
  }
  param_1[0x6a] = puVar1;
  for (uVar3 = (int)*(short *)((int)param_1 + 0x1a6) & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined1 *)puVar1 = 0;
    puVar1 = (undefined4 *)((int)puVar1 + 1);
  }
  *param_1 = &PTR_FUN_005a5ae8;
  ExceptionList = local_c;
  return param_1;
}


