// FUN_004d56c0 @ 004d56c0 size=108

void __thiscall FUN_004d56c0(int *param_1,int param_2)

{
  int iVar1;
  
  param_1[0x68] = param_2;
  if (param_2 == 1) {
    iVar1 = (**(code **)(*param_1 + 0x1c0))();
    if (0 < iVar1) {
      param_1[0x66] = -5;
      param_1[0x67] = 100;
      param_1[0x68] = 1;
      return;
    }
  }
  else {
    (**(code **)(*param_1 + 0x1c0))();
    param_1[0x66] = 5;
    param_1[0x67] = 0;
    param_1[0x68] = -1;
  }
  return;
}


