// FUN_0052fa60 @ 0052fa60 size=131

void __thiscall FUN_0052fa60(int *param_1,int param_2,int param_3)

{
  int iVar1;
  
  if (param_3 == 0) goto code_r0x0052fac5;
  if (param_2 == 0x31) {
    iVar1 = param_1[0x60];
LAB_0052fa98:
    param_1[0x62] = iVar1;
    (**(code **)(*param_1 + 0x2c))(1);
  }
  else {
    if (param_2 == 0x32) {
      iVar1 = param_1[0x60] + 1;
      goto LAB_0052fa98;
    }
    if (param_2 == 0x33) {
      iVar1 = param_1[0x60] + 2;
      goto LAB_0052fa98;
    }
  }
  if ((int)(short)param_1[0x65] <= param_1[0x62]) {
    param_1[0x62] = (short)param_1[0x65] + -1;
  }
code_r0x0052fac5:
  switch(param_2) {
  case 0x42:
  case 0x56:
  case 0x62:
  case 0x76:
    break;
  default:
    FUN_004361f0(param_2,param_3);
  }
  return;
}


