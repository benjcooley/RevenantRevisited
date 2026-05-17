// FUN_0047a660_TPlayScreen_Initialize @ 0047a660 size=3068

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0047a660_TPlayScreen_Initialize(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  uint *puVar6;
  byte *pbVar7;
  undefined4 extraout_ECX;
  bool bVar8;
  char *pcVar9;
  undefined4 uVar10;
  char acStack_105 [259];
  char acStack_2 [2];
  
  FUN_004820b0(s_Initializing_TPlayScreen_005d701c);
  DAT_0066829c = (uint)(*(int *)(param_1 + 0x6d8) == 3);
  if (DAT_00668158 == 0) {
    FUN_00483120(&DAT_0065d6a4,acStack_105 + 1,0x104);
    if ((DAT_0065a784 < 0) || (iVar3 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4), iVar3 == 0)) {
      FUN_004485a0(s_loadbar_dat_005d7054,0);
    }
    else {
      pcVar9 = acStack_105;
      uVar5 = 0xffffffff;
      do {
        pcVar9 = pcVar9 + 1;
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
      } while (*pcVar9 != '\0');
      iVar1 = -(~uVar5 - 1);
      _strncpy(acStack_105 + ~uVar5,(char *)(iVar3 + 0x58),iVar1 + 0x103);
      (acStack_105 + ~uVar5)[iVar1 + 0x103] = '\0';
      pcVar9 = acStack_105;
      uVar5 = 0xffffffff;
      do {
        pcVar9 = pcVar9 + 1;
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
      } while (*pcVar9 != '\0');
      iVar3 = -(~uVar5 - 1);
      _strncpy(acStack_105 + ~uVar5,s__loadscreen_bmp_005d7038,iVar3 + 0x103);
      (acStack_105 + ~uVar5)[iVar3 + 0x103] = '\0';
      FUN_004485a0(s_loadbar_dat_005d7048,acStack_105 + 1);
    }
    FUN_00448680(0,1);
  }
  *(undefined4 *)(param_1 + 0x54) = 0;
  FUN_00448680(10,0);
  *(undefined4 *)(param_1 + 0x5e4) = 0;
  *(undefined4 *)(param_1 + 0x5e8) = 0;
  *(undefined4 *)(param_1 + 0x5ec) = 0;
  *(undefined4 *)(param_1 + 0x69c) = 0;
  *(undefined4 *)(param_1 + 0x7c) = 0;
  *(undefined4 *)(param_1 + 0x670) = 0;
  *(undefined4 *)(param_1 + 0x67c) = 0;
  *(undefined4 *)(param_1 + 0x6d4) = 0;
  *(undefined4 *)(param_1 + 0x674) = 0;
  *(undefined4 *)(param_1 + 0x678) = 0;
  *(undefined4 *)(param_1 + 0x680) = 0;
  *(undefined4 *)(param_1 + 0x684) = 0;
  *(undefined4 *)(param_1 + 0x688) = 0;
  if (*(int *)(param_1 + 0x6d8) == 2) {
    uVar2 = FUN_00460d60(&DAT_0065bb18,0);
    *(undefined4 *)(param_1 + 0x6dc) = uVar2;
  }
  if (*(int *)(param_1 + 0x6dc) == -1) {
    *(undefined4 *)(param_1 + 0x6dc) = DAT_0065a780;
  }
  FUN_004609f0(*(undefined4 *)(param_1 + 0x6dc));
  FUN_00448680(10,0);
  iVar1 = DAT_0065a784;
  iVar3 = DAT_0065a77c;
  if (DAT_0065a784 < 0) {
    puVar6 = (uint *)0x0;
  }
  else {
    puVar6 = *(uint **)(DAT_0065a77c + DAT_0065a784 * 4);
  }
  bVar8 = DAT_0065a784 < 0;
  *(uint *)(param_1 + 0x5d8) = *puVar6 >> 6 & 1;
  if (bVar8) {
    puVar6 = (uint *)0x0;
  }
  else {
    puVar6 = *(uint **)(iVar3 + iVar1 * 4);
  }
  *(uint *)(param_1 + 0x5e0) = ~*puVar6 >> 7 & 1;
  *(undefined4 *)(param_1 + 0x5dc) = 0;
  *(undefined4 *)(param_1 + 0x6a8) = 0;
  *(undefined4 *)(param_1 + 0x6a4) = 0;
  *(undefined4 *)(param_1 + 0x6a0) = 0;
  *(undefined4 *)(param_1 + 0x6b4) = 0;
  *(undefined4 *)(param_1 + 0x6b0) = 0;
  *(undefined4 *)(param_1 + 0x6ac) = 0;
  *(undefined4 *)(param_1 + 0x6b8) = 0;
  *(undefined4 *)(param_1 + 0x6c0) = 2;
  DAT_0065dde4 = FUN_0047f670(s_EquipPane_dat_005d7060,0xffffffff,0);
  FUN_00448680(0xf,0);
  DAT_0065a574 = FUN_0047f670(s_SpellPane_dat_005d7070,0xffffffff,0);
  FUN_00448680(5,0);
  DAT_0065bc3c = FUN_0047f670(s_SpellIcons_dat_005d7080,0xffffffff,0);
  FUN_0043a000();
  FUN_00448680(5,0);
  if (DAT_006680c8 == 0) {
    pcVar9 = s_StatusBar_dat_005d70a4;
  }
  else {
    pcVar9 = s_StatusBarNoTex_dat_005d7090;
  }
  DAT_0065a9d0 = FUN_0047f670(pcVar9,0xffffffff,0);
  FUN_00448680(5,0);
  if (DAT_006680c8 == 0) {
    pcVar9 = s_SideBarTabs_dat_005d70cc;
  }
  else {
    pcVar9 = s_SideBarTabsNoTex_dat_005d70b4;
  }
  DAT_0065c5c8 = FUN_0047f670(pcVar9,0xffffffff,0);
  FUN_00448680(5,0);
  DAT_0065c130 = FUN_0047f670(s_Inventory_dat_005d70dc,0xffffffff,0);
  FUN_00448680(5,0);
  DAT_0065a570 = FUN_0047f670(s_BottomBar_dat_005d70ec,0xffffffff,0);
  FUN_00448680(5,0);
  DAT_00666640 = FUN_0047f670(s_SpellScroll_dat_005d70fc,0xffffffff,0);
  FUN_00448680(5,0);
  DAT_00666444 = FUN_0047f670(s_Dialog_dat_005d710c,0xffffffff,0);
  FUN_00448680(5,0);
  DAT_0065a9d4 = FUN_0047f670(s_Portraits_dat_005d7118,0xffffffff,0);
  FUN_00448680(5,0);
  DAT_0065c6f4 = FUN_0047f670(s_StatsPane_dat_005d7128,0xffffffff,0);
  FUN_00448680(5,0);
  if (DAT_006680c8 == 0) {
    DAT_00667fc4 = FUN_0047f670(s_mpingametex_dat_005d7160,0xffffffff,0);
    pcVar9 = s_createchartex_dat_005d7170;
  }
  else {
    DAT_00667fc4 = FUN_0047f670(s_mpingamenotex_dat_005d7138,0xffffffff,0);
    pcVar9 = s_createcharnotex_dat_005d714c;
  }
  DAT_0065c890 = FUN_0047f670(pcVar9,0xffffffff,0);
  FUN_004820b0(s_Creating_background_areas_005d7184);
  *(undefined4 *)(param_1 + 0x58) = 1;
  FUN_00448680(0x28,0);
  FUN_004820b0(s_Initializing_game_script_manager_005d71a0);
  FUN_00496240_TScriptManager_Initialize();
  FUN_00448680(10,0);
  FUN_004820b0(s_Initializing_game_areas_005d71c4);
  FUN_0041bf00_TAreaMgr_Initialize();
  FUN_00448680(0x1e,0);
  _DAT_006668ec = 0;
  *(undefined4 *)(param_1 + 0x5b8) = DAT_00668168;
  _DAT_006668f0 = 0;
  DAT_006668f4 = *(undefined4 *)(PTR_DAT_005d79e0 + 4);
  DAT_006668f8 = *(undefined4 *)(PTR_DAT_005d79e0 + 8);
  iVar3 = FUN_0044d5c0();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_map_pane_005d71e0,0);
  }
  iVar3 = FUN_0053c8c0_TSidePane_Initialize();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_side_pane_005d7200,0);
  }
  iVar3 = FUN_0052d8a0_TBottomPane_Initialize();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_bottom_pane_005d7220,0);
  }
  iVar3 = FUN_0053cc30_TSideTabsPane_Initialize();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_SideTabs_pa_005d7244,0);
  }
  iVar3 = FUN_00529970();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_automap_pan_005d7268,0);
  }
  iVar3 = FUN_00537650();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_inventory_p_005d728c,0);
  }
  iVar3 = FUN_00544160_TQuickSpellPane_Initialize();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_quick_spell_005d72b0,0);
  }
  FUN_00448680(5,0);
  iVar3 = FUN_0052c970_TBarInvPane_Initialize();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_BarInv_pane_005d72dc,0);
  }
  FUN_00448680(5,0);
  iVar3 = FUN_0054bf70_TTextBar_Initialize();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_text_bar_005d7300,0);
  }
  FUN_00448680(5,0);
  _DAT_00667cdc = 0;
  _DAT_00667ce0 = 0;
  _DAT_00667ce4 = *(undefined4 *)(PTR_DAT_005d79e0 + 4);
  _DAT_00667ce8 = *(undefined4 *)(PTR_DAT_005d79e0 + 8);
  iVar3 = FUN_00534fd0();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_dialog_pane_005d7320,0);
  }
  FUN_00448680(5,0);
  iVar3 = FUN_0052c780_TBottomBarPane_Initialize();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_Bottom_bar_005d7344,0);
  }
  FUN_00448680(5,0);
  iVar3 = FUN_00536360();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_equipment_p_005d7364,0);
  }
  FUN_00448680(5,0);
  iVar3 = FUN_005432a0();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_spell_pane_005d7388,0);
  }
  FUN_00448680(5,0);
  iVar3 = FUN_00546b50();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_stat_pane_005d73a8,0);
  }
  FUN_00448680(5,0);
  iVar3 = FUN_005449e0();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_stat_pane_005d73c8,0);
  }
  FUN_00448680(5,0);
  iVar3 = FUN_00549740_TPlyrStatusBar_Initialize();
  if (iVar3 == 0) {
    FUN_00481c10(s_Trouble_initializing_PlyrStatusB_005d73e8,0);
  }
  FUN_00448680(5,0);
  FUN_0048ed90(&DAT_006668d8,0xffffffff);
  FUN_0048ed90(&DAT_00667cc8,0xffffffff);
  FUN_0048ed90(&DAT_00666140,0xffffffff);
  FUN_0048ed90(&DAT_00667c58,0xffffffff);
  FUN_0048ed90(&DAT_0065be50,0xffffffff);
  FUN_0048ed90(&DAT_0065c5d0,0xffffffff);
  FUN_0048ed90(&DAT_0065a8c0,0xffffffff);
  FUN_0053ea00();
  FUN_0053ead0();
  FUN_00448680(10,0);
  DAT_0065a28c = FUN_0046d710(s_cursor_005d7410);
  if (DAT_0065a28c == 0) {
    return 0;
  }
  _DAT_0065a284 = FUN_0046d710(s_handcursor_005d7418);
  if (_DAT_0065a284 == 0) {
    return 0;
  }
  FUN_0043a020(DAT_0065a28c);
  *(undefined4 *)(param_1 + 0x6d0) = 1;
  FUN_0053cab0(0);
  FUN_0053cb40(0);
  if ((((*(int *)(param_1 + 0x6a0) != 1) && (*(int *)(param_1 + 0x6a4) == 0)) &&
      (*(int *)(param_1 + 0x6a8) == 0)) && (*(int *)(param_1 + 0x6a0) == 0)) {
    *(undefined4 *)(param_1 + 0x6a4) = 1;
  }
  FUN_0047ebc0(1);
  iVar3 = *(int *)(param_1 + 0x6d8);
  DAT_00667eb8 = 0;
  if (iVar3 == 0) {
    *(undefined1 *)(param_1 + 0x6e4) = 0;
    *(undefined4 *)(param_1 + 0x6e0) = 0xffffffff;
    FUN_0044e460();
    FUN_0048e610_LoadNewGame();
    goto LAB_0047b060;
  }
  if (iVar3 == 1) {
    if (*(char *)(param_1 + 0x6e4) != '\0') {
      uVar2 = FUN_0048d6d0(param_1 + 0x6e4);
      *(undefined4 *)(param_1 + 0x6e0) = uVar2;
    }
    FUN_0044e460();
    iVar3 = FUN_0048e5b0(*(undefined4 *)(param_1 + 0x6e0),0);
    if (iVar3 == 0) {
      uVar2 = FUN_0049d800(s_GAMENOTFOUND_005d7424);
      FUN_0054d170(&DAT_0065c5d0,uVar2);
      FUN_0048e610_LoadNewGame();
    }
    goto LAB_0047b060;
  }
  if (iVar3 != 2) {
    if (iVar3 == 3) {
      DAT_00668108 = 0;
      DAT_0066812c = 0;
      DAT_00668104 = 0;
      DAT_00668110 = 0;
      DAT_0066810c = 0;
      DAT_00668130 = 0;
      FUN_0057a620(0xffffffff,0);
    }
    goto LAB_0047b060;
  }
  FUN_004820b0(s_Initializing_editor_005d7434);
  *(undefined1 *)(param_1 + 0x6e4) = 0;
  *(undefined4 *)(param_1 + 0x6e0) = 0xffffffff;
  FUN_0044e460();
  DAT_0065a294 = 1;
  if (DAT_0065a784 < 0) {
    pbVar7 = (byte *)0x0;
  }
  else {
    pbVar7 = *(byte **)(DAT_0065a77c + DAT_0065a784 * 4);
  }
  if ((*pbVar7 & 4) == 0) {
LAB_0047afd2:
    iVar3 = FUN_0048e610_LoadNewGame();
    if ((iVar3 != 0) && (DAT_00667fcc != (int *)0x0)) {
      FUN_00450d20(DAT_00667fcc + 4);
      DAT_00666970 = (uint)*(ushort *)((int)DAT_00667fcc + 0xe);
      if (DAT_00666970 != DAT_00666974) {
LAB_0047b012:
        FUN_004546a0();
      }
LAB_0047b01c:
      DAT_0065a294 = 0;
    }
  }
  else {
    if (DAT_0065a784 < 0) {
      iVar3 = 0;
    }
    else {
      iVar3 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    if (*(int *)(iVar3 + 0x88) < 1) goto LAB_0047afd2;
    if (DAT_0065a784 < 0) {
      iVar3 = 0;
    }
    else {
      iVar3 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    iVar3 = **(int **)(iVar3 + 0x98);
    if (iVar3 != 0) {
      FUN_00450d20(iVar3 + 0x28);
      DAT_00666970 = *(uint *)(iVar3 + 0x20);
      if (DAT_00666970 != DAT_00666974) goto LAB_0047b012;
      goto LAB_0047b01c;
    }
  }
  *(undefined4 *)(param_1 + 0x6bc) = 1;
LAB_0047b060:
  FUN_004820b0(s_Loading_interface_starting_game__005d744c);
  *(undefined4 *)(param_1 + 0x5b4) = 0;
  uVar2 = FUN_00446aa0(s_Misc_Blood_I3D_005d7470,1);
  iVar3 = FUN_00446b10(uVar2);
  *(int *)(param_1 + 0x70) = iVar3;
  if (iVar3 != 0) {
    FUN_00447ac0(1);
  }
  FUN_00448680(5,0);
  uVar2 = FUN_00446aa0(s_Misc_Sparks_I3D_005d7480,1);
  iVar3 = FUN_00446b10(uVar2);
  *(int *)(param_1 + 0x74) = iVar3;
  if (iVar3 != 0) {
    FUN_00447ac0(1);
  }
  FUN_00448680(5,0);
  uVar2 = FUN_00446aa0(s_Misc_Impact_I3D_005d7490,1);
  iVar3 = FUN_00446b10(uVar2);
  *(int *)(param_1 + 0x78) = iVar3;
  if (iVar3 != 0) {
    FUN_00447ac0(1);
  }
  FUN_00448680(5,0);
  if (DAT_00668158 == 0) {
    iVar3 = *(int *)(param_1 + 0x5bc);
    uVar10 = 0;
    uVar4 = FUN_00444e20(0);
    uVar2 = extraout_ECX;
    FUN_00419dd0(uVar4);
    (**(code **)(iVar3 + 0x30))(8,uVar2,uVar10);
    *(int *)(param_1 + 0x40) = param_1 + 0x5bc;
    *(int *)(param_1 + 0x44) = param_1 + 0x5bc;
  }
  if ((DAT_00667fcc != (int *)0x0) && (DAT_005d7a30 != 0)) {
    _DAT_0065a26c = (float)_DAT_0065d3f4 * _DAT_005a3570;
    _DAT_0065a280 = _DAT_005a34e4 - _DAT_0065a26c;
    FUN_004997d0(DAT_00667fcc + 4,*(undefined2 *)((int)DAT_00667fcc + 0xe),&LAB_0047b260);
  }
  FUN_00448800(0x3f800000,0);
  FUN_00448650();
  FUN_0047c580(1);
  if (((((DAT_0066829c != 0) && (DAT_00667fcc != (int *)0x0)) &&
       (((DAT_00667fcc[0xdb] & 0x100000U) == 0 ||
        (iVar3 = (**(code **)(*DAT_00667fcc + 0x1c0))(), iVar3 < 1)))) &&
      ((DAT_0066829c != 0 && (DAT_00659800 == 0)))) &&
     ((iVar3 = (**(code **)(*DAT_00667fcc + 0x1c0))(), iVar3 < 1 ||
      ((DAT_00667fcc[0xdb] & 0x100000U) != 0)))) {
    FUN_0047c580(0);
    FUN_00469370();
    FUN_0048ed90(&DAT_006597c0,0xffffffff);
    uVar4 = 7;
    uVar2 = FUN_0048ed60(&DAT_006597c0);
    FUN_0048eea0(uVar2,uVar4);
  }
  return 1;
}


