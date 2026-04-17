#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4875e0



// Function at 004875e0

cls_0x4875e0::cls_0x4875e0()

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  tagMSG tStack_90;
  byte local_74 [104];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d4ab;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x10);
  DAT_0065ba08 = DAT_0065ba08 + 0x4c;
  local_4 = 0;
  if (DAT_00667fbc < DAT_0065ba08) {
    DAT_00667fbc = DAT_0065ba08;
  }
  puVar1 = (undefined4 *)_malloc(0x4c);
  puVar3 = puVar1;
  for (iVar2 = 0x13; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  if (puVar1 == (undefined4 *)0x0) {
    if (DAT_0065b9f8 != 0) {
                    /* WARNING: Subroutine does not return */
      FUN_0058d065(1);
    }
    DAT_0065b9f8 = 1;
    FUN_0058b100((char *)local_74,(byte *)s_OUT_OF_MEMORY___Increase_your_vi_005d7cd8);
    FUN_004820b0(local_74);
    FUN_004820b0((byte *)s_Press_any_key_to_exit_005d7bdc);
    DAT_00667fd0 = 0;
    cls_0x49e370::meth_0x49e410((cls_0x49e370 *)&UNK_0065c55c.field_0x60);
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x6c))();
    FUN_004a7460();
    cls_0x4489a0::meth_0x448c90((cls_0x4489a0 *)&DAT_0065b8e8);
    iVar2 = PeekMessageA(&tStack_90,(HWND)0x0,0,0,1);
    while (iVar2 != 0) {
      TranslateMessage(&tStack_90);
      DispatchMessageA(&tStack_90);
      iVar2 = PeekMessageA(&tStack_90,(HWND)0x0,0,0,1);
    }
    ShowWindow(DAT_0065b8ec,0);
    MessageBoxA((HWND)0x0,(LPCSTR)local_74,s_FATAL_ERROR_005d7bf4,0x10);
                    /* WARNING: Subroutine does not return */
    FUN_0058d065(1);
  }
  (this->cls_0x41c7f0).mbr_0x14 = (dword)puVar1;
  for (iVar2 = 0x13; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  ExceptionList = local_c;
  return this;
}



