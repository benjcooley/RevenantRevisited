// FUN_00451610_TMapPane_RemoveObject @ 00451610 size=557

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int * __thiscall FUN_00451610_TMapPane_RemoveObject(int param_1,int *param_2)

{
  HANDLE hMutex;
  int *piVar1;
  int iVar2;
  undefined1 auStack_58 [28];
  int *local_3c;
  
  if (param_2 == (int *)0x0) {
    return (int *)0x0;
  }
  iVar2 = param_2[0x14];
  if (-1 < iVar2) {
    FUN_0044cf80_TMapIterator_Initialize(0,0x80,0,0,0xffffffff);
    piVar1 = local_3c;
    while (local_3c = piVar1, piVar1 != (int *)0x0) {
      if (piVar1[0x10] == iVar2) goto LAB_00451670;
      FUN_0044d080_TMapIterator_NextItem();
      piVar1 = local_3c;
    }
    piVar1 = (int *)FUN_0051f330(iVar2);
LAB_00451670:
    if (piVar1 != (int *)0x0) {
      FUN_00451610_TMapPane_RemoveObject(piVar1);
    }
  }
  iVar2 = (**(code **)(*param_2 + 0x20))();
  if (iVar2 != 0) {
    (**(code **)(*param_2 + 0x2c))();
  }
  iVar2 = (**(code **)(*param_2 + 0xb4))();
  if ((iVar2 == 0) && (param_2[0x19] == 0)) {
    FUN_00584270(param_2,0);
    if (param_2[0x11] != 0) {
      FUN_00452750(param_2,3,0);
      if (DAT_0065844c != 0) {
        FUN_00481e80(DAT_00658310);
        _DAT_0065845c = s_d__revenant_MapPane_cpp_005d0468;
        _DAT_00658474 = 0x800;
      }
      iVar2 = param_2[0x10];
      if (-1 < iVar2) {
        FUN_0045f800_TMapIterator_Ctor(0,0x80,0,0,0xffffffff);
        while (local_3c != (int *)0x0) {
          if (local_3c[0x10] == iVar2) goto LAB_0045174f;
          FUN_0044d080_TMapIterator_NextItem();
        }
        local_3c = (int *)FUN_0051f330(iVar2);
LAB_0045174f:
        if ((((local_3c != (int *)0x0) && (iVar2 = (**(code **)(*local_3c + 0xfc))(), iVar2 != 4))
            && ((**(code **)(*local_3c + 0xf4))(auStack_58), *(int *)(param_1 + 0x50) == 0)) &&
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
      FUN_00499250_TSector_RemoveObject(param_2);
      if (DAT_0065844c != 0) {
        ReleaseMutex(DAT_00658310);
        hMutex = DAT_00658310;
        iVar2 = ReleaseMutex(DAT_00658310);
        while (iVar2 != 0) {
          iVar2 = ReleaseMutex(hMutex);
        }
        _DAT_0065845c = (char *)0x0;
        _DAT_00658474 = 0;
        return param_2;
      }
    }
  }
  else {
    (**(code **)(*param_2 + 0x60))();
  }
  return param_2;
}


