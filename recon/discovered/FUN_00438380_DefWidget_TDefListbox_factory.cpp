// FUN_00438380_DefWidget_TDefListbox_factory @ 00438380 size=106

undefined4 __thiscall FUN_00438380_DefWidget_TDefListbox_factory(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ccdb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  iVar1 = FUN_00482fb0(0x1d0,param_1);
  local_4 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_00430090_DefWidget_TDefListbox_ctor(param_2,param_3,0);
    ExceptionList = local_c;
    return uVar2;
  }
  ExceptionList = local_c;
  return 0;
}


