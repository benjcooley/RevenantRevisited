// FUN_00434e40_TButtonPane_Initialize @ 00434e40 size=233

undefined4 __fastcall FUN_00434e40_TButtonPane_Initialize(int *param_1)

{
  undefined *puVar1;
  uint uVar2;
  int iVar3;
  undefined4 *puVar4;
  
  FUN_00491900_TPane_Initialize();
  if ((undefined4 *)param_1[0x26] != (undefined4 *)0x0) {
    puVar4 = (undefined4 *)param_1[0x26];
    for (uVar2 = param_1[0x24] & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
      *(undefined1 *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  puVar1 = PTR_DAT_005d79e0;
  param_1[0x23] = 0;
  param_1[0x22] = 0;
  param_1[0x27] = 0;
  param_1[0x29] = 0;
  param_1[0x28] = 0;
  param_1[0x18] = 0;
  param_1[0x32] = 0;
  param_1[0x33] = 0;
  *(undefined1 *)(param_1 + 0x34) = 0;
  param_1[0x5e] = 0;
  *(undefined1 *)(param_1 + 0x56) = 0;
  param_1[0x54] = 0;
  param_1[0x55] = 0;
  param_1[0x19] = (int)puVar1;
  param_1[0x1a] = 0;
  param_1[0x1b] = 0;
  param_1[0x1c] = 0;
  param_1[0x1d] = 0;
  param_1[0x1e] = 0;
  param_1[0x21] = 0;
  param_1[0x1f] = 0;
  param_1[0x20] = 0;
  param_1[0x2a] = 0;
  param_1[0x2b] = 0;
  param_1[0x2f] = 0;
  param_1[0x2e] = 0;
  param_1[0x2c] = -1;
  param_1[0x2d] = -1;
  param_1[0x30] = 5;
  param_1[0x31] = -1;
  (**(code **)(*param_1 + 0x2c))(1);
  return 1;
}


