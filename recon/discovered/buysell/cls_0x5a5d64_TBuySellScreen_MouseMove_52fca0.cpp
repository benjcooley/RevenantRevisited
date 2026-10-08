// FUN_0052fca0 @ 0052fca0 size=163

void __thiscall FUN_0052fca0(int *param_1,undefined4 param_2,int param_3,int param_4)

{
  FUN_00436660(param_2,param_3,param_4);
  param_1[99] = -1;
  if (param_3 < 200) {
    if ((param_4 < 0x2b) || (0x55 < param_4)) {
      if ((param_4 < 0x57) || (0x81 < param_4)) {
        if ((0x82 < param_4) && (param_4 < 0xae)) {
          param_1[99] = param_1[0x60] + 2;
        }
      }
      else {
        param_1[99] = param_1[0x60] + 1;
      }
    }
    else {
      param_1[99] = param_1[0x60];
    }
    if ((int)(short)param_1[0x65] <= param_1[99]) {
      param_1[99] = (short)param_1[0x65] + -1;
    }
  }
  (**(code **)(*param_1 + 0x2c))(1);
  return;
}


