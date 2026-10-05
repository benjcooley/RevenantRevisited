// FUN_0048d720 @ 0048d720 size=2066

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall FUN_0048d720(int *param_1,char *param_2,undefined4 param_3)

{
  char cVar1;
  int iVar2;
  int iVar3;
  byte *pbVar4;
  int iVar5;
  char *pcVar6;
  HANDLE hFindFile;
  undefined4 uVar7;
  int *piVar8;
  uint uVar9;
  uint uVar10;
  undefined4 *puVar11;
  char *pcVar12;
  undefined4 *puVar13;
  char *pcVar14;
  undefined1 local_81c [4];
  int local_818;
  int *local_814;
  int local_810;
  int *local_808;
  undefined4 local_804;
  undefined4 uStack_800;
  int *piStack_7fc;
  int *piStack_7f8;
  undefined4 uStack_7f4;
  undefined4 uStack_7f0;
  undefined4 uStack_7ec;
  undefined4 uStack_7e8;
  char local_7e4 [258];
  char acStack_6e2 [2];
  undefined4 local_6e0 [5];
  int local_6cc;
  undefined4 local_6c8;
  undefined4 local_6c4;
  char local_6c0 [31];
  undefined1 local_6a1;
  char acStack_661 [259];
  char acStack_55e [2];
  undefined4 local_55c [65];
  _WIN32_FIND_DATAA local_458;
  char local_318 [260];
  char local_214 [260];
  CHAR local_110 [260];
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d8e3;
  local_c = ExceptionList;
  if (DAT_00667fcc == 0) {
    return 0;
  }
  ExceptionList = &local_c;
  local_808 = param_1;
  if ((DAT_00668154 != 0) &&
     (ExceptionList = &local_c, iVar3 = FUN_0059a530(param_2,s_newgame_005d9ca0), iVar3 == 0)) {
    pbVar4 = (byte *)0x0;
    if (-1 < DAT_0065a784) {
      pbVar4 = *(byte **)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    if ((*pbVar4 & 0x20) == 0) {
      uVar9 = 0xffffffff;
      pcVar6 = &DAT_0065d6a4;
      do {
        pcVar14 = pcVar6;
        if (uVar9 == 0) break;
        uVar9 = uVar9 - 1;
        pcVar14 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar14;
      } while (cVar1 != '\0');
      uVar9 = ~uVar9;
      pcVar6 = pcVar14 + -uVar9;
      pcVar14 = local_7e4;
      for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
        *(undefined4 *)pcVar14 = *(undefined4 *)pcVar6;
        pcVar6 = pcVar6 + 4;
        pcVar14 = pcVar14 + 4;
      }
      for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
        *pcVar14 = *pcVar6;
        pcVar6 = pcVar6 + 1;
        pcVar14 = pcVar14 + 1;
      }
      iVar3 = -1;
      pcVar6 = local_7e4;
      do {
        pcVar14 = pcVar6;
        if (iVar3 == 0) break;
        iVar3 = iVar3 + -1;
        pcVar14 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar14;
      } while (cVar1 != '\0');
      *(undefined2 *)(pcVar14 + -1) = DAT_005d9d18;
      if (DAT_0065a784 < 0) {
        iVar3 = 0;
      }
      else {
        iVar3 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
      }
      uVar9 = 0xffffffff;
      pcVar6 = (char *)(iVar3 + 0x58);
      do {
        pcVar14 = pcVar6;
        if (uVar9 == 0) break;
        uVar9 = uVar9 - 1;
        pcVar14 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar14;
      } while (cVar1 != '\0');
      uVar9 = ~uVar9;
      iVar3 = -1;
      pcVar6 = local_7e4;
      do {
        pcVar12 = pcVar6;
        if (iVar3 == 0) break;
        iVar3 = iVar3 + -1;
        pcVar12 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar12;
      } while (cVar1 != '\0');
      pcVar6 = pcVar14 + -uVar9;
      pcVar14 = pcVar12 + -1;
      for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
        *(undefined4 *)pcVar14 = *(undefined4 *)pcVar6;
        pcVar6 = pcVar6 + 4;
        pcVar14 = pcVar14 + 4;
      }
      for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
        *pcVar14 = *pcVar6;
        pcVar6 = pcVar6 + 1;
        pcVar14 = pcVar14 + 1;
      }
      iVar3 = -1;
      pcVar6 = local_7e4;
      do {
        pcVar14 = pcVar6;
        if (iVar3 == 0) break;
        iVar3 = iVar3 + -1;
        pcVar14 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar14;
      } while (cVar1 != '\0');
      *(undefined4 *)(pcVar14 + -1) = s__newgame_sav_005d9d1c._0_4_;
      *(undefined4 *)(pcVar14 + 3) = s__newgame_sav_005d9d1c._4_4_;
      *(undefined4 *)(pcVar14 + 7) = s__newgame_sav_005d9d1c._8_4_;
      pcVar14[0xb] = s__newgame_sav_005d9d1c[0xc];
      goto LAB_0048dbde;
    }
  }
  if (param_2 == (char *)0x0) {
    param_2 = s_Default_Save_005d9ca8;
  }
  iVar3 = 0;
  if (0 < *param_1) {
    do {
      iVar5 = FUN_0059a530(param_2,**(undefined4 **)(param_1[4] + iVar3 * 4));
      if (iVar5 == 0) break;
      iVar3 = iVar3 + 1;
    } while (iVar3 < *param_1);
  }
  FUN_00483120(&DAT_0065da00,local_7e4,0x104);
  pcVar6 = _strrchr(local_7e4,0x5c);
  if (pcVar6 != (char *)0x0) {
    *pcVar6 = '\0';
  }
  CreateDirectoryA(local_7e4,(LPSECURITY_ATTRIBUTES)0x0);
  if (DAT_0066829c == 0) {
    uVar9 = 0xffffffff;
    pcVar6 = local_7e4;
    do {
      if (uVar9 == 0) break;
      uVar9 = uVar9 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    uVar9 = ~uVar9;
    pcVar6 = local_7e4 + (uVar9 - 1);
    pcVar14 = s__Single_005d9cc0;
  }
  else {
    uVar9 = 0xffffffff;
    pcVar6 = local_7e4;
    do {
      if (uVar9 == 0) break;
      uVar9 = uVar9 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    uVar9 = ~uVar9;
    pcVar6 = local_7e4 + (uVar9 - 1);
    pcVar14 = s__Multi_005d9cb8;
  }
  _strncpy(pcVar6,pcVar14,-(uVar9 - 1) + 0x103);
  pcVar6[-(uVar9 - 1) + 0x103] = '\0';
  CreateDirectoryA(local_7e4,(LPSECURITY_ATTRIBUTES)0x0);
  uVar9 = 0xffffffff;
  pcVar6 = local_7e4;
  do {
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  iVar3 = -(~uVar9 - 1);
  _strncpy(local_7e4 + (~uVar9 - 1),&DAT_005d9cc8,iVar3 + 0x103);
  (local_7e4 + (~uVar9 - 1))[iVar3 + 0x103] = '\0';
  uVar9 = 0xffffffff;
  pcVar6 = local_7e4;
  do {
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  iVar3 = -(~uVar9 - 1);
  _strncpy(local_7e4 + (~uVar9 - 1),param_2,iVar3 + 0x103);
  (local_7e4 + (~uVar9 - 1))[iVar3 + 0x103] = '\0';
  _strncpy(acStack_661 + 1,local_7e4,0x103);
  pcVar6 = acStack_661;
  uVar9 = 0xffffffff;
  acStack_55e[1] = 0;
  do {
    pcVar6 = pcVar6 + 1;
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
  } while (*pcVar6 != '\0');
  iVar3 = -(~uVar9 - 1);
  _strncpy(acStack_661 + ~uVar9,s__CurMap_005d9ccc,iVar3 + 0x103);
  (acStack_661 + ~uVar9)[iVar3 + 0x103] = '\0';
  CreateDirectoryA(local_7e4,(LPSECURITY_ATTRIBUTES)0x0);
  CreateDirectoryA(acStack_661 + 1,(LPSECURITY_ATTRIBUTES)0x0);
  GetCurrentDirectoryA(0x104,local_110);
  SetCurrentDirectoryA(local_7e4);
  DeleteFileA(s_game_sav_005d9cd4);
  DeleteFileA(s_ss_bmp_005d9ce0);
  uVar9 = 0xffffffff;
  pcVar6 = acStack_661 + 1;
  do {
    pcVar14 = pcVar6;
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    pcVar14 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar14;
  } while (cVar1 != '\0');
  uVar9 = ~uVar9;
  pcVar6 = pcVar14 + -uVar9;
  pcVar14 = local_318;
  for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *(undefined4 *)pcVar14 = *(undefined4 *)pcVar6;
    pcVar6 = pcVar6 + 4;
    pcVar14 = pcVar14 + 4;
  }
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *pcVar14 = *pcVar6;
    pcVar6 = pcVar6 + 1;
    pcVar14 = pcVar14 + 1;
  }
  iVar3 = -1;
  pcVar6 = local_318;
  do {
    pcVar14 = pcVar6;
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    pcVar14 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar14;
  } while (cVar1 != '\0');
  *(undefined4 *)(pcVar14 + -1) = DAT_005d9ce8;
  pcVar14[3] = DAT_005d9cec;
  SetCurrentDirectoryA(acStack_661 + 1);
  hFindFile = FindFirstFileA(local_318,&local_458);
  if (hFindFile != (HANDLE)0xffffffff) {
    DeleteFileA(local_458.cFileName);
    iVar3 = FindNextFileA(hFindFile,&local_458);
    while (iVar3 != 0) {
      DeleteFileA(local_458.cFileName);
      iVar3 = FindNextFileA(hFindFile,&local_458);
    }
    FindClose(hFindFile);
  }
  SetCurrentDirectoryA(local_110);
  FUN_0044e250(acStack_661 + 1);
  uVar9 = 0xffffffff;
  pcVar6 = local_7e4;
  do {
    pcVar14 = pcVar6;
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    pcVar14 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar14;
  } while (cVar1 != '\0');
  uVar9 = ~uVar9;
  pcVar6 = pcVar14 + -uVar9;
  pcVar14 = (char *)local_55c;
  for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *(undefined4 *)pcVar14 = *(undefined4 *)pcVar6;
    pcVar6 = pcVar6 + 4;
    pcVar14 = pcVar14 + 4;
  }
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *pcVar14 = *pcVar6;
    pcVar6 = pcVar6 + 1;
    pcVar14 = pcVar14 + 1;
  }
  iVar3 = -1;
  pcVar6 = (char *)local_55c;
  do {
    pcVar14 = pcVar6;
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    pcVar14 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar14;
  } while (cVar1 != '\0');
  *(undefined2 *)(pcVar14 + -1) = DAT_005d9cf0;
  iVar3 = -1;
  pcVar6 = (char *)local_55c;
  do {
    pcVar14 = pcVar6;
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    pcVar14 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar14;
  } while (cVar1 != '\0');
  *(undefined4 *)(pcVar14 + -1) = DAT_005d9cf4;
  *(undefined2 *)(pcVar14 + 3) = DAT_005d9cf8;
  pcVar14[5] = DAT_005d9cfa;
  uVar9 = 0xffffffff;
  pcVar6 = &DAT_005d9cfc;
  do {
    pcVar14 = pcVar6;
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    pcVar14 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar14;
  } while (cVar1 != '\0');
  uVar9 = ~uVar9;
  pcVar6 = pcVar14 + -uVar9;
  pcVar14 = local_214;
  for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *(undefined4 *)pcVar14 = *(undefined4 *)pcVar6;
    pcVar6 = pcVar6 + 4;
    pcVar14 = pcVar14 + 4;
  }
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *pcVar14 = *pcVar6;
    pcVar6 = pcVar6 + 1;
    pcVar14 = pcVar14 + 1;
  }
  iVar3 = -1;
  pcVar6 = local_214;
  do {
    pcVar14 = pcVar6;
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    pcVar14 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar14;
  } while (cVar1 != '\0');
  *(undefined4 *)(pcVar14 + -1) = DAT_005d9d00;
  *(undefined2 *)(pcVar14 + 3) = DAT_005d9d04;
  pcVar14[5] = DAT_005d9d06;
  FUN_004814d0(local_214,local_55c);
  iVar3 = -1;
  pcVar6 = local_7e4;
  do {
    pcVar14 = pcVar6;
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    pcVar14 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar14;
  } while (cVar1 != '\0');
  iVar3 = -1;
  *(undefined2 *)(pcVar14 + -1) = DAT_005d9d08;
  pcVar6 = local_7e4;
  do {
    pcVar14 = pcVar6;
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    pcVar14 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar14;
  } while (cVar1 != '\0');
  *(undefined4 *)(pcVar14 + -1) = s_game_sav_005d9d0c._0_4_;
  *(undefined4 *)(pcVar14 + 3) = s_game_sav_005d9d0c._4_4_;
  pcVar14[7] = s_game_sav_005d9d0c[8];
  param_1 = local_808;
