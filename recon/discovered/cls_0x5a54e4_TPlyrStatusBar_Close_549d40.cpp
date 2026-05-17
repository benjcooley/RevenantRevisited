// FUN_00549d40_TPlyrStatusBar_Close @ 00549d40 size=90

void __fastcall FUN_00549d40_TPlyrStatusBar_Close(int param_1)

{
  if (*(undefined4 **)(param_1 + 100) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 100))(1);
  }
  if (*(undefined4 **)(param_1 + 0x60) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x60))(1);
  }
  if (*(undefined4 **)(param_1 + 0x6c) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x6c))(1);
  }
  if (*(undefined4 **)(param_1 + 0x70) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x70))(1);
  }
  if (*(undefined4 **)(param_1 + 0x74) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x74))(1);
  }
  if (*(undefined4 **)(param_1 + 0x68) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x68))(1);
  }
  FUN_00491970_TPane_Close();
  return;
}


