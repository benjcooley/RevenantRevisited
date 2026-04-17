#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4489a0



// Function at 004489a0

undefined4 cls_0x4489a0::meth_0x4489a0()

{
  int iVar1;
  HWND pHVar2;
  tagRECT tStack_38;
  WNDCLASSA local_28;
  
  if (DAT_00668190 == 1) {
    local_28.style = 3;
    local_28.lpfnWndProc = FUN_00448cc0;
    local_28.cbClsExtra = 0;
    local_28.cbWndExtra = 0;
    local_28.hInstance = DAT_00667c28;
    if (DAT_00668148 == 0) {
      local_28.hIcon = LoadIconA(DAT_00667c28,(LPCSTR)0x64);
      local_28.hCursor = LoadCursorA(DAT_00667c28,(LPCSTR)0x8b);
      local_28.hbrBackground = (HBRUSH)0x0;
      local_28.lpszMenuName = (char *)0x0;
      local_28.lpszClassName = s_REVENANTClass_005cfe74;
      RegisterClassA(&local_28);
      tStack_38.left = (DAT_005d7a4c + -0x280) / 2 + DAT_00668138;
      tStack_38.right = tStack_38.left + 0x280;
      tStack_38.top = (DAT_005d7a50 + -0x1e0) / 2 + DAT_0066813c;
      tStack_38.bottom = tStack_38.top + 0x1e0;
      AdjustWindowRectEx(&tStack_38,0xc80000,1,0x300);
      iVar1 = FUN_004805b0(0xf);
      tStack_38.bottom = tStack_38.bottom - iVar1;
      pHVar2 = CreateWindowExA(0x300,s_REVENANTClass_005cfe90,s_Revenant_005cfe84,0xc80000,
                               tStack_38.left,tStack_38.top,tStack_38.right - tStack_38.left,
                               tStack_38.bottom - tStack_38.top,(HWND)0x0,(HMENU)0x0,DAT_00667c28,
                               (LPVOID)0x0);
      this->mbr_0x4 = (dword)pHVar2;
    }
    else {
      local_28.hIcon = LoadIconA(DAT_00667c28,(LPCSTR)0x64);
      local_28.hCursor = LoadCursorA(DAT_00667c28,(LPCSTR)0x8b);
      local_28.hbrBackground = (HBRUSH)0x0;
      local_28.lpszMenuName = s_REVENANTClass_005cfea0;
      local_28.lpszClassName = s_REVENANTClass_005cfeb0;
      RegisterClassA(&local_28);
      tStack_38.left = (DAT_005d7a4c + -0x280) / 2 + DAT_00668138;
      tStack_38.right = tStack_38.left + 0x280;
      tStack_38.top = (DAT_005d7a50 + -0x1e0) / 2 + 0x13 + DAT_0066813c;
      tStack_38.bottom = tStack_38.top + 0x1e0;
      AdjustWindowRectEx(&tStack_38,0x80000000,1,0);
      pHVar2 = CreateWindowExA(0,s_REVENANTClass_005cfecc,s_Revenant_005cfec0,0x80000000,
                               tStack_38.left,tStack_38.top,tStack_38.right - tStack_38.left,
                               tStack_38.bottom - tStack_38.top,(HWND)0x0,(HMENU)0x0,DAT_00667c28,
                               (LPVOID)0x0);
      this->mbr_0x4 = (dword)pHVar2;
    }
  }
  else {
    local_28.style = 3;
    local_28.lpfnWndProc = FUN_00448cc0;
    local_28.cbClsExtra = 0;
    local_28.cbWndExtra = 0;
    local_28.hInstance = DAT_00667c28;
    local_28.hIcon = LoadIconA(DAT_00667c28,(LPCSTR)0x64);
    local_28.hCursor = (HCURSOR)0x0;
    local_28.hbrBackground = (HBRUSH)0x0;
    local_28.lpszMenuName = s_REVENANTClass_005cfedc;
    local_28.lpszClassName = s_REVENANTClass_005cfeec;
    RegisterClassA(&local_28);
    pHVar2 = CreateWindowExA(8,s_REVENANTClass_005cff08,s_Revenant_005cfefc,0x80000000,DAT_00668138,
                             DAT_0066813c,0x280,0x1e0,(HWND)0x0,(HMENU)0x0,DAT_00667c28,(LPVOID)0x0)
    ;
    this->mbr_0x4 = (dword)pHVar2;
    ShowCursor(0);
  }
  if (this->mbr_0x4 == 0) {
    FUN_00481c10((byte *)s_Couldn_t_create_main_window_005cff18);
  }
  ShowWindow((HWND)this->mbr_0x4,1);
  UpdateWindow((HWND)this->mbr_0x4);
  SetFocus((HWND)this->mbr_0x4);
  this->mbr_0x8 = (dword)local_28.hIcon;
  return 1;
}



// Function at 00448c90

void cls_0x4489a0::meth_0x448c90()

{
  if (this->mbr_0x4 != 0) {
    this->mbr_0x4 = 0;
  }
  return;
}



