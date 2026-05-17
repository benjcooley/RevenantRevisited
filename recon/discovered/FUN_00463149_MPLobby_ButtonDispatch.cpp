// FUN_00463149_MPLobby_ButtonDispatch @ 00463149 size=1850

/* WARNING: Removing unreachable block (ram,0x004634f4) */
/* WARNING: Removing unreachable block (ram,0x00463254) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 * FUN_00463149_MPLobby_ButtonDispatch(void)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  int iVar5;
  uint uVar6;
  char cVar7;
  undefined4 *unaff_EBX;
  int *unaff_EBP;
  undefined4 *puVar8;
  char *pcVar9;
  undefined4 *in_stack_00000014;
  
  iVar2 = FUN_0059a530_stricmp(s_userinfo_005d2d30);
  if (iVar2 == 0) {
    _DAT_006596ac = unaff_EBX;
    FUN_00435150_DefScreen_Open(s_userinfo_005d4064,s_default_005d405c,0x11,0x7e,0x41,0x18a,0x13c,0x118,300,
                 s_widgets_005d4054);
    FUN_0048ed90(&DAT_00659450);
    uVar3 = FUN_0048ed60(&DAT_00659450);
    FUN_0048eea0(uVar3);
    return in_stack_00000014;
  }
  iVar2 = FUN_0059a530_stricmp(s_hostgame_005d2d3c);
  cVar7 = (char)unaff_EBX;
  if (iVar2 == 0) {
    if (DAT_00659c60 == unaff_EBX) {
      FUN_0053c060(s_MPMUSTSELCHAR_005d2d48);
      return (undefined4 *)0x1;
    }
    if ((int)unaff_EBX <= DAT_00658da0) {
      FUN_0046e6f0();
      puVar4 = DAT_00659c60;
      _strncpy((char *)(DAT_00659c60 + 0xde),&DAT_00658da4,0x1f);
      *(char *)((int)puVar4 + 0x397) = cVar7;
      FUN_00485320(&stack0x00000020);
      if (DAT_00659c60 != unaff_EBX) {
        FUN_005707d0();
        puVar4 = (undefined4 *)&stack0x00000020;
        puVar8 = DAT_00659c60 + 0x15c;
        for (iVar2 = 0x38; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar8 = *puVar4;
          puVar4 = puVar4 + 1;
          puVar8 = puVar8 + 1;
        }
      }
      iVar2 = FUN_00460ca0(DAT_00658da0);
      if (DAT_006596dc < (int)unaff_EBX) {
        puVar4 = &DAT_00658e58;
        for (iVar5 = 0x31; iVar5 != 0; iVar5 = iVar5 + -1) {
          *puVar4 = 0;
          puVar4 = puVar4 + 1;
        }
        puVar4 = (undefined4 *)(iVar2 + 0x8f4);
        puVar8 = &DAT_00658ef0;
        for (iVar5 = 0xb; iVar5 != 0; iVar5 = iVar5 + -1) {
          *puVar8 = *puVar4;
          puVar4 = puVar4 + 1;
          puVar8 = puVar8 + 1;
        }
        _strncpy(&DAT_00658e68,(char *)(iVar2 + 8),0x1f);
        DAT_00658e87 = cVar7;
        _strncpy(&DAT_00658e88,&DAT_0065994c,0x1f);
        DAT_00658ea7 = cVar7;
      }
      else {
        puVar4 = (undefined4 *)FUN_0048e5e0();
        if (puVar4 != unaff_EBX) {
          puVar8 = &DAT_00658e58;
          for (iVar5 = 0x31; iVar5 != 0; iVar5 = iVar5 + -1) {
            *puVar8 = *puVar4;
            puVar4 = puVar4 + 1;
            puVar8 = puVar8 + 1;
          }
        }
      }
      _strncpy(&DAT_00658ea8,(char *)(iVar2 + 0x58),0x1f);
      _DAT_00659bf0 = &DAT_00658dc8;
      DAT_00658ec7 = cVar7;
      FUN_0048ea40();
      return in_stack_00000014;
    }
    FUN_0053c060(s_MPMUSTSELMOD_005d2d58);
    return (undefined4 *)0x1;
  }
  iVar5 = FUN_0059a530_stricmp(s_joingame_005d2d68);
  iVar2 = DAT_0065996c;
  if (iVar5 == 0) {
    if (DAT_00659c60 == unaff_EBX) {
      FUN_0053c060(s_MPMUSTSELCHAR_005d2d74);
      return (undefined4 *)0x1;
    }
    if (DAT_00658da0 < (int)unaff_EBX) {
      FUN_0053c060(s_MPMUSTSELMOD_005d2d84);
      return (undefined4 *)0x1;
    }
    if (DAT_0065996c < (int)unaff_EBX) {
      FUN_0053c060(s_MPMUSTSELGAME_005d2d94);
      return (undefined4 *)0x1;
    }
    puVar4 = &DAT_00658e58;
    for (iVar5 = 0x31; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    FUN_005769c0(iVar2);
    puVar4 = (undefined4 *)FUN_00460d00(&DAT_00658ea8);
    if (puVar4 != unaff_EBX) {
      if (CONCAT11(DAT_00658eea,DAT_00658eeb) != CONCAT11(DAT_005d79dc,DAT_005d79dd)) {
        FUN_00461900();
        return (undefined4 *)0x1;
      }
      if (CONCAT11(DAT_00658ee8,DAT_00658ee9) ==
          CONCAT11(*(undefined1 *)(puVar4 + 0x1e),*(undefined1 *)((int)puVar4 + 0x79))) {
        FUN_0046e6f0();
        puVar4 = DAT_00659c60;
        _strncpy((char *)(DAT_00659c60 + 0xde),&DAT_0065994c,0x1f);
        *(char *)((int)puVar4 + 0x397) = cVar7;
        FUN_00485320(&stack0x00000020);
        if (DAT_00659c60 != unaff_EBX) {
          puVar4 = (undefined4 *)&stack0x00000020;
          puVar8 = DAT_00659c60 + 0x15c;
          for (iVar2 = 0x38; iVar2 != 0; iVar2 = iVar2 + -1) {
            *puVar8 = *puVar4;
            puVar4 = puVar4 + 1;
            puVar8 = puVar8 + 1;
          }
        }
        _DAT_00659bf0 = &DAT_00659970;
        FUN_0048ea40();
        return in_stack_00000014;
      }
      FUN_00461900();
      return (undefined4 *)0x1;
    }
    FUN_0053c060(s_MPMODULEUNAVAILABLE_005d2da4);
    return (undefined4 *)0x1;
  }
  iVar2 = FUN_0059a530_stricmp(&DAT_005d2db8);
  if (iVar2 == 0) {
    _DAT_00659bf0 = &DAT_0065d358;
    DAT_00659c60 = unaff_EBX;
    FUN_0048ea40();
    return in_stack_00000014;
  }
  iVar2 = FUN_0059a530_stricmp(s_deathmatch_005d2dc0);
  if (iVar2 == 0) {
    DAT_005d7a44 = 1;
    FUN_00482160();
    FUN_00482a20(s_SimpleMode_005d2dd8,DAT_005d7a44);
    DAT_00659c60 = unaff_EBX;
    FUN_0048ee10();
    (**(code **)(*unaff_EBP + 4))();
    FUN_0046b010_LoadDef_connectsimple();
    FUN_0048ed90(&DAT_00659278);
    return in_stack_00000014;
  }
  iVar2 = FUN_0059a530_stricmp(s_newchar_005d2de4);
  if (iVar2 == 0) {
    _DAT_00659bf0 = &DAT_00658f20;
    FUN_0048ea40();
    _DAT_00659140 = unaff_EBX;
    _DAT_006591a8 = unaff_EBX;
    _DAT_00659144 = (undefined4 *)0x2;
    return in_stack_00000014;
  }
  iVar2 = FUN_0059a530_stricmp(s_editchar_005d2dec);
  if (iVar2 == 0) {
    iVar2 = *(int *)(unaff_EBP[0x62] + 0xa0);
    if (iVar2 < (int)unaff_EBX) {
      FUN_0053c060(s_MPMUSTSELCHAR_005d2df8);
      return (undefined4 *)0x0;
    }
    if ((iVar2 < DAT_00659bb8) &&
       (puVar4 = (undefined4 *)**(undefined4 **)(DAT_00659bc8 + iVar2 * 4), puVar4 != unaff_EBX)) {
      iVar2 = *(int *)(DAT_00659bc8 + iVar2 * 4);
      _DAT_00659bf0 = &DAT_00658f20;
      FUN_0048ea40();
      _DAT_00659140 = unaff_EBX;
      _DAT_00659144 = unaff_EBX;
      _DAT_006591a8 = puVar4;
      _strncpy(&DAT_006591ac,(char *)(iVar2 + 4),0x7f);
      DAT_0065922b = cVar7;
      return in_stack_00000014;
    }
    return (undefined4 *)0x0;
  }
  iVar2 = FUN_0059a530_stricmp(s_delchar_005d2e08);
  if (iVar2 != 0) {
    return unaff_EBX;
  }
  iVar2 = *(int *)(unaff_EBP[0x62] + 0xa0);
  if (iVar2 < (int)unaff_EBX) {
    FUN_0053c060(s_MPMUSTSELCHAR_005d2e10);
    return (undefined4 *)0x0;
  }
  if ((DAT_00659bb8 <= iVar2) ||
     ((undefined4 *)**(undefined4 **)(DAT_00659bc8 + iVar2 * 4) == unaff_EBX)) {
    return (undefined4 *)0x0;
  }
  iVar2 = *(int *)(DAT_00659bc8 + iVar2 * 4);
  iVar5 = FUN_0053c060(s_MPDELCHAR_005d2e20);
  if (iVar5 == 0) {
    return (undefined4 *)0x0;
  }
  FUN_00483120(&DAT_0065da00,&stack0x00000020);
  uVar6 = 0xffffffff;
  pcVar9 = &stack0x00000020;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (cVar1 != '\0');
  iVar5 = -(~uVar6 - 1);
  _strncpy(&stack0x0000001f + ~uVar6,s_Chars__005d2e2c,iVar5 + 0x103);
  (&stack0x0000001f + ~uVar6)[iVar5 + 0x103] = cVar7;
  uVar6 = 0xffffffff;
  pcVar9 = &stack0x00000020;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (cVar1 != '\0');
  iVar5 = -(~uVar6 - 1);
  _strncpy(&stack0x0000001f + ~uVar6,(char *)(iVar2 + 4),iVar5 + 0x103);
  (&stack0x0000001f + ~uVar6)[iVar5 + 0x103] = cVar7;
  FUN_0058c01b(&stack0x00000020);
  FUN_00461bc0();
  FUN_00430c50();
  FUN_00430b80();
  return in_stack_00000014;
}


