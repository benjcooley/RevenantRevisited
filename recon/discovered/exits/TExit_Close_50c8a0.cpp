// TExit_Close @ 0x0050c8a0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// WriteExitList + free list. Caller: 0x0044d9c0 (TMapPane close)
// FUN_0050c8a0 @ 0050c8a0 size=77

void FUN_0050c8a0(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  FUN_0050cca0();
  puVar2 = DAT_0066d1c4;
  if (DAT_0066d1c4 == (undefined4 *)0x0) {
    DAT_0066d1c4 = (undefined4 *)0x0;
    DAT_0066d24c = 0;
    return;
  }
  do {
    puVar1 = (undefined4 *)puVar2[8];
    FUN_004830f0(*puVar2);
    FUN_004830f0(puVar2);
    puVar2 = puVar1;
  } while (puVar1 != (undefined4 *)0x0);
  DAT_0066d1c4 = (undefined4 *)0x0;
  DAT_0066d24c = 0;
  return;
}


