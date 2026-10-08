// FUN_00470480 @ 00470480 size=77

void __thiscall FUN_00470480(int param_1,int *param_2)

{
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  if (param_2 != (int *)0x0) {
    (**(code **)(*param_2 + 0x60))();
    uStack_4 = *(undefined4 *)(param_1 + 0x18);
    uStack_8 = *(undefined4 *)(param_1 + 0x14);
    uStack_c = *(undefined4 *)(param_1 + 0x10);
    (**(code **)(*param_2 + 8))(&uStack_c,0xffffffff,0);
    (**(code **)(*param_2 + 0x8c))();
  }
  return;
}


