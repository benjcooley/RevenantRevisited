// FUN_004daa70_TActionBlock_Ctor2 @ 004daa70 size=106

undefined4 * __thiscall
FUN_004daa70_TActionBlock_Ctor2(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  param_1[10] = 0;
  param_1[0xc] = 0;
  param_1[0xb] = 0;
  *(undefined1 *)(param_1 + 1) = 0;
  param_1[0x10] = 0;
  param_1[0xf] = 0;
  param_1[0xe] = 0;
  param_1[0x11] = 0;
  param_1[0x17] = 0;
  param_1[0x14] = 0;
  param_1[0x12] = 0;
  param_1[0x13] = 0;
  *param_1 = 1;
  param_1[9] = 0xffffffff;
  param_1[0xd] = 0x10;
  param_1[0x18] = 1;
  FUN_0058b100(param_1 + 1,&DAT_005e0770,param_2,param_3);
  *param_1 = param_4;
  return param_1;
}


