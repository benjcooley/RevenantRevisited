// TTrapPressPlate_Use @ 0x00524fe0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0xbc
// FUN_00524fe0 @ 00524fe0 size=39

void __thiscall FUN_00524fe0(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = FUN_004705f0(param_2,param_3);
  if (iVar1 == 0) {
    *(undefined4 *)(param_1 + 0xd8) = 1;
  }
  return;
}


