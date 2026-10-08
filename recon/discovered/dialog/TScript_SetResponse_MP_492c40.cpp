// FUN_00492c40 @ 00492c40 size=114

void __thiscall FUN_00492c40(int param_1,undefined1 param_2)

{
  short sVar1;
  int *piVar2;
  
  *(undefined1 *)(param_1 + 0xb6) = param_2;
  *(undefined1 *)(param_1 + 0xb5) = 0;
  if ((((*(char *)(param_1 + 0xb4) == '\x03') || (*(char *)(param_1 + 0xb4) == '\b')) &&
      (piVar2 = *(int **)(param_1 + 0xbc), piVar2 != (int *)0x0)) && ((piVar2[2] & 0x20000U) != 0))
  {
    piVar2 = (int *)(**(code **)(*piVar2 + 0x1fc))();
    if (*piVar2 == 0x10) {
      *(undefined1 *)(param_1 + 0xb4) = 0;
      sVar1 = *(short *)(*(int *)(param_1 + 0xbc) + 4);
      if ((sVar1 == 0xc) || (sVar1 == 0xb)) {
        FUN_004d6000();
      }
      *(undefined4 *)(param_1 + 0xbc) = 0;
    }
  }
  return;
}


