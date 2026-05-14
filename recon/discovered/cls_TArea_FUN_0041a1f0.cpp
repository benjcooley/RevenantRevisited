// FUN_0041a1f0_TArea_Ctor @ 0041a1f0 size=441

/* WARNING: Removing unreachable block (ram,0x0041a2ca) */

undefined1 * __thiscall FUN_0041a1f0_TArea_Ctor(undefined1 *param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  uint uVar2;
  int iVar3;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059c8c9;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *(undefined2 *)(param_1 + 0x48) = 0;
  *(undefined2 *)(param_1 + 0x4a) = 4;
  *(undefined4 *)(param_1 + 0x4c) = 0;
  puVar1 = (undefined4 *)FUN_00482fb0(0x40);
  *(undefined4 **)(param_1 + 0x4c) = puVar1;
  for (uVar2 = (uint)((int)*(short *)(param_1 + 0x4a) << 4) >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  }
  for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
    *(undefined1 *)puVar1 = 0;
    puVar1 = (undefined4 *)((int)puVar1 + 1);
  }
  local_4 = 0;
  FUN_0041c7f0(8,8);
  local_4 = CONCAT31(local_4._1_3_,1);
  *(undefined4 *)(param_1 + 0x44) = param_2;
  *param_1 = 0;
  if (*(int *)(param_1 + 0x4c) != 0) {
    FUN_004830f0(*(int *)(param_1 + 0x4c));
  }
  *(undefined4 *)(param_1 + 0x4c) = 0;
  param_1[0xd8] = 0xff;
  param_1[0xd9] = 0xff;
  param_1[0xda] = 0xff;
  param_1[0xdc] = 0xff;
  param_1[0xdd] = 0xff;
  param_1[0xde] = 0xff;
  *(undefined2 *)(param_1 + 0x48) = 0;
  *(undefined2 *)(param_1 + 0x4a) = 0;
  param_1[0x50] = 0;
  *(undefined4 *)(param_1 + 0xd0) = 0x1e;
  *(undefined4 *)(param_1 + 0xd4) = 10;
  *(undefined4 *)(param_1 + 0x40) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x4c) = 0;
  *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x6200;
  *(undefined2 *)(param_1 + 0x48) = 0;
  *(undefined2 *)(param_1 + 0x4a) = 0;
  *(undefined4 *)(param_1 + 0x1b8) = 0;
  *(undefined4 *)(param_1 + 0x1bc) = 0;
  *(undefined4 *)(param_1 + 0x240) = 0;
  *(undefined4 *)(param_1 + 0x244) = 0;
  *(undefined4 *)(param_1 + 600) = 0;
  *(undefined4 *)(param_1 + 0x25c) = 0;
  *(undefined4 *)(param_1 + 0xec) = 0;
  *(undefined4 *)(param_1 + 0xe8) = 0;
  *(undefined4 *)(param_1 + 0xe4) = 0;
  *(undefined4 *)(param_1 + 0xf8) = 0;
  *(undefined4 *)(param_1 + 0xf4) = 0;
  *(undefined4 *)(param_1 + 0xf0) = 0;
  *(undefined4 *)(param_1 + 0xfc) = 0x43a00000;
  *(undefined4 *)(param_1 + 0x100) = 0x407b53d1;
  *(undefined4 *)(param_1 + 0x104) = 0x3f490fdb;
  *(undefined4 *)(param_1 + 0x120) = 0x2d;
  *(undefined4 *)(param_1 + 0x108) = 0x42480000;
  *(undefined4 *)(param_1 + 0x10c) = 0x44160000;
  *(undefined4 *)(param_1 + 0x110) = 0xd368d4a5;
  *(undefined4 *)(param_1 + 0x114) = 0x5368d4a5;
  *(undefined4 *)(param_1 + 0x118) = 0xbf060a92;
  *(undefined4 *)(param_1 + 0x11c) = 0x3fb2b8c2;
  ExceptionList = local_c;
  return param_1;
}


