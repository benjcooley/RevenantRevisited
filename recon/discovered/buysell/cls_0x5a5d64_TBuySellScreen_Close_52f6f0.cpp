// FUN_0052f6f0 @ 0052f6f0 size=162

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0052f6f0(int param_1)

{
  undefined4 uVar1;
  
  if (*(int *)(param_1 + 400) != 0) {
    FUN_00482f80(*(int *)(param_1 + 400));
    *(undefined4 *)(param_1 + 400) = 0;
  }
  if (*(undefined4 **)(param_1 + 0x19c) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x19c))(1);
    *(undefined4 *)(param_1 + 0x19c) = 0;
  }
  if (*(int *)(param_1 + 0x1a4) != 0) {
    FUN_004830f0(*(int *)(param_1 + 0x1a4));
    *(undefined4 *)(param_1 + 0x1a4) = 0;
  }
  if (*(int *)(param_1 + 0x1a8) != 0) {
    FUN_004830f0(*(int *)(param_1 + 0x1a8));
    *(undefined4 *)(param_1 + 0x1a8) = 0;
  }
  uVar1 = FUN_0048ed60(param_1);
  FUN_0048ef30(uVar1);
  FUN_0048ee10(param_1);
  _DAT_0065cb40 = 1;
  *(undefined4 *)(param_1 + 0x1ac) = 0;
  FUN_00434f30();
  return;
}


