// FUN_0048df70_LoadGame @ 0048df70 size=1585

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

bool __thiscall FUN_0048df70_LoadGame(int *param_1,char *param_2,uint param_3)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  char *pcVar5;
  int iVar6;
  bool bVar7;
  undefined4 *puVar8;
  int *piStack_1a8;
  int *piStack_1a4;
  uint uStack_1a0;
  int iStack_19c;
  int *local_198;
  int iStack_194;
  int iStack_190;
  undefined4 uStack_18c;
  uint local_188;
  undefined4 local_184 [5];
  int iStack_170;
  int iStack_16c;
  int iStack_168;
  char acStack_164 [31];
  undefined1 uStack_145;
  char acStack_105 [259];
  char acStack_2 [2];
  
  local_198 = (int *)(param_3 & 1);
  if (local_198 == (int *)0x0) {
    if (param_2 == (char *)0x0) {
      param_2 = s_Default_Save_005d9d40;
    }
    iVar6 = 0;
    if (0 < *param_1) {
      do {
        iVar2 = FUN_0059a530_stricmp(param_2,**(undefined4 **)(param_1[4] + iVar6 * 4));
        if (iVar2 == 0) break;
        iVar6 = iVar6 + 1;
      } while (iVar6 < *param_1);
    }
    if (*param_1 <= iVar6) {
      return false;
    }
    _strncpy(acStack_105 + 1,*(char **)(*(int *)(param_1[4] + iVar6 * 4) + 4),0x103);
    acStack_2[1] = 0;
    _strncpy(&DAT_00667eb8,acStack_105 + 1,0x103);
    uVar4 = 0xffffffff;
    DAT_00667fbb = 0;
    pcVar5 = &DAT_00667eb8;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar1 = *pcVar5;
      pcVar5 = pcVar5 + 1;
    } while (cVar1 != '\0');
    for (pcVar5 = &DAT_00667eb7 + ~uVar4; (pcVar5 != &DAT_00667eb8 && (*pcVar5 != '\\'));
        pcVar5 = pcVar5 + -1) {
      *pcVar5 = '\0';
    }
    uVar4 = 0xffffffff;
    pcVar5 = &DAT_00667eb8;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar1 = *pcVar5;
      pcVar5 = pcVar5 + 1;
    } while (cVar1 != '\0');
    uVar4 = ~uVar4;
    _strncpy(&DAT_00667eb7 + uVar4,s_CurMap_005d9d50,-(uVar4 - 1) + 0x103);
    *(undefined1 *)(uVar4 + -(uVar4 - 1) + 0x667fba) = 0;
  }
  else {
    FUN_00483120(&DAT_0065d6a4,acStack_105 + 1,0x104);
    if (DAT_0065a780 < 0) {
      iVar6 = 0;
    }
    else {
      iVar6 = *(int *)(DAT_0065a77c + DAT_0065a780 * 4);
    }
    pcVar5 = acStack_105;
    uVar4 = 0xffffffff;
    do {
      pcVar5 = pcVar5 + 1;
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
    } while (*pcVar5 != '\0');
    iVar2 = -(~uVar4 - 1);
    _strncpy(acStack_105 + ~uVar4,(char *)(iVar6 + 0x58),iVar2 + 0x103);
    (acStack_105 + ~uVar4)[iVar2 + 0x103] = '\0';
    pcVar5 = acStack_105;
    uVar4 = 0xffffffff;
    do {
      pcVar5 = pcVar5 + 1;
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
    } while (*pcVar5 != '\0');
    iVar6 = -(~uVar4 - 1);
    _strncpy(acStack_105 + ~uVar4,s__newgame_sav_005d9d30,iVar6 + 0x103);
    (acStack_105 + ~uVar4)[iVar6 + 0x103] = '\0';
    DAT_00667eb8 = 0;
  }
  param_3 = param_3 & 2;
  param_1[5] = 1;
  local_188 = param_3;
  if (param_3 == 0) {
    FUN_0044e460();
    if (local_198 == (int *)0x0) {
      FUN_0044e050(&DAT_00667eb8);
    }
    FUN_0047ece0();
    _DAT_00667ca8 = 1;
    FUN_005360f0();
    FUN_00532f40();
    FUN_00496e20();
    FUN_004975c0_TScriptManager_ReloadStates();
    FUN_0041c600();
    FUN_0051eda0_TPlayerManager_Clear();
    FUN_0047c580(1);
  }
  iVar6 = FUN_004a13f0(acStack_105 + 1,&DAT_005d9d58,0);
  if (iVar6 == 0) {
    param_1[5] = 0;
    return false;
  }
  iVar2 = FUN_004a17b0(iVar6);
  if (iVar2 == 0) {
    FUN_00481c10(s_Invalid_save_file__s_in_save_dir_005d9d5c,acStack_105 + 1);
  }
  iVar3 = FUN_004a15a0(local_184,0x80,1,iVar6);
  bVar7 = 0 < iVar3;
  uStack_18c = local_184[0];
  uVar4 = iVar2 - 0x80;
  iStack_194 = iStack_170;
  iStack_190 = iStack_16c;
  iStack_19c = iStack_168;
  if ((iStack_170 == 0) || (iStack_16c < 1)) {
    puVar8 = &DAT_00668300;
    for (iVar2 = 0x80; param_3 = local_188, iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar8 = 0;
      puVar8 = puVar8 + 1;
    }
  }
  else {
    iVar3 = FUN_004a15a0(&DAT_00668300,0x200,1,iVar6);
    bVar7 = 0 < iVar3 && bVar7;
    uVar4 = iVar2 - 0x280;
  }
  if (param_3 == 0) {
    local_198 = (int *)FUN_00482ef0(uVar4);
    iVar2 = FUN_004a15a0(local_198,uVar4,1,iVar6);
    if (iVar2 < 1) {
      bVar7 = false;
    }
    FUN_004a1540(iVar6);
    DAT_0065a254 = 0;
    piStack_1a4 = local_198;
    piStack_1a8 = local_198;
    uStack_1a0 = uVar4;
    if ((bVar7 != false) && (9 < iStack_19c)) {
      FUN_004974c0(&piStack_1a8);
    }
    param_1[8] = 0x20;
    if ((bVar7 == false) || (iStack_19c < 0xb)) {
      if (param_1[7] != 0) {
        FUN_004830f0(param_1[7]);
      }
      param_1[7] = 0;
      param_1[6] = 0;
    }
    else {
      iVar6 = *piStack_1a4;
      piStack_1a4 = piStack_1a4 + 1;
      param_1[6] = iVar6;
      if (0x20 < iVar6) {
        do {
          iVar2 = param_1[8];
          param_1[8] = iVar2 + 0x20;
        } while (iVar2 + 0x20 < iVar6);
      }
      if (param_1[7] != 0) {
        FUN_004830f0(param_1[7]);
      }
      param_1[7] = 0;
      if (0 < param_1[6]) {
        iVar2 = FUN_00482fb0(param_1[8] << 3);
        iVar6 = param_1[6];
        iVar3 = 0;
        param_1[7] = iVar2;
        if (0 < iVar6) {
          do {
            if (1 < iStack_190) {
              *(int *)(iVar2 + iVar3 * 8) = *piStack_1a4;
              piStack_1a4 = piStack_1a4 + 1;
              *(int *)(iVar2 + 4 + iVar3 * 8) = *piStack_1a4;
            }
            piStack_1a4 = piStack_1a4 + 1;
            iVar3 = iVar3 + 1;
          } while (iVar3 < iVar6);
        }
      }
    }
    iVar6 = iStack_190;
    if (iStack_190 < 2) {
      if (param_1[7] != 0) {
        FUN_004830f0(param_1[7]);
      }
      param_1[7] = 0;
      param_1[6] = 0;
    }
    if (iVar6 < 1) {
      if (((bVar7 != false) && (0xc < iStack_19c)) &&
         (uVar4 = (int)piStack_1a4 + (0x50 - (int)piStack_1a8), uVar4 <= uStack_1a0)) {
        piStack_1a4 = (int *)(uVar4 + (int)piStack_1a8);
      }
      if ((-1 < DAT_0065a780) && (iVar2 = *(int *)(DAT_0065a77c + DAT_0065a780 * 4), iVar2 != 0)) {
        _strncpy(acStack_164,(char *)(iVar2 + 0x58),0x1f);
        uStack_145 = 0;
      }
    }
    if (bVar7 != false) {
      if (iVar6 < 1) {
        iVar6 = FUN_00471ce0_CreateObjectFromStream(&piStack_1a8,iStack_19c,1);
        if (iVar6 != 0) {
          FUN_0051f0a0_TPlayerManager_AddPlayer(iVar6,0xffffffff);
          FUN_0051d680_SetPlayerState(*(uint *)(iVar6 + 0x36c) & 0xfffffffd | 1);
          if (iStack_194 == 0) {
            FUN_0051f060_TPlayerManager_SetMainPlayer(iVar6);
          }
        }
      }
      else {
        iVar6 = *piStack_1a4;
        piStack_1a4 = piStack_1a4 + 1;
        if (0 < iVar6) {
          do {
            iVar2 = FUN_00471ce0_CreateObjectFromStream(&piStack_1a8,iStack_19c,1);
            if (iVar2 != 0) {
              FUN_0051f0a0_TPlayerManager_AddPlayer(iVar2,0xffffffff);
              FUN_0051d680_SetPlayerState(*(uint *)(iVar2 + 0x36c) & 0xfffffffd | 1);
              if (iStack_194 == 0) {
                FUN_0051f060_TPlayerManager_SetMainPlayer(iVar2);
              }
            }
            iVar6 = iVar6 + -1;
          } while (iVar6 != 0);
        }
      }
    }
    if (local_198 != (int *)0x0) {
      FUN_00482f80(local_198);
    }
    iVar6 = FUN_0051ee70_TPlayerManager_NumPlayers(0);
    if (iVar6 < 1) {
      bVar7 = false;
    }
    iVar6 = FUN_00460d60(acStack_164,0);
    if (iVar6 < 0) {
      bVar7 = false;
    }
    else {
      FUN_004609f0(iVar6);
    }
    FUN_0047e950(uStack_18c);
    _DAT_0065cb40 = 1;
    FUN_0047c580(1);
    param_1[5] = 0;
    DAT_0065a254 = 0;
    return bVar7;
  }
  FUN_004a1540(iVar6);
  param_1[5] = 0;
  return true;
}


