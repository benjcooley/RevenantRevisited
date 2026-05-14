// FUN_004da870 @ 004da870 size=90

undefined4 * FUN_004da870(undefined4 param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059eaab;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  puVar1 = (undefined4 *)FUN_00482fb0(0xf7c);
  local_4 = 0;
  puVar2 = (undefined4 *)0x0;
  if (puVar1 != (undefined4 *)0x0) {
    FUN_004d76b0(param_1);
    *puVar1 = &PTR_FUN_005a7dd8;
    puVar2 = puVar1;
  }
  ExceptionList = local_c;
  return puVar2;
}


