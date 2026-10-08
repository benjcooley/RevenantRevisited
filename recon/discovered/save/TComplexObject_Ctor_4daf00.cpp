// FUN_004daf00 @ 004daf00 size=246

void __fastcall FUN_004daf00(undefined4 *param_1)

{
  int iVar1;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ec38;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *param_1 = &PTR_FUN_005a7b98;
  local_4 = 0;
  if ((param_1[2] & 0x80000000) == 0) {
    FUN_0046e630();
  }
  iVar1 = param_1[0x38];
  if ((((iVar1 != 0) && (iVar1 != param_1[0x37])) && (iVar1 != param_1[0x36])) && (iVar1 != 0)) {
    if (*(int *)(iVar1 + 0x5c) != 0) {
      FUN_00482f80(*(int *)(iVar1 + 0x5c));
    }
    FUN_004830f0(iVar1);
  }
  iVar1 = param_1[0x36];
  if (((iVar1 != 0) && (iVar1 != param_1[0x37])) && (iVar1 != 0)) {
    if (*(int *)(iVar1 + 0x5c) != 0) {
      FUN_00482f80(*(int *)(iVar1 + 0x5c));
    }
    FUN_004830f0(iVar1);
  }
  iVar1 = param_1[0x37];
  if (iVar1 != 0) {
    if (*(int *)(iVar1 + 0x5c) != 0) {
      FUN_00482f80(*(int *)(iVar1 + 0x5c));
    }
    FUN_004830f0(iVar1);
  }
  param_1[0x37] = 0;
  param_1[0x36] = 0;
  param_1[0x38] = 0;
  local_4 = 0xffffffff;
  FUN_0046e420();
  ExceptionList = local_c;
  return;
}


