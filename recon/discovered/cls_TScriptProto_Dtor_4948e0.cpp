// FUN_004948e0_TScriptProto_Dtor @ 004948e0 size=367

void __fastcall FUN_004948e0_TScriptProto_Dtor(int *param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  void *local_c;
  undefined1 *puStack_8;
  uint local_4;
  
  puStack_8 = &LAB_0059d9f5;
  local_c = ExceptionList;
  iVar4 = 0;
  local_4 = 1;
  ExceptionList = &local_c;
  if (*param_1 != 0) {
    ExceptionList = &local_c;
    FUN_00482f80(*param_1);
  }
  if (param_1[0xf] != 0) {
    FUN_00482f80(param_1[0xf]);
  }
  if (param_1[1] != 0) {
    FUN_00482f80(param_1[1]);
  }
  piVar1 = param_1 + 9;
  *param_1 = 0;
  param_1[0xf] = 0;
  param_1[1] = 0;
  if (0 < *piVar1) {
    do {
      if ((-1 < iVar4) && (iVar3 = *(int *)(param_1[0xd] + iVar4 * 4), iVar3 != 0)) {
        if (*(int *)(iVar3 + 4) != 0) {
          FUN_004830f0(*(int *)(iVar3 + 4));
        }
        *(undefined4 *)(iVar3 + 4) = 0;
        FUN_004830f0(iVar3);
      }
      FUN_0041cb40(iVar4);
      iVar4 = iVar4 + 1;
    } while (iVar4 < *piVar1);
  }
  piVar2 = param_1 + 3;
  *piVar1 = 0;
  iVar4 = 0;
  param_1[10] = 0;
  if (0 < *piVar2) {
    do {
      if ((-1 < iVar4) && (iVar3 = *(int *)(param_1[7] + iVar4 * 4), iVar3 != 0)) {
        FUN_004830f0(iVar3);
      }
      FUN_0041cb40(iVar4);
      iVar4 = iVar4 + 1;
    } while (iVar4 < *piVar2);
  }
  *piVar2 = 0;
  param_1[4] = 0;
  local_4 = local_4 & 0xffffff00;
  FUN_004830f0(param_1[0xd]);
  iVar4 = 0;
  local_4 = 2;
  if (0 < *piVar2) {
    do {
      if ((-1 < iVar4) && (iVar3 = *(int *)(param_1[7] + iVar4 * 4), iVar3 != 0)) {
        FUN_004830f0(iVar3);
      }
      FUN_0041cb40(iVar4);
      iVar4 = iVar4 + 1;
    } while (iVar4 < *piVar2);
  }
  *piVar2 = 0;
  param_1[4] = 0;
  FUN_004830f0(param_1[8]);
  local_4 = 0xffffffff;
  FUN_004830f0(param_1[7]);
  ExceptionList = local_c;
  return;
}


