// FUN_00534a40 @ 00534a40 size=72

void __fastcall FUN_00534a40(int param_1)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = 0;
  *(undefined4 *)(param_1 + 0x50) = 1;
  *(undefined4 *)(param_1 + 0x58) = 0;
  if (0 < *(int *)(param_1 + 0x5c)) {
    piVar2 = (int *)(param_1 + 0x100);
    do {
      if (*piVar2 != 0) {
        FUN_004367d0(*(undefined4 *)(*piVar2 + 0xc));
        *piVar2 = 0;
      }
      iVar1 = iVar1 + 1;
      piVar2 = piVar2 + 1;
    } while (iVar1 < *(int *)(param_1 + 0x5c));
  }
  return;
}


