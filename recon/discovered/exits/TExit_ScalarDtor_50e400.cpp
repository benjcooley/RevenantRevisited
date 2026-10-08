// TExit_ScalarDtor @ 0x0050e400 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x0
// FUN_0050e400 @ 0050e400 size=30

undefined4 __thiscall FUN_0050e400(undefined4 param_1,byte param_2)

{
  thunk_FUN_0046e420();
  if ((param_2 & 1) != 0) {
    FUN_004830f0(param_1);
  }
  return param_1;
}


