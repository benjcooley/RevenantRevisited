// FUN_0047ebc0 @ 0047ebc0 size=81

void __thiscall FUN_0047ebc0(int param_1,int param_2)

{
  if (((*(int *)(param_1 + 0x6ac) != param_2) && (*(int *)(param_1 + 0x6b0) == 0)) &&
     (*(int *)(param_1 + 0x6b4) == 0)) {
    if (param_2 == 0) {
      if (*(int *)(param_1 + 0x6ac) != 0) {
        *(undefined4 *)(param_1 + 0x6b4) = 1;
        return;
      }
    }
    else {
      *(undefined4 *)(param_1 + 0x6c4) = 2;
      *(undefined4 *)(param_1 + 0x6b0) = 1;
    }
  }
  return;
}


