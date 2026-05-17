// FUN_0053d360_TSideTabsPane_Close @ 0053d360 size=60

void __fastcall FUN_0053d360_TSideTabsPane_Close(int param_1)

{
  if (*(undefined4 **)(param_1 + 0x188) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x188))(1);
  }
  if (*(undefined4 **)(param_1 + 0x18c) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x18c))(1);
  }
  if (*(undefined4 **)(param_1 + 400) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 400))(1);
  }
  FUN_00434f30_TButtonPane_Close();
  return;
}


