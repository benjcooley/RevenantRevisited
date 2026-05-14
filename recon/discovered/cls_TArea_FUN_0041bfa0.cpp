// FUN_0041bfa0_TAreaMgr_Close @ 0041bfa0 size=81

void __fastcall FUN_0041bfa0_TAreaMgr_Close(undefined4 *param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)param_1[1]) {
    do {
      if ((-1 < iVar2) && (iVar1 = *(int *)(param_1[5] + iVar2 * 4), iVar1 != 0)) {
        FUN_0041a3b0_TArea_Dtor();
        FUN_004830f0(iVar1);
      }
      FUN_0041cb40(iVar2);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)param_1[1]);
  }
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[6] = 0;
  *param_1 = 0;
  return;
}


