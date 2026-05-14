// FUN_00455f90 @ 00455f90 size=329

void FUN_00455f90(void)

{
  HANDLE hMutex;
  HANDLE pvVar1;
  int iVar2;
  undefined4 *puVar3;
  
  if (DAT_0065844c != 0) {
    if ((DAT_00658324 != 0) && ((DAT_00658d90 != 0 || (DAT_00658d94 != 0)))) {
      GetTickCount();
      pvVar1 = DAT_006584b4;
      iVar2 = ReleaseMutex(DAT_006584b4);
      hMutex = DAT_00658310;
      while (DAT_00658310 = hMutex, iVar2 != 0) {
        iVar2 = ReleaseMutex(pvVar1);
        hMutex = DAT_00658310;
      }
      iVar2 = ReleaseMutex(hMutex);
      pvVar1 = DAT_006584b8;
      while (DAT_006584b8 = pvVar1, iVar2 != 0) {
        iVar2 = ReleaseMutex(hMutex);
        pvVar1 = DAT_006584b8;
      }
      iVar2 = ReleaseMutex(pvVar1);
      while (iVar2 != 0) {
        iVar2 = ReleaseMutex(pvVar1);
      }
      if (DAT_00658478 == 0) {
        while (DAT_00658324 != 0) {
          SetEvent(DAT_00658458);
          WaitForSingleObject(DAT_006584a8,0xffffffff);
          ResetEvent(DAT_006584a8);
        }
      }
      else {
        ResetEvent(DAT_00658458);
        if ((DAT_00658324 != 0) && (DAT_006682bc == 0)) {
          iVar2 = 0;
          if (0 < DAT_00658438) {
            puVar3 = &DAT_006584c8;
            do {
              FUN_00456810(puVar3,puVar3[4]);
              iVar2 = iVar2 + 1;
              puVar3 = puVar3 + 7;
            } while (iVar2 < DAT_00658438);
          }
          DAT_00658324 = 0;
          DAT_006584c0 = 0;
          SetEvent(DAT_006584a8);
        }
      }
      DAT_006584c0 = 0;
      GetTickCount();
    }
    FUN_00482130();
    DAT_00658438 = 0;
    FUN_00482140();
  }
  return;
}


