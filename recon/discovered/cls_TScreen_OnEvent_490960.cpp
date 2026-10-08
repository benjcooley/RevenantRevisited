// FUN_00490960 @ 00490960 size=97

void __thiscall FUN_00490960(int param_1,int param_2,undefined4 param_3)

{
  uint uVar1;
  
  if (param_2 == 0x103) {
    FUN_00416fb0();
  }
  uVar1 = 0;
  if (0 < *(int *)(param_1 + 4)) {
    do {
      if (((*(int *)(param_1 + 0x14) != 0) && (uVar1 < *(uint *)(param_1 + 4))) &&
         (*(int *)(*(int *)(param_1 + 0x14) + uVar1 * 4) != 0)) {
        (**(code **)(**(int **)(*(int *)(param_1 + 0x14) + uVar1 * 4) + 0x78))(param_2,param_3);
      }
      uVar1 = uVar1 + 1;
    } while ((int)uVar1 < *(int *)(param_1 + 4));
  }
  return;
}


