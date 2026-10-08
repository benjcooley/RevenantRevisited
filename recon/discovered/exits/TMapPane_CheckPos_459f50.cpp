// TMapPane_CheckPos @ 0x00459f50 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// static (object, pos, level)
// FUN_00459f50 @ 00459f50 size=673

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_00459f50(int *param_1,int *param_2,uint param_3)

{
  HANDLE hMutex;
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar1 = (**(code **)(*param_1 + 0xb4))();
  if (iVar1 != 0) {
    return param_1[0x10];
  }
  if ((param_3 == 0xffffffff) || ((param_1[2] & 0x80000U) == 0)) {
    param_3 = (uint)*(ushort *)((int)param_1 + 0xe);
  }
  if (*param_2 < 0) {
    *param_2 = 0;
  }
  else if (0x7fff < *param_2) {
    *param_2 = 0x8000;
  }
  if (param_2[1] < 0) {
    param_2[1] = 0;
  }
  else if (0x7fff < param_2[1]) {
    param_2[1] = 0x8000;
  }
  iVar1 = param_2[1] >> 10;
  iVar5 = *param_2 >> 10;
  if ((param_1[2] & 0x80000U) == 0) {
    iVar2 = FUN_00499e10(param_3,iVar5,iVar1);
    if (iVar2 == 0) {
      iVar5 = (param_1[4] >> 10) * 0x400;
      iVar6 = (param_1[5] >> 10) * 0x400;
      iVar2 = ((param_1[4] >> 10) + 1) * 0x400;
      iVar1 = ((param_1[5] >> 10) + 1) * 0x400;
      iVar4 = *param_2;
      if (*param_2 <= iVar5) {
        iVar4 = iVar5;
      }
      if ((iVar2 <= iVar4) || (iVar2 = *param_2, iVar5 < *param_2)) {
        iVar5 = iVar2;
      }
      iVar2 = param_2[1];
      *param_2 = iVar5;
      iVar4 = DAT_0066829c;
      iVar3 = iVar2;
      if (iVar2 <= iVar6) {
        iVar3 = iVar6;
      }
      if ((iVar3 < iVar1) && (iVar1 = iVar2, iVar2 <= iVar6)) {
        iVar1 = iVar6;
      }
      iVar5 = iVar5 >> 10;
      param_2[1] = iVar1;
      iVar1 = iVar1 >> 10;
      if (((iVar4 != 0) && (DAT_00676828 != 0)) && (DAT_0067682c == 0)) {
        (**(code **)(*param_1 + 0x40))(param_1[2] | 0x1000);
      }
    }
  }
  else {
    iVar2 = FUN_00499e10(param_3,iVar5,iVar1);
    if (iVar2 == 0) {
      if (param_1 != (int *)0x0) {
        if ((-1 < param_1[0x14]) && (iVar1 = FUN_00452690(param_1[0x14],0), iVar1 != 0)) {
          FUN_00451610(iVar1);
        }
        iVar1 = (**(code **)(*param_1 + 0x20))();
        if (iVar1 != 0) {
          (**(code **)(*param_1 + 0x2c))();
        }
        iVar1 = (**(code **)(*param_1 + 0xb4))();
        if ((iVar1 == 0) && (param_1[0x19] == 0)) {
          FUN_00451b10(param_1);
          return 0;
        }
        (**(code **)(*param_1 + 0x60))();
      }
      return 0;
    }
  }
  if (((param_3 == *(ushort *)((int)param_1 + 0xe)) && (iVar5 == param_1[4] >> 10)) &&
     (iVar1 == param_1[5] >> 10)) {
    return param_1[0x10];
  }
  if (param_1 != (int *)0x0) {
    iVar2 = param_1[0x11];
    iVar1 = FUN_00499e10(param_3,iVar5,iVar1);
    if ((iVar2 != 0) && (iVar1 != 0)) {
      if (iVar2 == iVar1) {
        return param_1[0x10];
      }
      if (DAT_0065844c != 0) {
        FUN_00481e80(DAT_00658310);
        _DAT_0065845c = s_d__revenant_MapPane_cpp_005d06ec;
        _DAT_00658474 = 0x1512;
      }
      FUN_00499250(param_1);
      FUN_00498fb0(param_1,0xffffffff);
      if (DAT_0065844c != 0) {
        ReleaseMutex(DAT_00658310);
        hMutex = DAT_00658310;
        iVar1 = ReleaseMutex(DAT_00658310);
        while (iVar1 != 0) {
          iVar1 = ReleaseMutex(hMutex);
        }
        _DAT_0065845c = (char *)0x0;
        _DAT_00658474 = 0;
      }
      return param_1[0x10];
    }
  }
  return -1;
}


