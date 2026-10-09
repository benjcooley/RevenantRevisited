// FUN_00477be0 @ 00477be0 size=19

undefined4 __fastcall FUN_00477be0(int param_1)

{
  undefined4 uVar1;
  
  if (*(int **)(param_1 + 0x54) != (int *)0x0) {
    uVar1 = (**(code **)(**(int **)(param_1 + 0x54) + 0x28))(param_1);
    return uVar1;
  }
  return 0;
}


