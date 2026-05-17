// FUN_004385c0_DefWidget_TDefEdit_factory @ 004385c0 size=106

undefined4 __thiscall FUN_004385c0_DefWidget_TDefEdit_factory(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ccfb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  iVar1 = FUN_00482fb0(0x148,param_1);
  local_4 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_00431ca0_DefWidget_TDefEdit_ctor(param_2,param_3,0);
    ExceptionList = local_c;
    return uVar2;
  }
  ExceptionList = local_c;
  return 0;
}


