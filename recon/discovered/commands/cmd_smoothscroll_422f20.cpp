// FUN_00422f20 @ 00422f20 size=135

undefined4 FUN_00422f20(void)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  iVar1 = FUN_00479700(&DAT_005cb644,0);
  if (iVar1 == 0) {
    iVar1 = FUN_00479700(&PTR_DAT_005cb648,0);
    if (iVar1 == 0) {
      uVar2 = 4;
    }
    else if (DAT_005d7a04 != 0) {
      DAT_005d7a04 = 0;
      FUN_004546a0();
      FUN_00478a10();
      return 0;
    }
  }
  else if (DAT_005d7a04 == 0) {
    DAT_005d7a04 = 1;
    FUN_004546a0();
    FUN_00478a10();
    return 0;
  }
  FUN_00478a10();
  return uVar2;
}


