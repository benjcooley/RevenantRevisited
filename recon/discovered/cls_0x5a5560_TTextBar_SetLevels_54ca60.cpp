// FUN_0054ca60 @ 0054ca60 size=101

void __thiscall FUN_0054ca60(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  if (*(int *)(param_1 + 0x90) == 0) {
    uVar1 = FUN_0049d800(s_loadmsg_005e5814);
    uVar2 = 1;
    *(undefined4 *)(param_1 + 0x94) = 1;
    *(undefined4 *)(param_1 + 0x90) = 1;
    FUN_00444e20(0);
    FUN_0054d0c0(0x80,uVar2,uVar1);
  }
  *(undefined4 *)(param_1 + 0x9c) = param_3;
  *(undefined4 *)(param_1 + 0x98) = param_2;
  FUN_0054cd40(0);
  return;
}


