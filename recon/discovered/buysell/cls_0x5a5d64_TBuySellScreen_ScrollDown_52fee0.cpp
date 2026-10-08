// FUN_0052fee0 @ 0052fee0 size=87

undefined4 __thiscall FUN_0052fee0(int *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  if (param_3 == 3000) {
    iVar1 = param_1[0x60] + param_1[0x61];
    param_1[0x60] = iVar1;
    if ((short)param_1[0x65] + -1 < iVar1) {
      param_1[0x60] = iVar1 - param_1[0x61];
    }
    param_1[0x62] = -1;
    param_1[99] = -1;
    (**(code **)(*param_1 + 0x2c))(1);
    return 1;
  }
  return 0;
}


