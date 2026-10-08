// Sector_Release @ 0x00498460 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// refcount; saves (0x00498a40) and frees
// FUN_00498460 @ 00498460 size=106

void FUN_00498460(int param_1,int param_2)

{
  int iVar1;
  
  if ((1 < *(int *)(param_1 + 0x94)) && (param_2 == 0)) {
    *(int *)(param_1 + 0x94) = *(int *)(param_1 + 0x94) + -1;
    return;
  }
  FUN_00498a40();
  iVar1 = 0;
  if (0 < DAT_00668578) {
    do {
      if (param_1 == *(int *)(DAT_00668588 + iVar1 * 4)) {
        FUN_0041cb80(iVar1);
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < DAT_00668578);
  }
  if (param_1 != 0) {
    FUN_00498190();
    FUN_004830f0(param_1);
  }
  return;
}


