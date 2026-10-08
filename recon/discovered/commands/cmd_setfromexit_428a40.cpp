// FUN_00428a40 @ 00428a40 size=35

undefined4 FUN_00428a40(int param_1)

{
  if (*(short *)(param_1 + 4) == 10) {
    *(uint *)(param_1 + 0xe4) = *(uint *)(param_1 + 0xe4) & 4;
    return 0;
  }
  return 4;
}


