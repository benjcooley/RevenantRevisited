// FUN_00535120 @ 00535120 size=169

void __fastcall FUN_00535120(int param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  int *piVar4;
  
  *(undefined4 *)(param_1 + 0x48) = 1;
  *(undefined4 *)(param_1 + 0x4c) = 1;
  if (*(undefined4 **)(param_1 + 0x18c) != (undefined4 *)0x0) {
    puVar3 = *(undefined4 **)(param_1 + 0x18c);
    for (uVar1 = *(uint *)(param_1 + 0x184) & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined1 *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
  }
  *(undefined4 *)(param_1 + 0x180) = 0;
  *(undefined4 *)(param_1 + 0x17c) = 0;
  *(undefined4 *)(param_1 + 400) = 0;
  if (*(int *)(param_1 + 0x40) != 0) {
    iVar2 = 0;
    if (0 < *(int *)(param_1 + 0x1d8)) {
      piVar4 = (int *)(param_1 + 0x1b8);
      do {
        if (piVar4[-8] != 0) {
          FUN_00482f80(piVar4[-8]);
        }
        if (*piVar4 != 0) {
          FUN_00482f80(*piVar4);
        }
        iVar2 = iVar2 + 1;
        piVar4 = piVar4 + 1;
      } while (iVar2 < *(int *)(param_1 + 0x1d8));
    }
    *(undefined4 *)(param_1 + 0x1d8) = 0;
    *(undefined4 *)(param_1 + 0x1dc) = 0xffffffff;
    *(undefined4 *)(param_1 + 0x1e4) = 0;
  }
  return;
}


