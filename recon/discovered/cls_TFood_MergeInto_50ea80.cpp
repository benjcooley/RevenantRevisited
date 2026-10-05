// FUN_0050ea80 @ 0050ea80 size=90

undefined4 __thiscall FUN_0050ea80(int *param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  FUN_0046fee0(param_2);
  piVar1 = (int *)(**(code **)(*param_2 + 0xa8))(*(undefined4 *)param_1[0x13]);
  if (piVar1 != (int *)0x0) {
    iVar2 = (**(code **)(*piVar1 + 0x198))();
    iVar3 = (**(code **)(*param_1 + 0x198))();
    (**(code **)(*piVar1 + 0x19c))(iVar2 + iVar3);
    return 1;
  }
  return 0;
}


