// FUN_00491900_TPane_Initialize @ 00491900 size=98

undefined4 __fastcall FUN_00491900_TPane_Initialize(int *param_1)

{
  if (param_1[0x10] == 0) {
    param_1[1] = param_1[5];
    param_1[3] = param_1[7];
    param_1[2] = param_1[6];
    param_1[0xe] = 0;
    param_1[0xc] = 0;
    param_1[10] = 0;
    param_1[0xd] = 0;
    param_1[0xb] = 0;
    param_1[9] = 0;
    param_1[0x12] = 0;
    param_1[0x13] = 0;
    param_1[0x15] = 0;
    param_1[0x16] = -1;
    param_1[0x17] = -1;
    param_1[4] = param_1[8];
    param_1[0x14] = 1;
    (**(code **)(*param_1 + 0xc))();
    param_1[0x10] = 1;
  }
  return 1;
}


