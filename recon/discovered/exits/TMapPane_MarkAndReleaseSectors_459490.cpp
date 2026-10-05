// TMapPane_MarkAndReleaseSectors @ 0x00459490 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// keeps sectors around the centre and every active player; releases the rest
// FUN_00459490 @ 00459490 size=799

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00459490(int param_1)

{
  undefined4 uVar1;
  HANDLE pvVar2;
  int iVar3;
  int *piVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int *local_8;
  
  iVar8 = 0;
  if (DAT_0065844c != 0) {
    FUN_00481e80(DAT_00658310);
    _DAT_0065845c = s_d__revenant_MapPane_cpp_005d06b0;
    _DAT_00658474 = 0x136d;
  }
  piVar4 = DAT_00668588;
  iVar6 = DAT_00668578;
  if (0 < DAT_00668578) {
    do {
      iVar3 = *piVar4;
      iVar6 = iVar6 + -1;
      uVar1 = *(undefined4 *)(iVar3 + 0x98);
      *(undefined4 *)(iVar3 + 0x98) = 0;
      *(undefined4 *)(iVar3 + 0x9c) = uVar1;
      *(undefined4 *)(iVar3 + 0xa4) = 0;
      *(undefined4 *)(iVar3 + 0xa0) = 0;
      piVar4 = piVar4 + 1;
    } while (iVar6 != 0);
  }
  FUN_00459330(*(undefined4 *)(param_1 + 0x9c),*(int *)(param_1 + 0xb0) >> 10,
               *(int *)(param_1 + 0xb4) >> 10,0);
  iVar9 = 0;
  iVar3 = FUN_0051ee70(0);
  iVar6 = DAT_00668578;
  if (0 < iVar3) {
    do {
      iVar6 = FUN_0051eea0(iVar9,0);
      if (((((DAT_0066829c == 0) || (DAT_00676828 == 0)) || (DAT_0067682c != 0)) ||
          (iVar6 == DAT_00667fcc)) && ((iVar6 != 0 && ((*(byte *)(iVar6 + 0x36c) & 1) != 0)))) {
        uVar5 = *(int *)(iVar6 + 0x10) >> 10;
        uVar7 = *(int *)(iVar6 + 0x14) >> 10;
        if ((uVar5 < 0x20) && (uVar7 < 0x20)) {
          FUN_00459330(*(undefined2 *)(iVar6 + 0xe),uVar5,uVar7,iVar6);
        }
      }
      iVar9 = iVar9 + 1;
      iVar3 = FUN_0051ee70(0);
      iVar6 = DAT_00668578;
    } while (iVar9 < iVar3);
  }
  while (iVar6 = iVar6 + -1, -1 < iVar6) {
    iVar3 = DAT_00668588[iVar6];
    if (((iVar3 != 0) && (*(int *)(iVar3 + 0x98) == 0)) && (*(int *)(iVar3 + 0x10) == 0)) {
      FUN_0045f7e0(iVar3 + 0xb8);
      iVar9 = FUN_0045f7a0();
      while (iVar9 != 0) {
        if ((int *)*local_8 != (int *)0x0) {
          (**(code **)(*(int *)*local_8 + 0x128))();
        }
        local_8 = local_8 + 1;
        iVar9 = FUN_0045f7a0();
      }
      FUN_00498460(iVar3,0);
    }
  }
  if (DAT_0065844c != 0) {
    ReleaseMutex(DAT_00658310);
    pvVar2 = DAT_00658310;
    iVar6 = ReleaseMutex(DAT_00658310);
    while (iVar6 != 0) {
      iVar6 = ReleaseMutex(pvVar2);
    }
    _DAT_0065845c = (char *)0x0;
    _DAT_00658474 = 0;
  }
  iVar6 = DAT_00668578;
  if (0 < DAT_00668578) {
    do {
      iVar3 = DAT_00668588[iVar8];
      if (((*(int *)(iVar3 + 0xa4) == 1) && (*(int *)(iVar3 + 0xa0) == 0)) &&
         (iVar9 = 0, 0 < *(int *)(iVar3 + 0xb8))) {
        do {
          piVar4 = *(int **)(*(int *)(iVar3 + 200) + iVar9 * 4);
          if ((piVar4 != (int *)0x0) && ((piVar4[2] & 0x4000U) != 0)) {
            (**(code **)(*piVar4 + 0x128))();
          }
          iVar9 = iVar9 + 1;
          iVar6 = DAT_00668578;
        } while (iVar9 < *(int *)(iVar3 + 0xb8));
      }
      iVar8 = iVar8 + 1;
    } while (iVar8 < iVar6);
  }
  iVar8 = 0;
  if (((DAT_0066829c != 0) && (DAT_00676828 != 0)) &&
     ((DAT_0067682c == 0 && ((DAT_006768d4 != '\0' && (0 < iVar6)))))) {
    do {
      iVar6 = DAT_00668588[iVar8];
      if ((*(int *)(iVar6 + 0x9c) == 0) && (*(int *)(iVar6 + 0x98) == 1)) {
        if (DAT_0065844c != 0) {
          FUN_00481e80(DAT_00658310);
          _DAT_0065845c = s_d__revenant_MapPane_cpp_005d06c8;
          _DAT_00658474 = 0x13bc;
          if (DAT_0065844c != 0) {
            ReleaseMutex(DAT_00658310);
            pvVar2 = DAT_00658310;
            iVar3 = ReleaseMutex(DAT_00658310);
            while (iVar3 != 0) {
              iVar3 = ReleaseMutex(pvVar2);
            }
            _DAT_0065845c = (char *)0x0;
            _DAT_00658474 = 0;
          }
        }
        FUN_00586170(DAT_00667fcc,iVar6);
      }
      iVar8 = iVar8 + 1;
    } while (iVar8 < DAT_00668578);
  }
  return;
}


