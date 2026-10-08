// FUN_00426270 @ 00426270 size=216

undefined4 FUN_00426270(undefined4 param_1,int param_2)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  int iVar4;
  undefined4 uVar5;
  
  bVar3 = false;
  bVar1 = false;
  bVar2 = false;
  if (*(int *)(param_2 + 0x10) == 4) {
    do {
      iVar4 = FUN_00479700(&DAT_005cbf80,0);
      if (iVar4 != 0) {
        bVar1 = true;
      }
      iVar4 = FUN_00479700(s_sector_005cbf88,0);
      if (iVar4 != 0) {
        bVar2 = true;
      }
      iVar4 = FUN_00479700(s_script_005cbf90,0);
      if (iVar4 != 0) {
        bVar3 = true;
      }
      FUN_00479580();
    } while (*(int *)(param_2 + 0x10) == 4);
    if (bVar1) {
      uVar5 = 0;
      if (*(int *)(param_2 + 0x10) == 8) {
        uVar5 = *(undefined4 *)(param_2 + 0x14);
        FUN_00479580();
      }
      FUN_0041ee50(s_Loading_game____005cbf98);
      FUN_0047e770(uVar5);
    }
    if (!bVar2) goto LAB_00426326;
  }
  FUN_0041ee50(s_Reloading____005cbfac);
  FUN_004590e0();
LAB_00426326:
  if (bVar3) {
    FUN_0041ee50(s_Reloading_Scripts____005cbfbc);
    FUN_004970b0();
  }
  return 0;
}


