// FUN_00429910 @ 00429910 size=49

undefined4 FUN_00429910(int param_1,int param_2)

{
  if (param_2 < 0) {
    if (param_1 != 0) {
      return 0;
    }
  }
  else if (param_1 == 0) {
    return 0;
  }
  if (((0 < param_2) && (param_1 != 0)) && (*(short *)(param_1 + 4) != param_2)) {
    return 0;
  }
  return 1;
}


