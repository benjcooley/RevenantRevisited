// TMapPane_RemoveFromSector @ 0x00451b10 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// inferred role (CheckPos / 0x00459b80 take a player out of its sector)
// FUN_00451b10 @ 00451b10 size=413

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __thiscall FUN_00451b10(int param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  HANDLE hMutex;
  int iVar3;
  undefined4 uStack_58;
  undefined4 uStack_54;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  int *local_3c;
  
  FUN_00584270(param_2,0);
  if (*(int *)(param_2 + 0x44) == 0) {
    return param_2;
  }
  FUN_00452750(param_2,3,0);
  if (DAT_0065844c != 0) {
    FUN_00481e80(DAT_00658310);
    _DAT_0065845c = s_d__revenant_MapPane_cpp_005d0468;
    _DAT_00658474 = 0x800;
  }
  iVar3 = *(int *)(param_2 + 0x40);
  if (-1 < iVar3) {
    FUN_0044cf80(0,0x80,0,0,0xffffffff);
    while (local_3c != (int *)0x0) {
      if (local_3c[0x10] == iVar3) goto LAB_00451bc2;
      FUN_0044d080();
    }
    local_3c = (int *)FUN_0051f330(iVar3);
LAB_00451bc2:
    if ((((local_3c != (int *)0x0) && (iVar3 = (**(code **)(*local_3c + 0xfc))(), iVar3 != 4)) &&
        ((**(code **)(*local_3c + 0xf4))(&uStack_58), *(int *)(param_1 + 0x50) == 0)) && (iVar3 < 4)
       ) {
      iVar2 = *(int *)(param_1 + 0x130);
      if (iVar2 < 0x40) {
        puVar1 = (undefined4 *)(param_1 + (iVar2 + 0xb) * 0x1c);
        *puVar1 = uStack_58;
        puVar1[1] = uStack_54;
        puVar1[2] = uStack_50;
        puVar1[3] = uStack_4c;
        *(int *)(param_1 + 0x144 + iVar2 * 0x1c) = iVar3;
        *(int *)(param_1 + 0x130) = *(int *)(param_1 + 0x130) + 1;
      }
      else {
        FUN_004546a0();
        *(undefined4 *)(param_1 + 0x130) = 0;
      }
    }
  }
  FUN_00499250(param_2);
  if (DAT_0065844c != 0) {
    ReleaseMutex(DAT_00658310);
    hMutex = DAT_00658310;
    iVar3 = ReleaseMutex(DAT_00658310);
    while (iVar3 != 0) {
      iVar3 = ReleaseMutex(hMutex);
    }
    _DAT_0065845c = (char *)0x0;
    _DAT_00658474 = 0;
  }
  return param_2;
}


