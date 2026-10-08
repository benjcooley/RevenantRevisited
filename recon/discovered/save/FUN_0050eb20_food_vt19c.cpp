// FUN_0050eb20 @ 0050eb20 size=92

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0050eb20(int *param_1,undefined4 param_2)

{
  (**(code **)(*param_1 + 0xe8))(DAT_0066d2f4,param_2);
  if (param_1[0x19] == DAT_0065d674) {
    _DAT_0065d548 = 1;
    (**(code **)(DAT_0065d4f8 + 0x90))();
  }
  if (param_1[0x19] == DAT_0065b088) {
    _DAT_0065b078 = 1;
  }
  return;
}


