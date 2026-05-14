// FUN_0041bf00_TAreaMgr_Initialize @ 0041bf00 size=159

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0041bf00_TAreaMgr_Initialize(int *param_1)

{
  int iVar1;
  int iVar2;
  
  if (*param_1 == 0) {
    iVar2 = 0;
    if (0 < param_1[1]) {
      do {
        if ((-1 < iVar2) && (iVar1 = *(int *)(param_1[5] + iVar2 * 4), iVar1 != 0)) {
          FUN_0041a3b0_TArea_Dtor();
          FUN_004830f0(iVar1);
        }
        FUN_0041cb40(iVar2);
        iVar2 = iVar2 + 1;
      } while (iVar2 < param_1[1]);
    }
    DAT_005e91f0 = 0xfffe7960;
    DAT_005e91f4 = 0xfffe7960;
    _DAT_005e91f8 = 0xfffe7960;
    param_1[1] = 0;
    param_1[2] = 0;
    DAT_005e91ec = 0xff;
    _DAT_005e91fc = 0xffffffff;
    DAT_005e91e8 = 0xffffffff;
    iVar2 = FUN_0041c000_TAreaMgr_Load();
    if (iVar2 == 0) {
      return 0;
    }
    *param_1 = 1;
  }
  return 1;
}


