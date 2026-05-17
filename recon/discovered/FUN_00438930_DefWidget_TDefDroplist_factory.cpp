// FUN_00438930_DefWidget_TDefDroplist_factory @ 00438930 size=106

undefined4 __thiscall FUN_00438930_DefWidget_TDefDroplist_factory(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059cd1b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  iVar1 = FUN_00482fb0(0x240,param_1);
  local_4 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_00433640_DefWidget_TDefDroplist_ctor(param_2,param_3,0);
    ExceptionList = local_c;
    return uVar2;
  }
  ExceptionList = local_c;
  return 0;
}


