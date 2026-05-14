// FUN_004db1d0_TComplexObject_UpdateAction @ 004db1d0 size=214

void __thiscall FUN_004db1d0_TComplexObject_UpdateAction(int *param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  
  iVar2 = (**(code **)(*param_1 + 0x21c))(param_2);
  if (iVar2 == 0) {
    iVar2 = (-(uint)(param_1[0x20] != 0) & 0xfffffffe) + 2;
  }
  iVar1 = param_1[0x36];
  if (iVar1 != 0) {
    if ((*(uint *)(iVar1 + 0x60) & 1) != 0) {
      *(uint *)(iVar1 + 0x60) = *(uint *)(iVar1 + 0x60) & 0xfffffffe;
    }
    if ((iVar2 == 0) && ((*(uint *)(iVar1 + 0x60) & 0x10) != 0)) {
      *(uint *)(iVar1 + 0x60) = *(uint *)(iVar1 + 0x60) & 0xffffffef;
    }
  }
  iVar1 = param_1[0x37];
  if (iVar1 != 0) {
    if (iVar1 == param_1[0x38]) goto LAB_004db247;
    iVar2 = (**(code **)(*param_1 + 0x214))(iVar1,param_2,0);
  }
  if ((iVar2 != 0) && (iVar2 != 3)) {
    return;
  }
LAB_004db247:
  iVar2 = param_1[0x37];
  if ((((iVar2 != 0) && (iVar2 != param_1[0x36])) && (iVar2 != param_1[0x38])) && (iVar2 != 0)) {
    if (*(int *)(iVar2 + 0x5c) != 0) {
      FUN_00482f80(*(int *)(iVar2 + 0x5c));
    }
    FUN_004830f0(iVar2);
  }
  (**(code **)(*param_1 + 0x208))(0,0);
  (**(code **)(*param_1 + 0x214))(param_1[0x38],0,0);
  return;
}


