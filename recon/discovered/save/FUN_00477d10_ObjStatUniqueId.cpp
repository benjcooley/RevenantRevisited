// FUN_00477d10 @ 00477d10 size=32

undefined4 __thiscall FUN_00477d10(short *param_1,uint param_2)

{
  if (param_2 < (uint)(int)*param_1) {
    return *(undefined4 *)(param_2 * 0x50 + 0x4c + *(int *)(param_1 + 2));
  }
  return 0;
}


