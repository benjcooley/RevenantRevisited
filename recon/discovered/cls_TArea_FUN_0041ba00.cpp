// FUN_0041ba00_TArea_Enter @ 0041ba00 size=769

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0041ba00_TArea_Enter(char *param_1)

{
  char cVar1;
  DWORD DVar2;
  int iVar3;
  char *pcVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  bool bVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  char *pcVar13;
  undefined4 uVar14;
  char *pcVar15;
  undefined4 uVar16;
  char *pcVar17;
  undefined1 local_214 [4];
  undefined4 local_210 [2];
  int local_208;
  char acStack_200 [512];
  
  iVar8 = DAT_0066697c;
  iVar7 = DAT_00666970;
  local_208 = DAT_00666980;
  uVar9 = *(uint *)(param_1 + 0x44);
  if ((uVar9 & 2) != 0) {
    return;
  }
  *(uint *)(param_1 + 0x44) = uVar9 | 2;
  if (*(int *)(param_1 + 0x25c) != 0) {
    *(undefined4 *)(*(int *)(param_1 + 0x25c) + 0x20) = 1;
  }
  if ((uVar9 & 0x80) != 0) {
    FUN_0049bef0(*(undefined4 *)(param_1 + 600));
  }
  if ((param_1[0x44] & 0x20U) != 0) {
    param_1[0x1b8] = -1;
    param_1[0x1b9] = -1;
    param_1[0x1ba] = -1;
    param_1[0x1bb] = -1;
    DVar2 = GetTickCount();
    *(DWORD *)(param_1 + 0x248) = DVar2;
    bVar10 = DAT_0065abd8 == 0;
    param_1[0x250] = 'd';
    param_1[0x251] = '\0';
    param_1[0x252] = '\0';
    param_1[0x253] = '\0';
    param_1[0x254] = '\0';
    param_1[0x255] = '\0';
    param_1[0x256] = '\0';
    param_1[599] = '\0';
    if (bVar10) {
      DAT_0065abd8 = 8;
    }
    else {
      _DAT_0065abdc = 8;
    }
  }
  if ((*(uint *)(param_1 + 0x44) & 0x40) != 0) {
    *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x40;
    iVar3 = FUN_0049b880(param_1 + 0x138);
    if (iVar3 == 0) {
      *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) & 0xffffffbf;
    }
    else {
      uVar16 = 700;
      uVar14 = 0x50;
      uVar12 = 0;
      uVar11 = 0;
      uVar6 = 0x7f;
      uVar5 = FUN_0049c430(param_1 + 0x138);
      FUN_0049b990(uVar5,uVar6,uVar11,uVar12,uVar14,uVar16);
    }
  }
  if ((param_1[0x44] & 4U) != 0) {
    FUN_0041b3c0_TArea_GetCurrentAmbient(local_210,local_214);
    if (iVar7 == DAT_005e91ec) {
      iVar7 = iVar8 - DAT_005e91f0;
      if (iVar7 < 0) {
        iVar7 = DAT_005e91f0 - iVar8;
      }
      iVar8 = local_208 - DAT_005e91f4;
      if (iVar8 < 0) {
        iVar8 = DAT_005e91f4 - local_208;
      }
      iVar3 = iVar7;
      if (iVar8 <= iVar7) {
        iVar3 = iVar8;
      }
      if ((iVar8 - (iVar3 >> 1)) + iVar7 < 0x401) {
        FUN_00453720(local_210[0],local_214,0x48,10);
        goto LAB_0041bb8f;
      }
    }
    FUN_00453640(local_210[0],1);
    FUN_004536b0(local_214,1);
  }
LAB_0041bb8f:
  if ((DAT_0066829c == 0) && ((param_1[0x44] & 0x10U) != 0)) {
    iVar7 = FUN_0059a530_stricmp(param_1 + 0x50,s_master_s_005c6d0c);
    if (iVar7 == 0) {
      FUN_00481c10(s_The_MASTER_S_script_file_can_not_005c6d18,0);
    }
    FUN_00496490_TScriptManager_Load(param_1 + 0x50,param_1);
  }
  FUN_0041b690();
  if ((*(uint *)(param_1 + 0x44) & 0x3400) != 0) {
    FUN_0041b550();
  }
  if ((DAT_00667fcc == 0) || (*param_1 == '\0')) {
    return;
  }
  uVar9 = 0xffffffff;
  iVar8 = 0;
  iVar7 = 0;
  pcVar4 = param_1;
  do {
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
  } while (cVar1 != '\0');
  if (0 < (int)(~uVar9 - 1)) {
    do {
      cVar1 = param_1[iVar7];
      if (((('`' < cVar1) && (cVar1 < '{')) || (('@' < cVar1 && (cVar1 < '[')))) ||
         (('/' < cVar1 && (cVar1 < ':')))) {
        acStack_200[iVar8] = cVar1;
        iVar8 = iVar8 + 1;
      }
      uVar9 = 0xffffffff;
      iVar7 = iVar7 + 1;
      pcVar4 = param_1;
      do {
        if (uVar9 == 0) break;
        uVar9 = uVar9 - 1;
        cVar1 = *pcVar4;
        pcVar4 = pcVar4 + 1;
      } while (cVar1 != '\0');
    } while (iVar7 < (int)(~uVar9 - 1));
  }
  acStack_200[iVar8] = '\0';
  pcVar4 = (char *)FUN_0049d800(acStack_200);
  if ((pcVar4 == (char *)0x0) || (*pcVar4 == '[')) {
    pcVar4 = param_1;
  }
  iVar7 = FUN_0049d6d0(s_FULLBASEENTERED_005c6d58);
  if (iVar7 < 0) {
    iVar7 = FUN_0049d6d0(s_FULLBASEENTEREDREV_005c6d78);
    if (iVar7 < 0) {
      uVar5 = *(undefined4 *)(DAT_00667fcc + 0x38);
      uVar6 = FUN_0049d800(s_BASEENTERED_005c6da0);
      FUN_0054d170(&DAT_0065c5d0,s__s__s__s_005c6dac,uVar5,uVar6,pcVar4);
      return;
    }
    pcVar17 = *(char **)(DAT_00667fcc + 0x38);
    pcVar13 = s_FULLBASEENTEREDREV_005c6d8c;
    pcVar15 = pcVar4;
  }
  else {
    pcVar13 = s_FULLBASEENTERED_005c6d68;
    pcVar15 = *(char **)(DAT_00667fcc + 0x38);
    pcVar17 = pcVar4;
  }
  uVar5 = FUN_0049d800(pcVar13);
  FUN_0054d170(&DAT_0065c5d0,uVar5,pcVar15,pcVar17);
  return;
}


