// FUN_0046fbe0 @ 0046fbe0 size=87

undefined4 __thiscall FUN_0046fbe0(int *param_1,int *param_2,undefined4 param_3)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  undefined4 uVar5;
  
  piVar2 = (int *)param_1[0x19];
  piVar3 = param_1;
  while (piVar1 = piVar2, piVar1 != (int *)0x0) {
    piVar3 = piVar1;
    piVar2 = (int *)piVar1[0x19];
  }
  piVar3 = (int *)(-(uint)(piVar3 != param_1) & (uint)piVar3);
  if (piVar3 == (int *)0x0) {
    piVar3 = param_1;
  }
  if ((param_2[0x19] != 0) && (iVar4 = (**(code **)(*piVar3 + 0xac))(param_2[0x10]), iVar4 != 0)) {
    uVar5 = (**(code **)(*param_2 + 0x58))(param_2,param_3);
    return uVar5;
  }
  return 0;
}


