// FUN_004f47a0 @ 004f47a0 size=114

void __thiscall FUN_004f47a0(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  int iVar3;
  
  FUN_00472430(param_2,param_3,param_4);
  iVar3 = FUN_0049ce00(param_1 + 0x184);
  puVar1 = *(undefined4 **)(iVar3 + 4);
  *(undefined4 *)(param_1 + 0x1a8) = *puVar1;
  *(undefined4 *)(param_1 + 0x1ac) = puVar1[1];
  uVar2 = puVar1[2];
  *(undefined4 **)(iVar3 + 4) = puVar1 + 3;
  *(undefined4 *)(param_1 + 0x1b0) = uVar2;
  iVar3 = FUN_0049c430(param_1 + 0x184);
  *(int *)(param_1 + 0x1a4) = iVar3;
  if (-1 < iVar3) {
    FUN_0049b650(iVar3);
  }
  return;
}


