// TLever_Operate @ 0x0050de60 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x250
// FUN_0050de60 @ 0050de60 size=36

void __fastcall FUN_0050de60(int *param_1)

{
  if (((short)param_1[3] != 1) && ((short)param_1[3] != 3)) {
    (**(code **)(*param_1 + 0x18))(3);
    return;
  }
  (**(code **)(*param_1 + 0x18))(2);
  return;
}


