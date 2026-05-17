// FUN_0053d3a0 @ 0053d3a0 size=90

void __fastcall FUN_0053d3a0(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  
  iVar3 = 0;
  if (0 < *(int *)(param_1 + 0x88)) {
    piVar2 = (int *)(param_1 + 0x1ac);
    do {
      iVar1 = FUN_00436900(iVar3);
      if ((*(byte *)(iVar1 + 0x14) & 8) == 0) {
        piVar2[6] = 0;
      }
      else {
        piVar2[6] = 8;
      }
      iVar1 = *piVar2;
      if (iVar1 < piVar2[6]) {
        iVar1 = iVar1 + 1;
LAB_0053d3e5:
        *piVar2 = iVar1;
      }
      else if (piVar2[6] < iVar1) {
        iVar1 = iVar1 + -1;
        goto LAB_0053d3e5;
      }
      iVar3 = iVar3 + 1;
      piVar2 = piVar2 + 1;
    } while (iVar3 < *(int *)(param_1 + 0x88));
  }
  return;
}


