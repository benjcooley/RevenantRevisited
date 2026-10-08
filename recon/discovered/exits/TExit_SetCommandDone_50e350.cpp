// TExit_SetCommandDone @ 0x0050e350 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x158 (same body as the base 0x00471b50)
// FUN_0050e350 @ 0050e350 size=13

void __thiscall FUN_0050e350(int param_1,undefined4 param_2)

{
  *(undefined4 *)(param_1 + 0x80) = param_2;
  return;
}


