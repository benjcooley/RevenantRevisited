// FUN_004d6e00_SetMoveTarget @ 004d6e00 size=43

void __thiscall FUN_004d6e00_SetMoveTarget(int param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  
  *(undefined4 *)(param_1 + 0xec) = 1;
  *(undefined4 *)(param_1 + 0xf0) = *param_2;
  uVar1 = param_2[2];
  *(undefined4 *)(param_1 + 0xf4) = param_2[1];
  *(undefined4 *)(param_1 + 0xf8) = uVar1;
  return;
}


