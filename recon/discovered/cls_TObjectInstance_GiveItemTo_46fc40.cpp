// FUN_0046fc40 @ 0046fc40 size=237

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __thiscall FUN_0046fc40(int param_1,int *param_2,int *param_3,int param_4)

{
  int iVar1;
  int *unaff_retaddr;
  
  if (param_4 < 1) {
    return 0;
  }
  if (param_3 == (int *)0x0) {
    return 0;
  }
  iVar1 = (**(code **)(*param_3 + 0x198))();
  if (iVar1 < 2) {
    iVar1 = 1;
  }
  else {
    iVar1 = (**(code **)(*param_3 + 0x198))();
  }
  if (iVar1 <= param_4) {
    (**(code **)(*param_3 + 0x60))();
    if (param_2 != (int *)0x0) {
      (**(code **)(*param_2 + 0x58))(param_3,0xffffffff);
      return iVar1;
    }
    (**(code **)*param_3)(1);
    return iVar1;
  }
  (**(code **)(*param_3 + 0x19c))(iVar1 - param_4);
  if (unaff_retaddr != (int *)0x0) {
    iVar1 = (**(code **)(*unaff_retaddr + 0x54))(*(undefined4 *)(param_1 + 0x38),param_4,0xffffffff)
    ;
    if (iVar1 == 0) {
      return 0;
    }
  }
  if (param_3[0x19] == DAT_0065d674) {
    _DAT_0065d548 = 1;
    (**(code **)(DAT_0065d4f8 + 0x90))();
  }
  if (param_3[0x19] == DAT_0065b088) {
    _DAT_0065b078 = 1;
  }
  return param_4;
}


