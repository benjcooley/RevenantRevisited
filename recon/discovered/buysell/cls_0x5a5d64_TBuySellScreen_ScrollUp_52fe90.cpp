// FUN_0052fe90 @ 0052fe90 size=77

undefined4 __thiscall FUN_0052fe90(int *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  if (param_3 == 3000) {
    iVar1 = param_1[0x60];
    param_1[0x60] = iVar1 - param_1[0x61];
    if (iVar1 - param_1[0x61] < 0) {
      param_1[0x60] = 0;
    }
    param_1[0x62] = -1;
    param_1[99] = -1;
    (**(code **)(*param_1 + 0x2c))(1);
    return 1;
  }
  return 0;
}


