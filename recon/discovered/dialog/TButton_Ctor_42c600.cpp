// FUN_0042c600 @ 0042c600 size=158

undefined4 * __thiscall
FUN_0042c600(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8,
            undefined4 param_9,undefined4 param_10,undefined4 param_11,undefined4 param_12,
            undefined4 param_13)

{
  FUN_0042a210(0,4,param_2,0,0xffffffff,0,param_3,param_4,param_5,param_6,param_12,param_7,param_13,
               param_8,param_9,param_10,param_11,0);
  *param_1 = &PTR_FUN_005a3c68;
  param_1[0x26] = 0;
  param_1[0x24] = 0;
  if (-1 < (int)param_1[0x1d]) {
    param_1[5] = param_1[5] | 0x80000;
  }
  if ((param_1[5] & 0x80000) != 0) {
    param_1[5] = param_1[5] | 0x40000;
  }
  return param_1;
}


