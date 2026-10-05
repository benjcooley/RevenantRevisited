// FUN_00535870 @ 00535870 size=405

void __thiscall FUN_00535870(int param_1,char *param_2,char *param_3)

{
  char cVar1;
  char *pcVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  int *piVar6;
  char *pcVar7;
  int iVar8;
  char *pcVar9;
  
  if ((*(int *)(param_1 + 0x40) != 0) || (*(int *)(param_1 + 400) != 0)) {
    if ((-1 < *(int *)(param_1 + 0x1dc)) && (*(int *)(param_1 + 0x40) != 0)) {
      iVar8 = *(int *)(param_1 + 400);
      if (iVar8 != 0) {
        iVar5 = 0;
        *(undefined4 *)(iVar8 + 0x50) = 1;
        *(undefined4 *)(iVar8 + 0x58) = 0;
        if (0 < *(int *)(iVar8 + 0x5c)) {
          piVar6 = (int *)(iVar8 + 0x100);
          do {
            if (*piVar6 != 0) {
              FUN_004367d0(*(undefined4 *)(*piVar6 + 0xc));
              *piVar6 = 0;
            }
            iVar5 = iVar5 + 1;
            piVar6 = piVar6 + 1;
          } while (iVar5 < *(int *)(iVar8 + 0x5c));
        }
        *(undefined4 *)(param_1 + 400) = 0;
      }
      iVar8 = 0;
      if (0 < *(int *)(param_1 + 0x1d8)) {
        piVar6 = (int *)(param_1 + 0x1b8);
        do {
          if (piVar6[-8] != 0) {
            FUN_00482f80(piVar6[-8]);
          }
          if (*piVar6 != 0) {
            FUN_00482f80(*piVar6);
          }
          iVar8 = iVar8 + 1;
          piVar6 = piVar6 + 1;
        } while (iVar8 < *(int *)(param_1 + 0x1d8));
      }
      *(undefined4 *)(param_1 + 0x1dc) = 0xffffffff;
      *(undefined4 *)(param_1 + 0x1d8) = 0;
      *(undefined4 *)(param_1 + 0x1e4) = 0;
    }
    if (*(int *)(param_1 + 0x1d8) < 8) {
      if (param_3 == (char *)0x0) {
        *(undefined4 *)(param_1 + 0x198 + *(int *)(param_1 + 0x1d8) * 4) = 0;
      }
      else {
        uVar3 = 0xffffffff;
        pcVar2 = param_3;
        do {
          if (uVar3 == 0) break;
          uVar3 = uVar3 - 1;
          cVar1 = *pcVar2;
          pcVar2 = pcVar2 + 1;
        } while (cVar1 != '\0');
        pcVar2 = (char *)FUN_00482ef0(~uVar3);
        uVar3 = 0xffffffff;
        do {
          pcVar7 = param_3;
          if (uVar3 == 0) break;
          uVar3 = uVar3 - 1;
          pcVar7 = param_3 + 1;
          cVar1 = *param_3;
          param_3 = pcVar7;
        } while (cVar1 != '\0');
        uVar3 = ~uVar3;
        pcVar7 = pcVar7 + -uVar3;
        pcVar9 = pcVar2;
        for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
          *(undefined4 *)pcVar9 = *(undefined4 *)pcVar7;
          pcVar7 = pcVar7 + 4;
          pcVar9 = pcVar9 + 4;
        }
        for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
          *pcVar9 = *pcVar7;
          pcVar7 = pcVar7 + 1;
          pcVar9 = pcVar9 + 1;
        }
        *(char **)(param_1 + 0x198 + *(int *)(param_1 + 0x1d8) * 4) = pcVar2;
      }
      if (param_2 == (char *)0x0) {
        pcVar2 = (char *)0x0;
      }
      else {
        uVar3 = 0xffffffff;
        pcVar2 = param_2;
        do {
          if (uVar3 == 0) break;
          uVar3 = uVar3 - 1;
          cVar1 = *pcVar2;
          pcVar2 = pcVar2 + 1;
        } while (cVar1 != '\0');
        pcVar2 = (char *)FUN_00482ef0(~uVar3);
        uVar3 = 0xffffffff;
        do {
          pcVar7 = param_2;
          if (uVar3 == 0) break;
          uVar3 = uVar3 - 1;
          pcVar7 = param_2 + 1;
          cVar1 = *param_2;
          param_2 = pcVar7;
        } while (cVar1 != '\0');
        uVar3 = ~uVar3;
        pcVar7 = pcVar7 + -uVar3;
        pcVar9 = pcVar2;
        for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
          *(undefined4 *)pcVar9 = *(undefined4 *)pcVar7;
          pcVar7 = pcVar7 + 4;
          pcVar9 = pcVar9 + 4;
        }
        for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
          *pcVar9 = *pcVar7;
          pcVar7 = pcVar7 + 1;
          pcVar9 = pcVar9 + 1;
        }
      }
      *(char **)(param_1 + 0x1b8 + *(int *)(param_1 + 0x1d8) * 4) = pcVar2;
      *(int *)(param_1 + 0x1d8) = *(int *)(param_1 + 0x1d8) + 1;
    }
  }
  return;
}


