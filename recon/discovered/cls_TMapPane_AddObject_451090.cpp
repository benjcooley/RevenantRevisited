// FUN_00451090_TMapPane_AddObject @ 00451090 size=578

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall FUN_00451090_TMapPane_AddObject(int param_1,int param_2)

{
  HANDLE hMutex;
  int iVar1;
  int iVar2;
  int iVar3;
  DWORD DVar4;
  uint uVar5;
  undefined1 auStack_58 [28];
  int *local_3c;
  
  iVar1 = param_2;
  if (*(int *)(param_2 + 0x44) != 0) {
    return 0xffffffff;
  }
  iVar2 = FUN_00499e10_TSector_FindLoadedSector(*(undefined2 *)(param_2 + 0xe),*(int *)(param_2 + 0x10) >> 10,
                       *(int *)(param_2 + 0x14) >> 10);
  if (iVar2 == 0) {
    return 0xffffffff;
  }
  if (DAT_0065844c != 0) {
    FUN_00481e80(DAT_00658310);
    _DAT_0065845c = s_d__revenant_MapPane_cpp_005d03c4;
    _DAT_00658474 = 0x727;
  }
  FUN_00498fb0_TSector_AddObject(iVar1,0xffffffff);
  if (DAT_0065844c != 0) {
    ReleaseMutex(DAT_00658310);
    hMutex = DAT_00658310;
    iVar2 = ReleaseMutex(DAT_00658310);
    while (iVar2 != 0) {
      iVar2 = ReleaseMutex(hMutex);
    }
    _DAT_0065845c = (char *)0x0;
    _DAT_00658474 = 0;
  }
  if (*(int *)(iVar1 + 0x40) < 0) {
    FUN_0058c5a4(&param_2);
    iVar2 = param_2 + -0x34d6574c;
    if (iVar2 < 0) {
      iVar2 = -iVar2;
    }
    iVar3 = _rand();
    DVar4 = GetTickCount();
    FUN_0058c575(iVar3 + DVar4);
    uVar5 = _rand();
    if (DAT_00666970 == DAT_00658d8c) {
      DAT_00658d8c = DAT_00658d8c + 1;
    }
    if (0x1f < (int)DAT_00658d8c) {
      DAT_00658d8c = 0;
    }
    *(uint *)(iVar1 + 0x40) = (DAT_00658d8c & 0x1f) << 0x1a | (uVar5 | iVar2 << 0xf) & 0x7fffffff;
  }
  if (*(short *)(iVar1 + 0x7c) < 0) {
    FUN_00452750(iVar1,0,0);
    iVar2 = *(int *)(iVar1 + 0x40);
    if (-1 < iVar2) {
      FUN_0045f800_TMapIterator_Ctor(0,0x80,0,0,0xffffffff);
      while (local_3c != (int *)0x0) {
        if (local_3c[0x10] == iVar2) goto LAB_0045122a;
        FUN_0044d080_TMapIterator_NextItem();
      }
      local_3c = (int *)FUN_0051f330(iVar2);
LAB_0045122a:
      if ((((local_3c != (int *)0x0) && (iVar2 = (**(code **)(*local_3c + 0xfc))(), iVar2 != 4)) &&
          ((**(code **)(*local_3c + 0xf4))(auStack_58), *(int *)(param_1 + 0x50) == 0)) &&
         (iVar2 < 4)) {
        if (*(int *)(param_1 + 0x130) < 0x40) {
          FUN_0041c700(auStack_58);
          *(int *)(param_1 + 0x144 + *(int *)(param_1 + 0x130) * 0x1c) = iVar2;
          *(int *)(param_1 + 0x130) = *(int *)(param_1 + 0x130) + 1;
        }
        else {
          FUN_004546a0();
          *(undefined4 *)(param_1 + 0x130) = 0;
        }
      }
    }
  }
  FUN_00584220(iVar1,0);
  return *(undefined4 *)(iVar1 + 0x40);
}


