// FUN_00421360 @ 00421360 size=95

undefined4 FUN_00421360(void)

{
  int iVar1;
  
  iVar1 = FUN_00479700(&DAT_005cb0f0,0);
  if (iVar1 == 0) {
    iVar1 = FUN_004406e0();
    if (iVar1 == 0) {
      FUN_0041ee50(s_Nothing_to_deselect__005cb0f4);
    }
  }
  else {
    iVar1 = FUN_004406e0();
    if (iVar1 != 0) {
      do {
        iVar1 = FUN_004406e0();
      } while (iVar1 != 0);
      FUN_00479580();
      return 0;
    }
  }
  FUN_00479580();
  return 0;
}


