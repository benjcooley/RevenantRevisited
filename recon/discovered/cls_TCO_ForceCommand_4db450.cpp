// FUN_004db450_TComplexObject_ForceCommand @ 004db450 size=115

undefined4 __thiscall FUN_004db450_TComplexObject_ForceCommand(int *param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  if (param_2 == 0) {
    param_2 = param_1[0x38];
  }
  iVar1 = param_1[0x36];
  if ((((*(uint *)(iVar1 + 0x60) & 0x10) == 0) || (param_1[0x37] == iVar1)) &&
     ((((param_1[0x20] != 0 || (iVar1 == 0)) ||
       ((param_2 != iVar1 && ((*(uint *)(iVar1 + 0x60) & 0x40) != 0)))) ||
      ((iVar1 = param_1[0x37], iVar1 != 0 &&
       ((param_2 != iVar1 && ((*(byte *)(iVar1 + 0x60) & 0x20) != 0)))))))) {
    uVar2 = (**(code **)(*param_1 + 0x218))(param_2,param_3,param_4);
    return uVar2;
  }
  return 2;
}


