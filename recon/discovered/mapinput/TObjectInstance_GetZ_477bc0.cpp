// FUN_00477bc0 @ 00477bc0 size=30

undefined4 __thiscall FUN_00477bc0(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  if (*(int **)(param_1 + 0x54) != (int *)0x0) {
    uVar1 = (**(code **)(**(int **)(param_1 + 0x54) + 0x18))(param_1,param_2);
    return uVar1;
  }
  return 0;
}


