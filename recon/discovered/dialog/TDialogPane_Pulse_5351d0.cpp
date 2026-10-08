// FUN_005351d0 @ 005351d0 size=801

void __fastcall FUN_005351d0(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int *piStack_14;
  int iStack_10;
  
  iVar3 = DAT_006668e4 - DAT_0065be5c;
  iVar10 = 0;
  iVar6 = DAT_0065a8c8 + 10 + DAT_0065a8d0;
  iVar7 = DAT_006668e0 + -0x32 + DAT_006668e8;
  iVar9 = 0;
  if (0 < *(int *)(param_1 + 0x17c)) {
    do {
      FUN_005348f0();
      iVar9 = iVar9 + 1;
    } while (iVar9 < *(int *)(param_1 + 0x17c));
  }
  iVar1 = *(int *)(param_1 + 0x17c);
  iVar9 = 0;
  if (0 < iVar1) {
    piVar5 = *(int **)(param_1 + 0x18c);
    piStack_14 = (int *)iVar1;
    do {
      iVar2 = *piVar5;
      *(int *)(iVar2 + 0x18) = (iVar3 + -400) / 2;
      iVar8 = iVar6;
      if (*(int *)(iVar2 + 8) != 1) {
        iVar8 = iVar7;
      }
      *(int *)(iVar2 + 0x1c) = iVar8;
      if ((*(int *)(iVar2 + 0x50) == 0) && (*(int *)(iVar2 + 8) != 1)) {
        iVar10 = *(int *)(iVar2 + 0x2c) + iVar9;
        iVar9 = iVar9 + 5 + *(int *)(iVar2 + 0x2c);
      }
      piVar5 = piVar5 + 1;
      piStack_14 = (int *)((int)piStack_14 + -1);
    } while (piStack_14 != (int *)0x0);
  }
  iVar6 = 0;
  iVar10 = -iVar10;
  if (0 < iVar1) {
    piStack_14 = *(int **)(param_1 + 0x18c);
    iStack_10 = iVar1;
    do {
      iVar7 = *piStack_14;
      if (*(int *)(iVar7 + 0x50) == 0) {
        if (*(int *)(iVar7 + 8) == 1) {
          if ((*(int *)(iVar7 + 0x30) != 0) || (iVar6 != *(int *)(iVar7 + 0x34))) {
            if (*(int *)(iVar7 + 0x20) == -10000) {
              *(int *)(iVar7 + 0x24) = iVar6;
              *(undefined4 *)(iVar7 + 0x20) = 0;
              *(undefined4 *)(iVar7 + 0x30) = 0;
              *(int *)(iVar7 + 0x34) = iVar6;
              iVar6 = iVar6 + 5 + *(int *)(iVar7 + 0x2c);
              goto LAB_005353d7;
            }
            *(undefined4 *)(iVar7 + 0x30) = 0;
            *(int *)(iVar7 + 0x34) = iVar6;
            iVar3 = *(int *)(iVar7 + 0x20) << 0x10;
            *(int *)(iVar7 + 0x3c) = *(int *)(iVar7 + 0x24) << 0x10;
            *(int *)(iVar7 + 0x38) = iVar3;
            iVar3 = (int)((ulonglong)((longlong)iVar3 * -0x2aaaaaab) >> 0x20);
            *(int *)(iVar7 + 0x40) = (iVar3 >> 1) - (iVar3 >> 0x1f);
            *(int *)(iVar7 + 0x44) = ((iVar6 - *(int *)(iVar7 + 0x24)) * 0x10000) / 0xc;
          }
          iVar6 = iVar6 + 5 + *(int *)(iVar7 + 0x2c);
        }
        else {
          if ((*(int *)(iVar7 + 0x30) != 0) || (iVar10 != *(int *)(iVar7 + 0x34))) {
            if (*(int *)(iVar7 + 0x20) == -10000) {
              *(int *)(iVar7 + 0x24) = iVar10;
              *(undefined4 *)(iVar7 + 0x20) = 0;
              *(undefined4 *)(iVar7 + 0x30) = 0;
              *(int *)(iVar7 + 0x34) = iVar10;
            }
            else {
              *(undefined4 *)(iVar7 + 0x30) = 0;
              *(int *)(iVar7 + 0x34) = iVar10;
              iVar3 = *(int *)(iVar7 + 0x20) << 0x10;
              *(int *)(iVar7 + 0x3c) = *(int *)(iVar7 + 0x24) << 0x10;
              *(int *)(iVar7 + 0x38) = iVar3;
              iVar3 = (int)((ulonglong)((longlong)iVar3 * -0x2aaaaaab) >> 0x20);
              *(int *)(iVar7 + 0x40) = (iVar3 >> 1) - (iVar3 >> 0x1f);
              *(int *)(iVar7 + 0x44) = ((iVar10 - *(int *)(iVar7 + 0x24)) * 0x10000) / 0xc;
            }
          }
          iVar10 = iVar10 + 5 + *(int *)(iVar7 + 0x2c);
        }
      }
LAB_005353d7:
      piStack_14 = piStack_14 + 1;
      iStack_10 = iStack_10 + -1;
    } while (iStack_10 != 0);
  }
  iVar6 = 0;
  if (0 < iVar1) {
    do {
      iVar7 = *(int *)(*(int *)(param_1 + 0x18c) + iVar6 * 4);
      if ((*(int *)(iVar7 + 0x50) == 1) && (*(int *)(iVar7 + 0x54) == 0)) {
        iVar7 = *(int *)(*(int *)(param_1 + 0x18c) + iVar6 * 4);
        if (iVar7 != 0) {
          FUN_005343e0();
          FUN_004830f0(iVar7);
        }
        FUN_0041cb80(iVar6);
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < *(int *)(param_1 + 0x17c));
  }
  if (DAT_0065d0d0 == 0) {
    uVar4 = *(uint *)(param_1 + 0x60) | 0xc;
  }
  else {
    uVar4 = *(uint *)(param_1 + 0x60) & 0xfffffff3;
  }
  *(uint *)(param_1 + 0x60) = uVar4;
  if ((*(int *)(param_1 + 0x1e4) != 0) && (-1 < *(int *)(param_1 + 0x1dc))) {
    *(undefined4 *)(param_1 + 0x1e4) = 0;
    FUN_0047c580(*(undefined4 *)(param_1 + 0x1ec));
    iVar6 = *(int *)(param_1 + 400);
    if (iVar6 != 0) {
      iVar7 = 0;
      *(undefined4 *)(iVar6 + 0x50) = 1;
      *(undefined4 *)(iVar6 + 0x58) = 0;
      if (0 < *(int *)(iVar6 + 0x5c)) {
        piVar5 = (int *)(iVar6 + 0x100);
        do {
          if (*piVar5 != 0) {
            FUN_004367d0(*(undefined4 *)(*piVar5 + 0xc));
            *piVar5 = 0;
          }
          iVar7 = iVar7 + 1;
          piVar5 = piVar5 + 1;
        } while (iVar7 < *(int *)(iVar6 + 0x5c));
      }
      *(undefined4 *)(param_1 + 400) = 0;
    }
    *(undefined4 *)(param_1 + 0x194) = 0;
    *(undefined4 *)(param_1 + 0x1e8) = 0;
  }
  FUN_00435d70();
  return;
}


