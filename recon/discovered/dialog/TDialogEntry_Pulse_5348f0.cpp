// FUN_005348f0 @ 005348f0 size=329

void __fastcall FUN_005348f0(int param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int *piVar6;
  
  if (*(int *)(param_1 + 0x20) == -10000) {
    return;
  }
  iVar2 = 0;
  if (-1 < *(int *)(param_1 + 0x14)) {
    if (*(int *)(param_1 + 0x14) == 0) {
      iVar3 = 0;
      *(undefined4 *)(param_1 + 0x50) = 1;
      *(undefined4 *)(param_1 + 0x58) = 0;
      if (0 < *(int *)(param_1 + 0x5c)) {
        piVar5 = (int *)(param_1 + 0x100);
        do {
          if (*piVar5 != 0) {
            FUN_004367d0(*(undefined4 *)(*piVar5 + 0xc));
            *piVar5 = 0;
          }
          iVar3 = iVar3 + 1;
          piVar5 = piVar5 + 1;
        } while (iVar3 < *(int *)(param_1 + 0x5c));
      }
    }
    *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + -1;
  }
  iVar3 = *(int *)(param_1 + 0x30);
  if ((*(int *)(param_1 + 0x20) != iVar3) || (*(int *)(param_1 + 0x24) != *(int *)(param_1 + 0x34)))
  {
    iVar4 = *(int *)(param_1 + 0x38) + *(int *)(param_1 + 0x40);
    *(int *)(param_1 + 0x38) = iVar4;
    iVar4 = iVar4 >> 0x10;
    if (*(int *)(param_1 + 0x20) < iVar3) {
      if (iVar4 < iVar3) {
LAB_0053497d:
        iVar3 = iVar4;
      }
    }
    else if (iVar3 < iVar4) goto LAB_0053497d;
    *(int *)(param_1 + 0x20) = iVar3;
    iVar3 = *(int *)(param_1 + 0x34);
    iVar4 = *(int *)(param_1 + 0x3c) + *(int *)(param_1 + 0x44);
    *(int *)(param_1 + 0x3c) = iVar4;
    iVar4 = iVar4 >> 0x10;
    if (*(int *)(param_1 + 0x24) < iVar3) {
      if (iVar4 < iVar3) {
LAB_005349a9:
        iVar3 = iVar4;
      }
    }
    else if (iVar3 < iVar4) goto LAB_005349a9;
    *(int *)(param_1 + 0x24) = iVar3;
  }
  iVar3 = *(int *)(param_1 + 0x54);
  if (iVar3 < *(int *)(param_1 + 0x58)) {
    iVar3 = iVar3 + 1;
  }
  else {
    if (iVar3 <= *(int *)(param_1 + 0x58)) goto LAB_005349c1;
    iVar3 = iVar3 + -1;
  }
  *(int *)(param_1 + 0x54) = iVar3;
LAB_005349c1:
  if (0 < *(int *)(param_1 + 0x5c)) {
    piVar5 = (int *)(param_1 + 0x80);
    piVar6 = (int *)(param_1 + 0x120);
    do {
      if (piVar6[-8] != 0) {
        if ((*(byte *)(piVar6[-8] + 0x14) & 8) == 0) {
          piVar6[8] = 0;
        }
        else {
          piVar6[8] = 8;
        }
        iVar3 = *piVar6;
        if (iVar3 < piVar6[8]) {
          iVar3 = iVar3 + 1;
LAB_00534a00:
          *piVar6 = iVar3;
        }
        else if (piVar6[8] < iVar3) {
          iVar3 = iVar3 + -1;
          goto LAB_00534a00;
        }
        piVar1 = (int *)piVar6[-8];
        (**(code **)(*piVar1 + 0x28))
                  (*(int *)(param_1 + 0x18) + *piVar5 + *(int *)(param_1 + 0x20),
                   piVar5[1] + *(int *)(param_1 + 0x1c) + *(int *)(param_1 + 0x24),piVar1[0x1a],
                   piVar1[0x1b]);
      }
      iVar2 = iVar2 + 1;
      piVar6 = piVar6 + 1;
      piVar5 = piVar5 + 4;
    } while (iVar2 < *(int *)(param_1 + 0x5c));
  }
  return;
}


