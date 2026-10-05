// TDragonEntAnimator_Dtor @ 0x0050e120 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// FUN_0050e120 @ 0050e120 size=79

void __fastcall FUN_0050e120(undefined4 *param_1)

{
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0a88;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *param_1 = &PTR_FUN_005b2764;
  local_4 = 0;
  FUN_0040de10();
  local_4 = 0xffffffff;
  FUN_0040dcf0();
  ExceptionList = local_c;
  return;
}


