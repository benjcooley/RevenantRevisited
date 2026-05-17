// FUN_00462660_LoadDef_connect @ 00462660 size=182

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00462660_LoadDef_connect(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  FUN_00578210();
  FUN_00434e40_TButtonPane_Initialize();
  *(undefined4 *)(param_1 + 0x17c) = 0;
  FUN_0048d260(1);
  iVar1 = FUN_004a1ec0(0x14,0x14,*(uint *)(PTR_DAT_005d79e0 + 0x38) & 0x3001f,0);
  uVar2 = _DAT_006668d0;
  *(int *)(param_1 + 0x1b4) = iVar1;
  *(undefined4 *)(iVar1 + 0x18) = uVar2;
  iVar1 = FUN_0047f670(s_connect_dat_005d2b64,0xffffffff,0);
  if (iVar1 == 0) {
    return 0;
  }
  FUN_00435c60(iVar1,1);
  uVar2 = FUN_0046d710(s_background_005d2b70);
  FUN_00435660(uVar2);
  FUN_00435ad0(DAT_0065bb10,0);
  FUN_004377c0_DefWidget_LoadFile(s_connect_def_005d2b7c);
  FUN_00437620_DefWidget_ParseBuffer();
  return 1;
}


