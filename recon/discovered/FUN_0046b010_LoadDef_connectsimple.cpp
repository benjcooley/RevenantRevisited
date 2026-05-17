// FUN_0046b010_LoadDef_connectsimple @ 0046b010 size=254

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0046b010_LoadDef_connectsimple(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  FUN_00434e40();
  iVar1 = DAT_00676828;
  *(undefined4 *)(param_1 + 0x1cc) = 0;
  if ((iVar1 == 0) || (DAT_00676830 == 0)) {
    FUN_00578210();
  }
  *(undefined4 *)(param_1 + 0x1c4) = 0xffffffff;
  FUN_00575ed0(0);
  iVar1 = FUN_004a1ec0(0x14,0x14,*(uint *)(PTR_DAT_005d79e0 + 0x38) & 0x3001f,0);
  uVar2 = _DAT_006668d0;
  *(int *)(param_1 + 0x1c8) = iVar1;
  *(undefined4 *)(iVar1 + 0x18) = uVar2;
  iVar1 = FUN_0047f670(s_connect_dat_005d409c,0xffffffff,0);
  if (iVar1 == 0) {
    return 0;
  }
  FUN_00435c60(iVar1,1);
  uVar2 = FUN_0046d710(s_background_005d40a8);
  FUN_00435660(uVar2);
  FUN_00435ad0(DAT_0065bb10,0);
  iVar1 = FUN_00577c10();
  if (iVar1 == 0) {
    *(undefined4 *)(param_1 + 0x1b8) = 0;
  }
  else {
    *(undefined4 *)(param_1 + 0x1b8) = 1;
  }
  *(undefined4 *)(param_1 + 0x1ac) = 0;
  FUN_004377c0_DefWidget_LoadFile(s_connectsimple_def_005d40b4);
  FUN_00437620_DefWidget_ParseBuffer();
  return 1;
}


