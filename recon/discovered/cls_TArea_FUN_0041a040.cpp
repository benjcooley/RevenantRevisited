// FUN_0041a040 @ 0041a040 size=142

void __fastcall FUN_0041a040(undefined4 *param_1)

{
  int iVar1;
  int iVar2;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c878;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *param_1 = &PTR_FUN_005a393c;
  local_4 = 0;
  if (param_1[0x10] != 0) {
    iVar1 = param_1[2];
    if ((iVar1 != 0) &&
       (iVar2 = *(int *)(iVar1 + 0xc) + -1, *(int *)(iVar1 + 0xc) = iVar2, iVar2 < 1)) {
      *(undefined4 *)(iVar1 + 0xc) = 0;
      FUN_004191b0();
    }
    FUN_004162c0(param_1[0xf]);
    FUN_004459c0();
    param_1[0x10] = 0;
  }
  local_4 = 0xffffffff;
  FUN_00445990();
  ExceptionList = local_c;
  return;
}


