// FUN_005338a0_TDeathScreen_Initialize @ 005338a0 size=106

undefined4 __fastcall FUN_005338a0_TDeathScreen_Initialize(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  *(undefined4 *)(param_1 + 0x54) = 0;
  DAT_0066f6f4 = FUN_0047f670(s_death_dat_005e3ebc,0xffffffff,0);
  iVar1 = FUN_005339b0_TDeathPane_Initialize();
  if (iVar1 == 0) {
    FUN_00481c10(s_Trouble_initializing_Death_pane_005e3ec8,0);
  }
  uVar2 = FUN_0046d710(s_cursor_005e3ee8);
  FUN_0043a020(uVar2);
  FUN_0048ed90(&DAT_0066f500,0xffffffff);
  return 1;
}


