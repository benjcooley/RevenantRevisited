// FUN_004c16b0 @ 004c16b0 size=189

undefined4 * __thiscall FUN_004c16b0(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059e2ae;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0046e1f0(param_2,param_3);
  local_4 = 0;
  *param_1 = &PTR_FUN_005a7b98;
  param_1[0x37] = 0;
  param_1[0x36] = 0;
  param_1[0x38] = 0;
  FUN_004db000();
  local_4 = 1;
  FUN_0041c7f0(0x20,0x10);
  if ((undefined4 *)param_1[0x60] != (undefined4 *)0x0) {
    puVar3 = (undefined4 *)param_1[0x60];
    for (uVar1 = param_1[0x5e] & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined1 *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
  }
  param_1[0x5d] = 0;
  param_1[0x5c] = 0;
  param_1[0x61] = 0;
  local_4 = CONCAT31(local_4._1_3_,2);
  *param_1 = &PTR_FUN_005a7848;
  FUN_004c18a0();
  ExceptionList = local_c;
  return param_1;
}


