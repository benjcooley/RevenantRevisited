// FUN_00428690 @ 00428690 size=299

undefined4 FUN_00428690(void)

{
  int iVar1;
  tagRECT tStack_38;
  WNDCLASSA WStack_28;
  
  WStack_28.style = 3;
  WStack_28.lpfnWndProc = (WNDPROC)&LAB_00428670;
  WStack_28.cbClsExtra = 0;
  WStack_28.cbWndExtra = 0;
  WStack_28.hInstance = DAT_00654e88;
  WStack_28.hIcon = LoadIconA(DAT_00654e88,&DAT_00000064);
  WStack_28.hCursor = (HCURSOR)0x0;
  WStack_28.hbrBackground = (HBRUSH)0x0;
  WStack_28.lpszMenuName = (LPCSTR)0x0;
  WStack_28.lpszClassName = s_REVENANTEditorClass_005ccb8c;
  RegisterClassA(&WStack_28);
  tStack_38.left = (DAT_005d7a4c + -0x280) / 2 + DAT_00668138;
  tStack_38.top = (DAT_005d7a50 + -0x1e0) / 2 + DAT_0066813c;
  tStack_38.bottom = tStack_38.top + 100;
  tStack_38.right = tStack_38.left + 100;
  AdjustWindowRectEx(&tStack_38,0xc80000,1,0x300);
  iVar1 = GetSystemMetrics(0xf);
  tStack_38.bottom = tStack_38.bottom - iVar1;
  DAT_00654e8c = CreateWindowExA(0x300,s_REVENANTEditorClass_005ccbb0,s_Revenant_Editor_005ccba0,
                                 0xc80000,tStack_38.left,tStack_38.top,
                                 tStack_38.right - tStack_38.left,tStack_38.bottom - tStack_38.top,
                                 (HWND)0x0,(HMENU)0x0,DAT_00654e88,(LPVOID)0x0);
  ShowWindow(DAT_00654e8c,10);
  UpdateWindow(DAT_00654e8c);
  SetFocus(DAT_00654e8c);
  return 0;
}


