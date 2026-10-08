// FUN_00530600 @ 00530600 size=110

void __fastcall FUN_00530600(int param_1)

{
  int iVar1;
  
  iVar1 = 0;
  if (0 < *(short *)(param_1 + 0x194)) {
    do {
      FUN_0052f310();
      iVar1 = iVar1 + 1;
    } while (iVar1 < *(short *)(param_1 + 0x194));
  }
  if (*(int *)(param_1 + 0x198) != 0) {
    FUN_004830f0(*(int *)(param_1 + 0x198));
  }
  *(undefined4 *)(param_1 + 0x198) = 0;
  *(undefined2 *)(param_1 + 0x194) = 0;
  *(undefined2 *)(param_1 + 0x196) = 0;
  *(undefined4 *)(param_1 + 0x1b0) = 0;
  *(undefined4 *)(param_1 + 0x48) = 1;
  *(undefined4 *)(param_1 + 0x4c) = 1;
  return;
}


