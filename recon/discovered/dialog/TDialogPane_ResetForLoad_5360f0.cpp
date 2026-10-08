// FUN_005360f0 @ 005360f0 size=438

void __fastcall FUN_005360f0(int *param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  
  if (param_1[0x10] != 0) {
    iVar3 = param_1[100];
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
      param_1[100] = 0;
    }
    iVar3 = 0;
    if (0 < param_1[0x76]) {
      piVar2 = param_1 + 0x6e;
      do {
        if (piVar2[-8] != 0) {
          FUN_00482f80(piVar2[-8]);
        }
        if (*piVar2 != 0) {
          FUN_00482f80(*piVar2);
        }
        iVar3 = iVar3 + 1;
        piVar2 = piVar2 + 1;
      } while (iVar3 < param_1[0x76]);
    }
    param_1[0x76] = 0;
    param_1[0x77] = -1;
    param_1[0x79] = 0;
    if (param_1[0x10] != 0) {
      if (param_1[100] != 0) {
        FUN_00536010();
      }
      FUN_00535a10();
    }
    iVar3 = 0;
    if (0 < param_1[0x5f]) {
      do {
        FUN_00536320(iVar3);
        iVar3 = iVar3 + 1;
      } while (iVar3 < param_1[0x5f]);
    }
    iVar1 = 0;
    param_1[0x5f] = 0;
    param_1[0x60] = 0;
    iVar3 = param_1[100];
    if (iVar3 != 0) {
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
      param_1[100] = 0;
      FUN_0047c580(param_1[0x7b]);
      if (param_1[0x65] != 0) {
        FUN_00471290(s_Finish_005e3f54);
      }
      if ((param_1[0x65] != 0) && (*(int *)(param_1[0x65] + 0xd8) != 0)) {
        iVar3 = FUN_004dab80(&DAT_005e3f5c);
        if (iVar3 != 0) {
          *(undefined4 *)(param_1[0x65] + 0x108) = 1;
        }
      }
      (**(code **)(*param_1 + 0x34))();
    }
  }
  return;
}