LAB_0048dbde:
  _DAT_0065a250 = param_3;
  FUN_0049cc20(0x8000,0x4000);
  local_4 = 0;
  iVar3 = FUN_0058b5db(local_7e4,&DAT_005d9d2c);
  local_808 = (int *)iVar3;
  if (iVar3 == 0) {
    local_4 = 0xffffffff;
    FUN_0049cc50();
    uVar7 = 0;
  }
  else {
    local_804 = 1;
    FUN_0058c3a5(iVar3,0,0);
    puVar11 = local_6e0;
    for (iVar5 = 0x20; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar11 = 0;
      puVar11 = puVar11 + 1;
    }
    local_6e0[0] = FUN_0047e940();
    local_6cc = DAT_0066829c;
    local_6c8 = 2;
    local_6c4 = 0xf;
    if (DAT_0065a784 < 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    _strncpy(local_6c0,(char *)(iVar5 + 0x58),0x1f);
    local_6a1 = 0;
    iVar3 = FUN_0058beb8(local_6e0,0x80,1,iVar3);
    if (iVar3 == 0) {
      local_804 = 0;
    }
    if (DAT_0066829c != 0) {
      puVar11 = &DAT_00676764;
      puVar13 = &DAT_00668300;
      for (iVar3 = 0x31; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar13 = *puVar11;
        puVar11 = puVar11 + 1;
        puVar13 = puVar13 + 1;
      }
      iVar3 = FUN_0058beb8(&DAT_00668300,0x200,1,local_808);
      if (iVar3 == 0) {
        local_804 = 0;
      }
    }
    FUN_004974d0(local_81c);
    iVar3 = param_1[6];
    if ((local_818 - (int)local_814) + local_810 < 4) {
      FUN_0049cc70(4);
    }
    iVar5 = param_1[6];
    *local_814 = iVar3;
    local_814 = local_814 + 1;
    iVar3 = 0;
    if (0 < iVar5) {
      do {
        iVar5 = *(int *)(param_1[7] + iVar3 * 8);
        if ((local_818 - (int)local_814) + local_810 < 4) {
          FUN_0049cc70(4);
        }
        iVar2 = param_1[7];
        *local_814 = iVar5;
        local_814 = local_814 + 1;
        iVar5 = *(int *)(iVar2 + 4 + iVar3 * 8);
        if ((local_818 - (int)local_814) + local_810 < 4) {
          FUN_0049cc70(4);
        }
        iVar2 = param_1[6];
        *local_814 = iVar5;
        local_814 = local_814 + 1;
        iVar3 = iVar3 + 1;
      } while (iVar3 < iVar2);
    }
    iVar3 = FUN_0051ee70(1);
    if ((local_818 - (int)local_814) + local_810 < 4) {
      FUN_0049cc70(4);
    }
    iVar5 = 0;
    *local_814 = iVar3;
    local_814 = local_814 + 1;
    iVar3 = FUN_0051ee70(1);
    if (0 < iVar3) {
      do {
        piVar8 = (int *)FUN_0051eea0(iVar5,1);
        (**(code **)(*piVar8 + 0x40))(piVar8[2] & 0xfbffffff);
        uStack_800 = 0;
        uStack_7f0 = 0;
        uStack_7f4 = 0;
        uStack_7ec = 0;
        uStack_7e8 = (int *)0x0;
        piStack_7fc = piVar8;
        piStack_7f8 = piVar8;
        FUN_0046dfb0();
        while (uStack_7e8 != (int *)0x0) {
          (**(code **)(*uStack_7e8 + 0x40))(uStack_7e8[2] & 0xfbffffff);
          FUN_0046dfb0();
        }
        FUN_00472110(piVar8,local_81c);
        iVar5 = iVar5 + 1;
        iVar3 = FUN_0051ee70(1);
      } while (iVar5 < iVar3);
    }
    piVar8 = local_808;
    uVar7 = FUN_0049cdd0((int)local_814 - local_818,1,local_808);
    iVar3 = FUN_0058beb8(uVar7);
    uVar7 = local_804;
    if (iVar3 == 0) {
      uVar7 = 0;
    }
    FUN_0058b4f1(piVar8);
    _DAT_0065a250 = 0;
    local_4 = 0xffffffff;
    FUN_0049cc50();
  }
  ExceptionList = local_c;
  return uVar7;
}


