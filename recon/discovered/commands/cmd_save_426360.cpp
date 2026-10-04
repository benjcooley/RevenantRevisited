// FUN_00426360 @ 00426360 size=770

undefined4 FUN_00426360(int param_1,int param_2)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  bool bVar4;
  bool bVar5;
  bool bVar6;
  bool bVar7;
  bool bVar8;
  int iVar9;
  undefined4 uVar10;
  char *pcVar11;
  
  FUN_00447aa0();
  bVar5 = false;
  if (*(int *)(param_2 + 0x10) == 4) {
    bVar3 = false;
    bVar2 = false;
    bVar8 = false;
    bVar7 = false;
    bVar4 = false;
    bVar1 = false;
    bVar6 = false;
    do {
      iVar9 = FUN_00479700(&DAT_005cbfd4,0);
      if (iVar9 != 0) {
        bVar5 = true;
      }
      iVar9 = FUN_00479700(&DAT_005cbfdc,0);
      if (iVar9 != 0) {
        bVar6 = true;
      }
      iVar9 = FUN_00479700(s_headers_005cbfe0,0);
      if (iVar9 != 0) {
        bVar1 = true;
      }
      iVar9 = FUN_00479700(s_quickload_005cbfe8,0);
      if (iVar9 != 0) {
        bVar4 = true;
      }
      iVar9 = FUN_00479700(s_classes_005cbff4,0);
      if (iVar9 != 0) {
        bVar2 = true;
      }
      iVar9 = FUN_00479700(s_exits_005cbffc,0);
      if (iVar9 != 0) {
        bVar3 = true;
      }
      iVar9 = FUN_00479700(s_walkmap_005cc004,0);
      if (iVar9 != 0) {
        bVar7 = true;
      }
      iVar9 = FUN_00479700(s_player_005cc00c,0);
      if (iVar9 != 0) {
        bVar8 = true;
      }
      FUN_00479580();
    } while (*(int *)(param_2 + 0x10) == 4);
    if (bVar1) goto LAB_0042648c;
    if (!bVar4) goto LAB_00426500;
    if (DAT_0065c558 == 0) {
      if (DAT_00656da8 == 0) {
        FUN_0041ee50(s_Saving_imagery__quickload_dat__f_005cc114);
        FUN_00448070();
        goto LAB_00426500;
      }
      pcVar11 = s__imagery__quickload_dat__not_sav_005cc14c;
    }
    else {
      pcVar11 = s_Cannot_save__quickload_dat__file_005cc0d8;
    }
LAB_004264f8:
    FUN_0041ee50(pcVar11);
  }
  else {
    bVar2 = true;
    bVar8 = true;
    bVar7 = true;
    bVar3 = true;
    bVar6 = true;
LAB_0042648c:
    if (DAT_0065c558 != 0) {
      pcVar11 = s_Cannot_save_headers_and__quicklo_005cc014;
      goto LAB_004264f8;
    }
    if (DAT_00656da8 != 0) {
      pcVar11 = s__imagery_headers_and__quickload__005cc08c;
      goto LAB_004264f8;
    }
    FUN_0041ee50(s_Saving_imagery_headers_and__quic_005cc058);
    FUN_004468e0();
  }
LAB_00426500:
  if (bVar2) {
    if (DAT_0065c558 == 0) {
      FUN_0041ee50(s_Saving_classes___005cc1bc);
      FUN_004755c0();
      iVar9 = FUN_004765e0(0);
      if (iVar9 != 0) {
        FUN_0049de80();
        goto LAB_0042654b;
      }
      pcVar11 = s_There_was_an_error_while_trying_t_005cc1d0;
    }
    else {
      pcVar11 = s_Cannot_save_classes_when_imagery_005cc18c;
    }
    FUN_0041ee50(pcVar11);
  }
LAB_0042654b:
  if (bVar3) {
    if (DAT_0065b488 == 0) {
      FUN_0041ee50(s_Saving_exits____005cc234);
      FUN_0050cca0();
    }
    else {
      FUN_0041ee50(s_Cannot_save_exits_to_a_packed_mo_005cc208);
    }
  }
  if (bVar5) {
    uVar10 = 0;
    if (*(int *)(param_2 + 0x10) == 8) {
      uVar10 = *(undefined4 *)(param_2 + 0x14);
      FUN_00479580();
    }
    FUN_0041ee50(s_Saving_game____005cc248);
    FUN_0047e7e0(uVar10);
  }
  if (bVar6) {
    if (DAT_0065b488 == 0) {
      FUN_0041ee50(s_Saving_map_sectors____005cc288);
      FUN_0047e900();
    }
    else {
      FUN_0041ee50(s_Cannot_save_map_sectors_to_a_pac_005cc258);
    }
  }
  if (bVar7) {
    if (DAT_0065c558 == 0) {
      if (param_1 != 0) {
        FUN_00452750(param_1,1,0);
        if (*(int **)(param_1 + 0x54) != (int *)0x0) {
          (**(code **)(**(int **)(param_1 + 0x54) + 8))();
        }
      }
      pcVar11 = s_saving_walkmap____005cc2d0;
    }
    else {
      pcVar11 = s_Cannot_save_walkmaps_when_imager_005cc2a0;
    }
    FUN_0041ee50(pcVar11);
  }
  if ((bVar8) && (FUN_0041ee50(s_Saving_Player____005cc2e4), DAT_00667fcc != 0)) {
    FUN_0051e0a0(0,0);
  }
  FUN_00447ab0();
  return 0;
}


