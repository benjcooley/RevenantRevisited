// FUN_00451840_TMapPane_DeleteObject @ 00451840 size=528

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00451840_TMapPane_DeleteObject(int param_1,int *param_2)

{
  HANDLE hMutex;
  int iVar1;
  int *piVar2;
  undefined1 auStack_58 [28];
  int local_3c;
  
  if ((param_2 != (int *)0x0) &&
     ((((short)param_2[1] == 0xc || ((short)param_2[1] == 0xb)) && (param_2[0xa1] != 0)))) {
    FUN_004830f0(param_2[0xa1]);
    param_2[0xa1] = 0;
  }
  if (((param_2 == *(int **)(param_1 + 0xdc)) &&
      (*(undefined4 *)(param_1 + 0xdc) = 0, (*(byte *)(param_1 + 0xd8) & 1) != 0)) &&
     (DAT_00667fcc != 0)) {
    *(int *)(param_1 + 0xdc) = DAT_00667fcc;
    *(undefined4 *)(param_1 + 0xd8) = 9;
  }
  if (param_2 == (int *)0x0) {
    FUN_00481c10(s_Tried_to_delete_an_object_not_in_005d03f4,0);
  }
  else {
    iVar1 = param_2[0x14];
    if (-1 < iVar1) {
      FUN_0044cf80_TMapIterator_Initialize(0,0x80,0,0,0xffffffff);
      while (local_3c != 0) {
        if (*(int *)(local_3c + 0x40) == iVar1) goto LAB_004518f8;
        FUN_0044d080_TMapIterator_NextItem();
      }
      local_3c = FUN_0051f330(iVar1);
LAB_004518f8:
      if (local_3c != 0) {
        FUN_00451610_TMapPane_RemoveObject(local_3c);
      }
    }
    iVar1 = (**(code **)(*param_2 + 0x20))();
    if (iVar1 != 0) {
      (**(code **)(*param_2 + 0x2c))();
    }
    iVar1 = (**(code **)(*param_2 + 0xb4))();
    if ((iVar1 != 0) || (param_2[0x19] != 0)) {
      (**(code **)(*param_2 + 0x60))();
      FUN_0046e6d0(param_2);
      return;
    }
    FUN_00584270(param_2,0);
    if (param_2[0x11] != 0) {
      FUN_00452750(param_2,3,0);
      if (DAT_0065844c != 0) {
        FUN_00481e80(DAT_00658310);
        _DAT_0065845c = s_d__revenant_MapPane_cpp_005d0468;
        _DAT_00658474 = 0x800;
      }
      piVar2 = (int *)FUN_00452690_TMapPane_GetInstance(param_2[0x10],0);
      if ((piVar2 != (int *)0x0) && (iVar1 = (**(code **)(*piVar2 + 0xfc))(), iVar1 != 4)) {
        (**(code **)(*piVar2 + 0xf4))(auStack_58);
        FUN_004548a0(auStack_58,iVar1);
      }
      FUN_00499250_TSector_RemoveObject(param_2);
      if (DAT_0065844c != 0) {
        ReleaseMutex(DAT_00658310);
        hMutex = DAT_00658310;
        iVar1 = ReleaseMutex(DAT_00658310);
        while (iVar1 != 0) {
          iVar1 = ReleaseMutex(hMutex);
        }
        _DAT_0065845c = (char *)0x0;
        _DAT_00658474 = 0;
        FUN_0046e6d0(param_2);
        return;
      }
    }
  }
  FUN_0046e6d0(param_2);
  return;
}


