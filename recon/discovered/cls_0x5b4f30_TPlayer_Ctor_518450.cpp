// FUN_00518450_TPlayer_Ctor @ 00518450 size=278

undefined4 * __thiscall FUN_00518450_TPlayer_Ctor(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  uint uVar2;
  int iVar3;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1110;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_004c16b0(param_2,param_3);
  local_4 = 0;
  FUN_0041c7f0(0x40,0x40);
  local_4._0_1_ = 1;
  FUN_0041c7f0(0x40,0x40);
  param_1[0xc5] = &PTR_LAB_005b5354;
  local_4._0_1_ = 2;
  *(undefined2 *)(param_1 + 0xd3) = 0;
  *(undefined2 *)((int)param_1 + 0x34e) = 1;
  param_1[0xd4] = 0;
  puVar1 = (undefined4 *)FUN_00482fb0(8);
  param_1[0xd4] = puVar1;
  for (uVar2 = (uint)((int)*(short *)((int)param_1 + 0x34e) << 3) >> 2; uVar2 != 0;
      uVar2 = uVar2 - 1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
    *(undefined1 *)puVar1 = 0;
    puVar1 = (undefined4 *)((int)puVar1 + 1);
  }
  local_4._0_1_ = 3;
  *(undefined2 *)(param_1 + 0xd6) = 0;
  *(undefined2 *)((int)param_1 + 0x35a) = 4;
  param_1[0xd7] = 0;
  puVar1 = (undefined4 *)FUN_00482fb0(0x20);
  param_1[0xd7] = puVar1;
  for (uVar2 = (uint)((int)*(short *)((int)param_1 + 0x35a) << 3) >> 2; uVar2 != 0;
      uVar2 = uVar2 - 1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
    *(undefined1 *)puVar1 = 0;
    puVar1 = (undefined4 *)((int)puVar1 + 1);
  }
  local_4 = CONCAT31(local_4._1_3_,4);
  *param_1 = &PTR_FUN_005b4f30;
  FUN_00518750_TPlayer_ClearPlayer();
  ExceptionList = local_c;
  return param_1;
}


