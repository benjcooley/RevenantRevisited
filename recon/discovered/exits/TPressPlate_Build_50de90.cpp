// TPressPlate_Build @ 0x0050de90 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// object vtable 0x5b2258, size 0xf8
// FUN_0050de90 @ 0050de90 size=153

undefined4 * FUN_0050de90(undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0a1b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  puVar1 = (undefined4 *)FUN_00482fb0(0xf8);
  local_4 = 0;
  if (puVar1 != (undefined4 *)0x0) {
    FUN_0046e1f0(param_1,param_2);
    puVar1[0x37] = 0;
    puVar1[0x38] = 0;
    puVar1[2] = puVar1[2] | 0x8001;
    puVar1[0x39] = 0;
    puVar1[0x3a] = 0;
    *puVar1 = &PTR_FUN_005b2258;
    ExceptionList = local_c;
    return puVar1;
  }
  ExceptionList = local_c;
  return (undefined4 *)0x0;
}


