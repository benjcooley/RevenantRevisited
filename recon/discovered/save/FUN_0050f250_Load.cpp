// FUN_0050f250 @ 0050f250 size=125

void __thiscall FUN_0050f250(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  
  FUN_00472430(param_2,param_3,param_4);
  puVar1 = *(undefined4 **)(param_2 + 4);
  uVar2 = puVar1[1];
  *(undefined4 *)(param_1 + 0xd8) = *puVar1;
  iVar3 = puVar1[2];
  *(undefined4 *)(param_1 + 0xdc) = uVar2;
  iVar4 = *(int *)(param_1 + 0x10);
  *(undefined4 **)(param_2 + 4) = puVar1 + 3;
  *(int *)(param_1 + 0xe4) = *(int *)(param_1 + 0xd8) - iVar4;
  *(int *)(param_1 + 0xe0) = iVar3;
  *(int *)(param_1 + 0xe8) = *(int *)(param_1 + 0xdc) - *(int *)(param_1 + 0x14);
  *(int *)(param_1 + 0xec) = iVar3 - *(int *)(param_1 + 0x18);
  FUN_00497c40(*(undefined4 *)(param_1 + 0x38));
  return;
}


