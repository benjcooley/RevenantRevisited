// FUN_004db3a0_TComplexObject_SetDesired @ 004db3a0 size=171

undefined4 __thiscall FUN_004db3a0_TComplexObject_SetDesired(int *param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  
  if (param_2 == 0) {
    param_2 = param_1[0x38];
  }
  iVar1 = param_1[0x37];
  if (((iVar1 != 0) && (iVar1 != param_1[0x36])) && ((*(byte *)(param_1[0x36] + 0x60) & 0x10) != 0))
  {
    return 0;
  }
  if (param_2 != iVar1) {
    if (((iVar1 != 0) && (iVar1 != param_1[0x36])) && ((iVar1 != param_1[0x38] && (iVar1 != 0)))) {
      if (*(int *)(iVar1 + 0x5c) != 0) {
        FUN_00482f80(*(int *)(iVar1 + 0x5c));
      }
      FUN_004830f0(iVar1);
    }
    param_1[0x37] = param_2;
  }
  if ((((*(byte *)(param_2 + 0x60) & 0x20) != 0) && (param_1[0x36] != param_2)) &&
     ((*(byte *)(param_1[0x36] + 0x60) & 0x10) == 0)) {
    (**(code **)(*param_1 + 0x218))(param_2,0,param_3);
    *(uint *)(param_2 + 0x60) = *(uint *)(param_2 + 0x60) & 0xffffffdf;
  }
  return 1;
}


