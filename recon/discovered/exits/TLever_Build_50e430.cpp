// TLever_Build @ 0x0050e430 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// object vtable 0x5b2a4c, size 0x108; usedir (+0x104) = 3
// FUN_0050e430 @ 0050e430 size=163

undefined4 * FUN_0050e430(undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0acb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  puVar1 = (undefined4 *)FUN_00482fb0(0x108);
  local_4 = 0;
  if (puVar1 != (undefined4 *)0x0) {
    FUN_0046e1f0(param_1,param_2);
    puVar1[0x37] = 0;
    puVar1[0x38] = 0;
    puVar1[2] = puVar1[2] | 0x8001;
    puVar1[0x39] = 0;
    puVar1[0x3a] = 0;
    *puVar1 = &PTR_FUN_005b2a4c;
    puVar1[0x41] = 3;
    ExceptionList = local_c;
    return puVar1;
  }
  ExceptionList = local_c;
  return (undefined4 *)0x0;
}


