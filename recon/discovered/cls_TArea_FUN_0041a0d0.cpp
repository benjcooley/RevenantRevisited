// FUN_0041a0d0 @ 0041a0d0 size=121

undefined4 * FUN_0041a0d0(undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059c89b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  puVar1 = (undefined4 *)FUN_00482fb0(0x14);
  local_4 = 0;
  if (puVar1 != (undefined4 *)0x0) {
    FUN_00446210(param_1,param_2);
    *puVar1 = &PTR_FUN_005a384c;
    puVar1[3] = 0;
    ExceptionList = local_c;
    return puVar1;
  }
  ExceptionList = local_c;
  return (undefined4 *)0x0;
}


