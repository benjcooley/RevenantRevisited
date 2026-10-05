// FUN_0052fd50 @ 0052fd50 size=316

void __thiscall FUN_0052fd50(int *param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  
  FUN_00436530(param_2,param_3,param_4);
  if (((param_2 != 4) && (param_2 != 5)) || (199 < param_3)) goto LAB_0052fe7c;
  if ((param_4 < 0x2b) || (0x55 < param_4)) {
    if ((param_4 < 0x57) || (0x81 < param_4)) {
      if ((0x82 < param_4) && (param_4 < 0xae)) {
        param_1[0x62] = param_1[0x60] + 2;
        iVar1 = FUN_0049c430(s_click1_005e3dc4);
        if (-1 < iVar1) {
          iVar2 = FUN_0049b650(iVar1);
          goto joined_r0x0052fe07;
        }
      }
    }
    else {
      param_1[0x62] = param_1[0x60] + 1;
      iVar1 = FUN_0049c430(s_click1_005e3dbc);
      if (-1 < iVar1) {
        iVar2 = FUN_0049b650(iVar1);
        goto joined_r0x0052fe07;
      }
    }
  }
  else {
    param_1[0x62] = param_1[0x60];
    iVar1 = FUN_0049c430(s_click1_005e3db4);
    if (-1 < iVar1) {
      iVar2 = FUN_0049b650(iVar1);
joined_r0x0052fe07:
      if (iVar2 != 0) {
        FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
      }
    }
  }
  if ((int)(short)param_1[0x65] <= param_1[0x62]) {
    param_1[0x62] = (short)param_1[0x65] + -1;
  }
LAB_0052fe7c:
  (**(code **)(*param_1 + 0x2c))(1);
  return;
}


