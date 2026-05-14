// FUN_004da180 @ 004da180 size=56

void __fastcall FUN_004da180(int param_1)

{
  if (*(undefined4 **)(param_1 + 0xfc) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0xfc))(1);
    *(undefined4 *)(param_1 + 0xfc) = 0;
    FUN_004830f0(*(undefined4 *)(param_1 + 0x100));
    *(undefined4 *)(param_1 + 0x100) = 0;
  }
  return;
}


