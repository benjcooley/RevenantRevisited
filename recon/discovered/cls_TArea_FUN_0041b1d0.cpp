// FUN_0041b1d0_TArea_InitCDMusic @ 0041b1d0 size=76

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0041b1d0_TArea_InitCDMusic(int param_1)

{
  int iVar1;
  DWORD DVar2;
  
  *(undefined4 *)(param_1 + 0x1b8) = 0xffffffff;
  DVar2 = GetTickCount();
  *(DWORD *)(param_1 + 0x248) = DVar2;
  iVar1 = DAT_0065abd8;
  *(undefined4 *)(param_1 + 0x250) = 100;
  *(undefined4 *)(param_1 + 0x254) = 0;
  if (iVar1 == 0) {
    DAT_0065abd8 = 8;
    return;
  }
  _DAT_0065abdc = 8;
  return;
}


