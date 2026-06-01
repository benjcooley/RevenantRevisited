// FUN_00436900 @ 00436900 size=33

undefined4 __thiscall FUN_00436900(int param_1,int param_2)

{
  if ((-1 < param_2) && (param_2 < *(int *)(param_1 + 0x88))) {
    return *(undefined4 *)(*(int *)(param_1 + 0x98) + param_2 * 4);
  }
  return 0;
}


