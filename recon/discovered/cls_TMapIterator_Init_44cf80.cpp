// FUN_0044cf80_TMapIterator_Initialize @ 0044cf80 size=241

void __thiscall
FUN_0044cf80_TMapIterator_Initialize(uint *param_1,uint *param_2,uint param_3,uint param_4,int param_5,uint param_6)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  
  if (param_2 == (uint *)0x0) {
    param_1[10] = 0;
    param_1[0xb] = 0;
    param_1[0xc] = 0;
    param_1[0xd] = 0;
  }
  else {
    if ((param_3 & 0x43) == 0) {
      param_3 = param_3 | 3;
    }
    param_1[10] = *param_2;
    param_1[0xb] = param_2[1];
    uVar1 = param_2[3];
    param_1[0xc] = param_2[2];
    param_1[0xd] = uVar1;
  }
  *param_1 = param_3;
  param_1[1] = param_4;
  param_1[2] = 0;
  param_1[6] = 0;
  param_1[5] = 0;
  param_1[4] = 0xffffffff;
  param_1[8] = 0;
  param_1[3] = 0;
  param_1[7] = 0;
  iVar2 = DAT_00666988;
  if (((param_3 & 0x100) == 0) && ((param_3 & 0x80) != 0)) {
    param_1[9] = param_6;
    if (-1 < (int)param_6) {
      *param_1 = param_3 | 0x400;
      FUN_0044d080_TMapIterator_NextItem();
      return;
    }
  }
  else {
    if (param_5 == 0) {
      param_5 = DAT_00667fcc;
    }
    if (((param_3 & 0x100) == 0) || (param_5 == 0)) {
      param_1[9] = DAT_00666970;
      iVar3 = DAT_0066698c;
    }
    else {
      param_1[9] = (uint)*(ushort *)(param_5 + 0xe);
      iVar2 = *(int *)(param_5 + 0x10);
      iVar3 = *(int *)(param_5 + 0x14);
    }
    param_1[0x10] = (iVar2 >> 10) + 1;
    param_1[0xe] = (iVar2 >> 10) - 1;
    param_1[0xf] = (iVar3 >> 10) - 1;
    param_1[0x11] = (iVar3 >> 10) + 1;
  }
  FUN_0044d080_TMapIterator_NextItem();
  return;
}


