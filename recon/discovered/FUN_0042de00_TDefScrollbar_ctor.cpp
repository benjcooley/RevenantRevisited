// FUN_0042de00_DefWidget_TDefScrollbar_ctor @ 0042de00 size=242

undefined4 * __thiscall
FUN_0042de00_DefWidget_TDefScrollbar_ctor(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8,
            undefined4 param_9,undefined4 param_10,undefined4 *param_11)

{
  int iVar1;
  undefined4 *puVar2;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ca94;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0042a210(param_2,5,param_7,param_8,param_9,0,param_3,param_4,param_5,param_6,param_10,0,
               0xffffffff,0,0,0,0xffffffff,0);
  local_4 = 0;
  *param_1 = &PTR_FUN_005a3cd8;
  param_1[0x24] = 0;
  param_1[0x25] = 0;
  param_1[0x26] = 0;
  param_1[0x27] = 1000;
  param_1[0x28] = 100;
  if ((param_1[5] & 1) == 0) {
    if ((param_11 == (undefined4 *)0x0) && (param_11 = &DAT_00655d38, (param_1[5] & 0x20000) == 0))
    {
      param_11 = &DAT_006560f8;
    }
    puVar2 = param_1 + 0x29;
    for (iVar1 = 0x21; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar2 = *param_11;
      param_11 = param_11 + 1;
      puVar2 = puVar2 + 1;
    }
    FUN_0042e240();
  }
  ExceptionList = local_c;
  return param_1;
}


