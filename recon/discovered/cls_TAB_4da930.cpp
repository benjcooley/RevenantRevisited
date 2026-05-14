// FUN_004da930 @ 004da930 size=110

void __fastcall FUN_004da930(undefined4 *param_1)

{
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ec1b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *param_1 = &PTR_FUN_005a7e98;
  local_4 = 0;
  FUN_004d92d0();
  local_4 = 0xffffffff;
  if (param_1[0x2a] != 0) {
    FUN_004830f0(param_1[0x2a]);
  }
  if (param_1[0x2c] != 0) {
    FUN_004830f0(param_1[0x2c]);
  }
  ExceptionList = local_c;
  return;
}


