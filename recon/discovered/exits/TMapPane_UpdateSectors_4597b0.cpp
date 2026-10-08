// TMapPane_UpdateSectors @ 0x004597b0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// loads missing sectors; text-bar LOADMAPMSG + progress
// FUN_004597b0 @ 004597b0 size=577

void __fastcall FUN_004597b0(int param_1)

{
  int *piVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  
  iVar5 = DAT_00667fcc;
  bVar2 = false;
  if (DAT_00667fcc != 0) {
    iVar4 = FUN_0045f770(3);
    iVar3 = DAT_00667fcc;
    if ((iVar4 != 0) ||
       ((*(int **)(iVar5 + 0xe0) != (int *)0x0 && (**(int **)(iVar5 + 0xe0) == 0x19)))) {
      iVar5 = FUN_0045f770(3);
      if (((iVar5 != 0) ||
          ((piVar1 = *(int **)(iVar3 + 0xe0), piVar1 != (int *)0x0 && (*piVar1 == 0x19)))) &&
         (*(int *)(*(int *)(iVar3 + 0xe0) + 0x44) != 0)) {
        bVar2 = true;
      }
    }
  }
  if (((DAT_005d7a30 != 0) &&
      ((((*(int *)(param_1 + 0x68) != *(int *)(param_1 + 0x60) ||
         (*(int *)(param_1 + 0x6c) != *(int *)(param_1 + 100))) ||
        (*(int *)(param_1 + 0x9c) != *(int *)(param_1 + 0xa0))) || (*(int *)(param_1 + 0x50) != 0)))
      ) && (!bVar2)) {
    iVar5 = FUN_00499db0(param_1 + 0xb0,*(undefined4 *)(param_1 + 0x9c));
    if (iVar5 == 0) {
      if ((DAT_0065c610 != 0) && (DAT_0065c618 == 0)) {
        iVar5 = (**(code **)(DAT_0065c5d0 + 0x40))();
        if ((iVar5 != 0) && (0 < *(int *)(DAT_00667fd0 + 0x48))) {
          uVar6 = FUN_0049d800(s_loadmapmsg_005d06e0);
          FUN_0054ca20(uVar6);
          FUN_0054cbb0();
          FUN_00491990();
        }
      }
      if ((DAT_0066829c != 0) && (DAT_00667fcc != 0)) {
        iVar5 = FUN_0057d9d0(DAT_00667fcc,0x1d,1,0,0);
        if (iVar5 != 0) {
          FUN_0057dc70();
        }
        FUN_0051d680(*(uint *)(DAT_00667fcc + 0x36c) | 2);
      }
      FUN_004997d0(param_1 + 0xb0,*(undefined4 *)(param_1 + 0x9c),&LAB_00459a00);
      if ((((DAT_0066829c != 0) && (DAT_00667fcc != 0)) &&
          (*(int **)(DAT_00667fcc + 0xd8) != (int *)0x0)) &&
         (((iVar5 = **(int **)(DAT_00667fcc + 0xd8), iVar5 == 2 || (iVar5 == 4)) || (iVar5 == 0x1a))
         )) {
        FUN_0051d680(*(uint *)(DAT_00667fcc + 0x36c) & 0xfffffffd);
        FUN_00583e80(DAT_00667fcc,0x1c,*(undefined4 *)(DAT_00667fcc + 0xb0),1,0);
      }
      if ((DAT_0065c610 != 0) && (DAT_0065c618 == 0)) {
        iVar5 = (**(code **)(DAT_0065c5d0 + 0x40))();
        if ((iVar5 != 0) && (0 < *(int *)(DAT_00667fd0 + 0x48))) {
          FUN_0054cad0();
          FUN_0054cbb0();
          FUN_00491990();
        }
      }
      FUN_004546a0();
    }
  }
  return;
}


