// FUN_0049a0a0 @ 0049a0a0 size=86

int __fastcall FUN_0049a0a0(int param_1)

{
  undefined4 *puVar1;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059db88;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0041c7f0(0,0x1000);
  local_4 = 0;
  puVar1 = (undefined4 *)FUN_00482fb0(4);
  *(undefined4 **)(param_1 + 0x14) = puVar1;
  *puVar1 = 0;
  ExceptionList = local_c;
  return param_1;
}


