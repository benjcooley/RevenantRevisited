// FUN_00535060 @ 00535060 size=190

void __fastcall FUN_00535060(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  
  if (*(int *)(param_1 + 0x40) != 0) {
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
    iVar3 = 0;
    *(undefined4 *)(param_1 + 0x1d8) = 0;
    *(undefined4 *)(param_1 + 0x1dc) = 0xffffffff;
    *(undefined4 *)(param_1 + 0x1e4) = 0;
    if (0 < *(int *)(param_1 + 0x17c)) {
      do {
        if ((-1 < iVar3) && (iVar1 = *(int *)(*(int *)(param_1 + 0x18c) + iVar3 * 4), iVar1 != 0)) {
          FUN_005343e0();
          FUN_004830f0(iVar1);
        }
        FUN_0041cb40(iVar3);
        iVar3 = iVar3 + 1;
      } while (iVar3 < *(int *)(param_1 + 0x17c));
    }
    *(int *)(param_1 + 0x17c) = 0;
    *(undefined4 *)(param_1 + 0x180) = 0;
    *(undefined4 *)(param_1 + 400) = 0;
    FUN_00434f30();
  }
  return;
}


