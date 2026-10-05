// TPressPlate_Activate_Slot280 @ 0x0050da20 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// lands in a NEW slot 0x280 (does not override 0x248); no caller found
// FUN_0050da20 @ 0050da20 size=40

undefined4 __thiscall FUN_0050da20(int *param_1,undefined4 param_2)

{
  FUN_0050d3a0(DAT_00667fcc,param_2);
  (**(code **)(*param_1 + 0x18))(1);
  return 1;
}


