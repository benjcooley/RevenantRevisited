// FUN_004f4820 @ 004f4820 size=163

void __thiscall FUN_004f4820(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *puVar6;
  
  FUN_00472980(param_2);
  iVar5 = FUN_0049ccc0(param_1 + 0x184);
  uVar1 = *(undefined4 *)(param_1 + 0x1a8);
  if ((*(int *)(iVar5 + 4) - *(int *)(iVar5 + 8)) + *(int *)(iVar5 + 0xc) < 4) {
    FUN_0049cc70(4);
  }
  puVar6 = *(undefined4 **)(iVar5 + 8);
  iVar2 = *(int *)(iVar5 + 4);
  iVar3 = *(int *)(iVar5 + 0xc);
  *puVar6 = uVar1;
  uVar1 = *(undefined4 *)(param_1 + 0x1ac);
  puVar6 = puVar6 + 1;
  *(undefined4 **)(iVar5 + 8) = puVar6;
  if ((iVar2 - (int)puVar6) + iVar3 < 4) {
    FUN_0049cc70(4);
  }
  puVar6 = *(undefined4 **)(iVar5 + 8);
  iVar2 = *(int *)(iVar5 + 4);
  iVar3 = *(int *)(iVar5 + 0xc);
  uVar4 = *(undefined4 *)(param_1 + 0x1b0);
  *puVar6 = uVar1;
  puVar6 = puVar6 + 1;
  *(undefined4 **)(iVar5 + 8) = puVar6;
  if ((iVar2 - (int)puVar6) + iVar3 < 4) {
    FUN_0049cc70(4);
  }
  puVar6 = *(undefined4 **)(iVar5 + 8);
  *puVar6 = uVar4;
  *(undefined4 **)(iVar5 + 8) = puVar6 + 1;
  return;
}


