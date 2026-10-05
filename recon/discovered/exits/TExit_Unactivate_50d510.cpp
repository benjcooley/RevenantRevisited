// TExit_Unactivate @ 0x0050d510 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x24c
// FUN_0050d510 @ 0050d510 size=26

void __fastcall FUN_0050d510(int *param_1)

{
  int iVar1;
  
  iVar1 = (**(code **)(*param_1 + 0x1f0))();
  if (iVar1 != 0) {
    FUN_0050d530(2);
  }
  return;
}


