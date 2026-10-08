// FUN_0042d6c0 @ 0042d6c0 size=110

void __thiscall FUN_0042d6c0(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  if ((param_1[0x26] != 0) && ((param_1[5] & 0x40000U) == 0)) {
    iVar1 = (**(code **)(*param_1 + 0x54))(param_3,param_4);
    if (iVar1 != 0) {
      (**(code **)(*param_1 + 0x1c))(param_1[5] | 0x10000);
      (**(code **)(*param_1 + 0x1c))(param_1[5] | 0x20);
      return;
    }
    (**(code **)(*param_1 + 0x1c))(param_1[5] & 0xfffeffff);
    (**(code **)(*param_1 + 0x1c))(param_1[5] | 0x20);
  }
  return;
}


