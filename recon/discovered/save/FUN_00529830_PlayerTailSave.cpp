// FUN_00529830 @ 00529830 size=320

void __thiscall FUN_00529830(int param_1,int param_2)

{
  uint *puVar1;
  undefined2 uVar2;
  uint uVar3;
  uint *puVar4;
  undefined4 uVar5;
  undefined2 *puVar6;
  undefined4 *puVar7;
  int iVar8;
  int *piVar9;
  int iVar10;
  uint local_4;
  
  if (DAT_0065b550 == param_1) {
    FUN_0052c5c0();
  }
  FUN_0049ccc0(param_1 + 4);
  puVar1 = (uint *)(param_1 + 0x24);
  uVar3 = *puVar1;
  if ((*(int *)(param_2 + 0xc) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 4) < 4) {
    FUN_0049cc70(4);
  }
  puVar4 = *(uint **)(param_2 + 8);
  local_4 = 0;
  *puVar4 = uVar3;
  *(uint **)(param_2 + 8) = puVar4 + 1;
  piVar9 = *(int **)(param_1 + 0x34);
  for (; (puVar1 != (uint *)0x0 && (local_4 < *puVar1)); local_4 = local_4 + 1) {
    FUN_0049cc70(*(int *)(*piVar9 + 4) * 2 + 8);
    uVar5 = *(undefined4 *)*piVar9;
    if ((*(int *)(param_2 + 0xc) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 4) < 4) {
      FUN_0049cc70(4);
    }
    puVar7 = *(undefined4 **)(param_2 + 8);
    iVar8 = *piVar9;
    iVar10 = *(int *)(param_2 + 0xc);
    *puVar7 = uVar5;
    uVar5 = *(undefined4 *)(iVar8 + 4);
    puVar7 = puVar7 + 1;
    *(undefined4 **)(param_2 + 8) = puVar7;
    if ((iVar10 - (int)puVar7) + *(int *)(param_2 + 4) < 4) {
      FUN_0049cc70(4);
    }
    puVar7 = *(undefined4 **)(param_2 + 8);
    *puVar7 = uVar5;
    *(undefined4 **)(param_2 + 8) = puVar7 + 1;
    iVar8 = *piVar9;
    iVar10 = 0;
    if (0 < *(int *)(iVar8 + 4)) {
      do {
        uVar2 = *(undefined2 *)(*(int *)(iVar8 + 8) + iVar10 * 2);
        if ((*(int *)(param_2 + 0xc) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 4) < 2) {
          FUN_0049cc70(2);
        }
        puVar6 = *(undefined2 **)(param_2 + 8);
        *puVar6 = uVar2;
        *(undefined2 **)(param_2 + 8) = puVar6 + 1;
        iVar8 = *piVar9;
        iVar10 = iVar10 + 1;
      } while (iVar10 < *(int *)(iVar8 + 4));
    }
    piVar9 = piVar9 + 1;
  }
  return;
}


