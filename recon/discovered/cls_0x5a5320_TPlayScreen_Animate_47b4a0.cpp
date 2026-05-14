// FUN_0047b4a0_TPlayScreen_Animate @ 0047b4a0 size=36

void __fastcall FUN_0047b4a0_TPlayScreen_Animate(int param_1)

{
  if (*(undefined4 **)(param_1 + 0x69c) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x69c))();
    *(undefined4 *)(param_1 + 0x69c) = 0;
  }
  FUN_0048ff00_TScreen_Animate();
  return;
}


