// FUN_00530570 @ 00530570 size=140

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall FUN_00530570(int *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  if (param_3 != 3000) {
    (**(code **)(*param_1 + 0x2c))(1);
    return 0;
  }
  iVar1 = 0;
  param_1[0x6c] = 0;
  if (0 < (short)param_1[0x65]) {
    do {
      FUN_0052f310();
      iVar1 = iVar1 + 1;
    } while (iVar1 < (short)param_1[0x65]);
  }
  if (param_1[0x66] != 0) {
    FUN_004830f0(param_1[0x66]);
  }
  param_1[0x66] = 0;
  *(undefined2 *)(param_1 + 0x65) = 0;
  *(undefined2 *)((int)param_1 + 0x196) = 0;
  _DAT_0065d1a8 = 0;
  return 1;
}


