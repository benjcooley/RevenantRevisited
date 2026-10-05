// TObjectInstance_RunScript @ 0x00471260 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// script Continue now
// FUN_00471260 @ 00471260 size=44

void __fastcall FUN_00471260(int *param_1)

{
  undefined4 uVar1;
  
  if ((param_1[0x21] != 0) && ((param_1[2] & 0x200000U) == 0)) {
    uVar1 = (**(code **)(*param_1 + 0x154))();
    FUN_004933d0(uVar1);
  }
  return;
}


