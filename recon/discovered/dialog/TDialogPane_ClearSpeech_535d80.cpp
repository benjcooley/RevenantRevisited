// FUN_00535d80 @ 00535d80 size=270

void __thiscall FUN_00535d80(int param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  if (*(int *)(param_1 + 0x40) != 0) {
    if (*(int *)(param_1 + 400) != 0) {
      FUN_00536010();
    }
    FUN_00535a10();
  }
  if (param_2 != 0) {
    iVar2 = 0;
    if (0 < *(int *)(param_1 + 0x17c)) {
      do {
        if ((-1 < iVar2) && (iVar1 = *(int *)(*(int *)(param_1 + 0x18c) + iVar2 * 4), iVar1 != 0)) {
          FUN_005343e0();
          FUN_004830f0(iVar1);
        }
        FUN_0041cb40(iVar2);
        iVar2 = iVar2 + 1;
      } while (iVar2 < *(int *)(param_1 + 0x17c));
    }
    *(int *)(param_1 + 0x17c) = 0;
    *(undefined4 *)(param_1 + 0x180) = 0;
    return;
  }
  param_2 = 0;
  if (0 < *(int *)(param_1 + 0x17c)) {
    do {
      iVar2 = 0;
      if (((*(int *)(param_1 + 0x18c) != 0) && (param_2 < *(uint *)(param_1 + 0x17c))) &&
         (*(int *)(*(int *)(param_1 + 0x18c) + param_2 * 4) != 0)) {
        iVar1 = *(int *)(*(int *)(param_1 + 0x18c) + param_2 * 4);
        *(undefined4 *)(iVar1 + 0x50) = 1;
        *(undefined4 *)(iVar1 + 0x58) = 0;
        if (0 < *(int *)(iVar1 + 0x5c)) {
          piVar3 = (int *)(iVar1 + 0x100);
          do {
            if (*piVar3 != 0) {
              FUN_004367d0(*(undefined4 *)(*piVar3 + 0xc));
              *piVar3 = 0;
            }
            iVar2 = iVar2 + 1;
            piVar3 = piVar3 + 1;
          } while (iVar2 < *(int *)(iVar1 + 0x5c));
        }
      }
      param_2 = param_2 + 1;
    } while ((int)param_2 < *(int *)(param_1 + 0x17c));
  }
  return;
}


