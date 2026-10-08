// FUN_00534fd0 @ 00534fd0 size=142

undefined4 __fastcall FUN_00534fd0(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  uint uVar3;
  
  if (*(int *)(param_1 + 0x40) != 0) {
    return 1;
  }
  iVar1 = FUN_00434e40();
  if (iVar1 == 0) {
    return 0;
  }
  *(undefined4 *)(param_1 + 0x1dc) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x1e4) = 0;
  puVar2 = (undefined4 *)(param_1 + 0x198);
  iVar1 = 8;
  do {
    puVar2[8] = 0;
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  if (*(undefined4 **)(param_1 + 0x18c) != (undefined4 *)0x0) {
    puVar2 = *(undefined4 **)(param_1 + 0x18c);
    for (uVar3 = *(uint *)(param_1 + 0x184) & 0x3fffffff; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar2 = 0;
      puVar2 = puVar2 + 1;
    }
    for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined1 *)puVar2 = 0;
      puVar2 = (undefined4 *)((int)puVar2 + 1);
    }
  }
  *(undefined4 *)(param_1 + 0x180) = 0;
  *(undefined4 *)(param_1 + 0x17c) = 0;
  *(undefined4 *)(param_1 + 400) = 0;
  *(uint *)(param_1 + 0x60) = *(uint *)(param_1 + 0x60) | 2;
  return 1;
}


