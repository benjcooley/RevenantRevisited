// FUN_004d5880 @ 004d5880 size=124

void __thiscall FUN_004d5880(int *param_1,int param_2)

{
  int iVar1;
  
  if (param_1[0x69] != param_2) {
    if (param_2 != 0) {
      (**(code **)(*param_1 + 0x1c0))();
      param_1[0x69] = param_2;
      param_1[0x66] = 5;
      param_1[0x67] = 0x1e;
      param_1[0x68] = -1;
      return;
    }
    iVar1 = (**(code **)(*param_1 + 0x1c0))();
    if (0 < iVar1) {
      param_1[0x66] = -5;
      param_1[0x67] = 100;
      param_1[0x68] = 1;
    }
    param_1[0x69] = 0;
  }
  return;
}


