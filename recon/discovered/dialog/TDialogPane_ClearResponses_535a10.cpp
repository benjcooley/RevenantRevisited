// FUN_00535a10 @ 00535a10 size=181

void __fastcall FUN_00535a10(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  
  iVar3 = *(int *)(param_1 + 400);
  if (iVar3 != 0) {
    iVar1 = 0;
    *(undefined4 *)(iVar3 + 0x50) = 1;
    *(undefined4 *)(iVar3 + 0x58) = 0;
    if (0 < *(int *)(iVar3 + 0x5c)) {
      piVar2 = (int *)(iVar3 + 0x100);
      do {
        if (*piVar2 != 0) {
          FUN_004367d0(*(undefined4 *)(*piVar2 + 0xc));
          *piVar2 = 0;
        }
        iVar1 = iVar1 + 1;
        piVar2 = piVar2 + 1;
      } while (iVar1 < *(int *)(iVar3 + 0x5c));
    }
    *(undefined4 *)(param_1 + 400) = 0;
  }
  iVar3 = 0;
  if (0 < *(int *)(param_1 + 0x1d8)) {
    piVar2 = (int *)(param_1 + 0x1b8);
    do {
      if (piVar2[-8] != 0) {
        FUN_00482f80(piVar2[-8]);
      }
      if (*piVar2 != 0) {
        FUN_00482f80(*piVar2);
      }
      iVar3 = iVar3 + 1;
      piVar2 = piVar2 + 1;
    } while (iVar3 < *(int *)(param_1 + 0x1d8));
  }
  *(undefined4 *)(param_1 + 0x1d8) = 0;
  *(undefined4 *)(param_1 + 0x1dc) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x1e4) = 0;
  return;
}


