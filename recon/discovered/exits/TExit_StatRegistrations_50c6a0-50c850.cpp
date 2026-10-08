// TExit_StatRegistrations_50c6a0-50c850 -- retail Revenant.exe, raw Ghidra decompiles (see docs/gameflow/forensics/EXITS.md)
// DEFSTAT/DEFOBJSTAT registrations; the SStatEntry each writes (ECX) is listed in EXITS.md 1.2

// FUN_0050c6a0 @ 0050c6a0 size=36

void FUN_0050c6a0(void)

{
  FUN_00473e40(&DAT_0066d1f0,s_Openable_005e1644,&DAT_005e163c,0,0,0,1,0);
  return;
}



// FUN_0050c6d0 @ 0050c6d0 size=39

void FUN_0050c6d0(void)

{
  FUN_00473e40(&DAT_0066d1f0,s_Facing_005e1658,&DAT_005e1650,1,0,0,0xff,0);
  return;
}



// FUN_0050c700 @ 0050c700 size=36

void FUN_0050c700(void)

{
  FUN_00473e40(&DAT_0066d1f0,s_UseCenter_005e1664,&DAT_005e1660,2,0,0,2,0);
  return;
}



// FUN_0050c730 @ 0050c730 size=36

void FUN_0050c730(void)

{
  FUN_00473e40(&DAT_0066d1f0,s_StopMoving_005e1678,&DAT_005e1670,3,0,1,1,0);
  return;
}



// FUN_0050c760 @ 0050c760 size=39

void FUN_0050c760(void)

{
  FUN_00473e40(&DAT_0066d1f0,s_Delay_005e1688,&DAT_005e1684,4,0,0,1000,0);
  return;
}



// FUN_0050c790 @ 0050c790 size=36

void FUN_0050c790(void)

{
  FUN_00473e40(&DAT_0066d1f0,s_TileFlags_005e1698,&DAT_005e1690,5,0,1,0x20,0);
  return;
}



// FUN_0050c7c0 @ 0050c7c0 size=36

void FUN_0050c7c0(void)

{
  FUN_00473e40(&DAT_0066d1f0,s_Locked_005e16ac,&DAT_005e16a4,0,0,0,1,1);
  return;
}



// FUN_0050c7f0 @ 0050c7f0 size=39

void FUN_0050c7f0(void)

{
  FUN_00473e40(&DAT_0066d1f0,s_KeyId_005e16b8,&DAT_005e16b4,1,0,0,100000,1);
  return;
}



// FUN_0050c820 @ 0050c820 size=39

void FUN_0050c820(void)

{
  FUN_00473e40(&DAT_0066d1f0,s_PickDifficulty_005e16c8,&DAT_005e16c0,2,0,0,100000,1);
  return;
}



// FUN_0050c850 @ 0050c850 size=36

void FUN_0050c850(void)

{
  FUN_00473e40(&DAT_0066d1f0,s_AutoActivate_005e16e0,&DAT_005e16d8,3,0,0,1,1);
  return;
}



// ---- Disassembly: the SStatEntry (ECX) each registration constructs ----
// 0050c6aa  PUSH 0x5e163c
// 0050c6af  PUSH 0x5e1644
// 0050c6b9  MOV ECX,0x66d238
// 0050c6dd  PUSH 0x5e1650
// 0050c6e2  PUSH 0x5e1658
// 0050c6ec  MOV ECX,0x66d1c0
// 0050c70a  PUSH 0x5e1660
// 0050c70f  PUSH 0x5e1664
// 0050c719  MOV ECX,0x66d1c8
// 0050c73a  PUSH 0x5e1670
// 0050c73f  PUSH 0x5e1678
// 0050c749  MOV ECX,0x66d1bc
// 0050c76d  PUSH 0x5e1684
// 0050c772  PUSH 0x5e1688
// 0050c77c  MOV ECX,0x66d1e8
// 0050c79a  PUSH 0x5e1690
// 0050c79f  PUSH 0x5e1698
// 0050c7a9  MOV ECX,0x66d248
// 0050c7ca  PUSH 0x5e16a4
// 0050c7cf  PUSH 0x5e16ac
// 0050c7d9  MOV ECX,0x66d22c
// 0050c7fd  PUSH 0x5e16b4
// 0050c802  PUSH 0x5e16b8
// 0050c80c  MOV ECX,0x66d1d8
// 0050c82d  PUSH 0x5e16c0
// 0050c832  PUSH 0x5e16c8
// 0050c83c  MOV ECX,0x66d1b8
// 0050c85a  PUSH 0x5e16d8
// 0050c85f  PUSH 0x5e16e0
// 0050c869  MOV ECX,0x66d1cc
