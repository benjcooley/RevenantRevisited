// FUN_00471020 @ 00471020 size=117

void __thiscall FUN_00471020(int param_1,int *param_2)

{
  int unaff_ESI;
  int unaff_EDI;
  int iStack_10;
  int iStack_c;
  
  if (*(int **)(param_1 + 0x54) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x54) + 0xa8))(param_1,param_2);
    if (((*(byte *)(param_1 + 8) & 4) != 0) && (DAT_005d7a18 == 0)) {
      FUN_00471430(&stack0xffffffe8);
      if (unaff_EDI < *param_2) {
        *param_2 = unaff_EDI;
      }
      if (unaff_ESI < param_2[1]) {
        param_2[1] = unaff_ESI;
      }
      if (param_2[2] < iStack_10) {
        param_2[2] = iStack_10;
      }
      if (param_2[3] < iStack_c) {
        param_2[3] = iStack_c;
      }
    }
  }
  return;
}


