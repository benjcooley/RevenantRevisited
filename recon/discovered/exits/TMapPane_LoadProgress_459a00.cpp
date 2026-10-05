// TMapPane_LoadProgress @ 0x00459a00 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// sector-load progress callback (text bar bar)
// FUN_00459a00 @ 00459a00 size=111

void FUN_00459a00(int param_1)

{
  int iVar1;
  
  if ((DAT_0065c610 != 0) && (DAT_0065c618 == 0)) {
    iVar1 = (**(code **)(DAT_0065c5d0 + 0x40))();
    if ((iVar1 != 0) && (0 < *(int *)(DAT_00667fd0 + 0x48))) {
      iVar1 = (param_1 * 0xb4) / 1000;
      FUN_0054ca60(iVar1,iVar1);
      FUN_0054cbb0();
      FUN_00491990();
      return;
    }
  }
  return;
}


