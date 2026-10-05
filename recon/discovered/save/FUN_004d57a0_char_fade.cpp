// FUN_004d57a0 @ 004d57a0 size=217

void __fastcall FUN_004d57a0(int *param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = param_1[0x66];
  if (iVar1 == 0) {
    return;
  }
  iVar2 = param_1[0x65];
  param_1[0x65] = iVar2 - iVar1;
  if (((0 < iVar2 - iVar1) && (iVar1 < 0)) && ((param_1[2] & 0x80U) != 0)) {
    (**(code **)(*param_1 + 0x40))(param_1[2] & 0xffffff7f);
  }
  iVar1 = param_1[0x67];
  if (iVar1 == -1) {
    if (-1 < param_1[0x65]) goto LAB_004d581c;
    param_1[0x65] = 0;
  }
  else {
    if (param_1[0x66] < 1) {
      if (-1 < param_1[0x66]) {
        return;
      }
      if (iVar1 < param_1[0x65]) {
        param_1[0x65] = iVar1;
        param_1[0x68] = 0;
        param_1[0x66] = 0;
      }
      if (-1 < param_1[0x65]) {
        return;
      }
      param_1[0x65] = 0;
      param_1[0x68] = 0;
      param_1[0x66] = 0;
      return;
    }
    if (iVar1 <= param_1[0x65]) goto LAB_004d581c;
    param_1[0x65] = iVar1;
  }
  param_1[0x68] = 0;
  param_1[0x66] = 0;
LAB_004d581c:
  if (param_1[0x65] < 0x65) {
    return;
  }
  param_1[0x68] = 0;
  param_1[0x66] = 0;
  param_1[0x65] = 100;
  return;
}


