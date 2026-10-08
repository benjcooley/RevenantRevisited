// FUN_004f1000 @ 004f1000 size=147

void __thiscall FUN_004f1000(int param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 *puVar5;
  
  FUN_00472980(param_2);
  uVar1 = *(undefined4 *)(param_1 + 0x184);
  if ((*(int *)(param_2 + 0xc) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 4) < 4) {
    FUN_0049cc70(4);
  }
  puVar5 = *(undefined4 **)(param_2 + 8);
  iVar2 = *(int *)(param_2 + 0xc);
  iVar3 = *(int *)(param_2 + 4);
  *puVar5 = uVar1;
  uVar1 = *(undefined4 *)(param_1 + 0x188);
  puVar5 = puVar5 + 1;
  *(undefined4 **)(param_2 + 8) = puVar5;
  if ((iVar2 - (int)puVar5) + iVar3 < 4) {
    FUN_0049cc70(4);
  }
  puVar5 = *(undefined4 **)(param_2 + 8);
  iVar2 = *(int *)(param_2 + 0xc);
  iVar3 = *(int *)(param_2 + 4);
  uVar4 = *(undefined4 *)(param_1 + 0x18c);
  *puVar5 = uVar1;
  puVar5 = puVar5 + 1;
  *(undefined4 **)(param_2 + 8) = puVar5;
  if ((iVar2 - (int)puVar5) + iVar3 < 4) {
    FUN_0049cc70(4);
  }
  puVar5 = *(undefined4 **)(param_2 + 8);
  *puVar5 = uVar4;
  *(undefined4 **)(param_2 + 8) = puVar5 + 1;
  return;
}


