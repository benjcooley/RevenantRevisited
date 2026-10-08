// TCharacter_SetOnExit @ 0x004cdf30 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// OF_ONEXIT (0x100000) + exittimestamp (+0x114) = FrameCount
// FUN_004cdf30 @ 004cdf30 size=37

void __fastcall FUN_004cdf30(int *param_1)

{
  (**(code **)(*param_1 + 0x40))(param_1[2] | 0x100000);
  param_1[0x45] = *(int *)(DAT_00667fd0 + 0x48);
  return;
}


