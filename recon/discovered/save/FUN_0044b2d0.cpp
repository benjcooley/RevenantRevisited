// FUN_0044b2d0 @ 0044b2d0 size=326

void FUN_0044b2d0(void)

{
  char cVar1;
  HWND hWnd;
  WPARAM wParam;
  int iVar2;
  char *pcVar3;
  LPARAM lParam;
  int local_2c;
  undefined4 local_28;
  LRESULT local_24;
  undefined4 local_20;
  char *local_14;
  uint local_10;
  
  hWnd = GetDlgItem(DAT_006582e0,0x44c);
  SendMessageA(hWnd,0x1009,0,0);
  lParam = 0;
  wParam = FUN_0051ee70(0);
  SendMessageA(hWnd,0x102f,wParam,lParam);
  local_2c = 0;
  iVar2 = FUN_0051ee70(0);
  if (0 < iVar2) {
    do {
      iVar2 = FUN_0051eea0(local_2c,0);
      local_24 = 1;
      local_20 = 0;
      local_28 = 1;
      local_14 = &DAT_005d0048;
      if ((*(uint *)(iVar2 + 0x36c) & 0x20000) == 0) {
        local_14 = &DAT_005d004c;
      }
      local_10 = 2;
      local_24 = SendMessageA(hWnd,0x1007,0,(LPARAM)&local_28);
      local_20 = 1;
      local_28 = 1;
      local_14 = *(char **)(iVar2 + 0x38);
      local_10 = 0xffffffff;
      pcVar3 = *(char **)(iVar2 + 0x38);
      do {
        if (local_10 == 0) break;
        local_10 = local_10 - 1;
        cVar1 = *pcVar3;
        pcVar3 = pcVar3 + 1;
      } while (cVar1 != '\0');
      local_10 = ~local_10;
      SendMessageA(hWnd,0x1006,0,(LPARAM)&local_28);
      local_14 = (char *)(iVar2 + 0x570);
      local_10 = 0xffffffff;
      local_20 = 2;
      local_28 = 1;
      pcVar3 = local_14;
      do {
        if (local_10 == 0) break;
        local_10 = local_10 - 1;
        cVar1 = *pcVar3;
        pcVar3 = pcVar3 + 1;
      } while (cVar1 != '\0');
      local_10 = ~local_10;
      SendMessageA(hWnd,0x1006,0,(LPARAM)&local_28);
      local_2c = local_2c + 1;
      iVar2 = FUN_0051ee70(0);
    } while (local_2c < iVar2);
  }
  return;
}


