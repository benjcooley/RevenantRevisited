// TDragonEntAnimator_Build @ 0x0050e060 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// animator vtable 0x5b2764, size 0xfc
// FUN_0050e060 @ 0050e060 size=146

undefined4 * FUN_0050e060(undefined4 param_1)

{
  undefined4 *puVar1;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0a6e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  puVar1 = (undefined4 *)FUN_00482fb0(0xfc);
  local_4 = 0;
  if (puVar1 != (undefined4 *)0x0) {
    FUN_00445940(param_1);
    local_4._0_1_ = 1;
    FUN_0041c7f0(0x10,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    FUN_0041c7f0(0,0x10);
    *puVar1 = &PTR_FUN_005b2764;
    ExceptionList = local_c;
    return puVar1;
  }
  ExceptionList = local_c;
  return (undefined4 *)0x0;
}


