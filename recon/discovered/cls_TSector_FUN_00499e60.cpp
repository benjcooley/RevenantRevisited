// FUN_00499e60_TSector_SaveAllLoaded @ 00499e60 size=41

void FUN_00499e60_TSector_SaveAllLoaded(void)

{
  int iVar1;
  
  iVar1 = 0;
  if (0 < DAT_00668578) {
    do {
      if (*(int *)(DAT_00668588 + iVar1 * 4) != 0) {
        FUN_00498a40_TSector_Save();
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < DAT_00668578);
  }
  return;
}


