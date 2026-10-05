// FUN_004d5730 @ 004d5730 size=85

void __thiscall FUN_004d5730(int *param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  
  iVar1 = (**(code **)(*param_1 + 0x1c0))();
  if ((0 < iVar1) || (-1 < param_3)) {
    if (-1 < param_2) {
      param_1[0x65] = param_2;
    }
    param_1[0x66] = param_3;
    param_1[0x67] = param_4;
    if (0 < param_3) {
      param_1[0x68] = -1;
      return;
    }
    param_1[0x68] = 1;
  }
  return;
}


