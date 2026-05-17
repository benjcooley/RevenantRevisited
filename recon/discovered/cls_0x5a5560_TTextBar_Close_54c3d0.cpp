// FUN_0054c3d0_TTextBar_Close @ 0054c3d0 size=97

void __fastcall FUN_0054c3d0_TTextBar_Close(int param_1)

{
  FUN_004830f0(*(undefined4 *)(param_1 + 0x6c));
  *(undefined4 *)(param_1 + 0x6c) = 0;
  if (*(undefined4 **)(param_1 + 0x84) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x84))(1);
  }
  *(undefined4 *)(param_1 + 0x84) = 0;
  if (*(undefined4 **)(param_1 + 0x88) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x88))(1);
  }
  *(undefined4 *)(param_1 + 0x88) = 0;
  if (*(undefined4 **)(param_1 + 0x8c) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x8c))(1);
  }
  *(undefined4 *)(param_1 + 0x8c) = 0;
  FUN_00491970_TPane_Close();
  return;
}


