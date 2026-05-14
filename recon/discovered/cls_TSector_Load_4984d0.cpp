// FUN_004984d0_TSector_Load @ 004984d0 size=586

undefined4 __thiscall FUN_004984d0_TSector_Load(int param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  char *pcVar8;
  char *pcVar9;
  char *pcVar10;
  char *pcVar11;
  undefined4 *local_110;
  undefined4 *local_10c;
  int local_108;
  char local_104 [260];
  
  uVar5 = 0xffffffff;
  pcVar9 = (char *)(param_1 + 0x14);
  pcVar8 = &DAT_0065c02c;
  do {
    pcVar11 = pcVar8;
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pcVar11 = pcVar8 + 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar11;
  } while (cVar1 != '\0');
  uVar5 = ~uVar5;
  *(undefined4 *)(param_1 + 0x94) = 1;
  pcVar8 = pcVar11 + -uVar5;
  pcVar11 = local_104;
  for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *(undefined4 *)pcVar11 = *(undefined4 *)pcVar8;
    pcVar8 = pcVar8 + 4;
    pcVar11 = pcVar11 + 4;
  }
  for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
    *pcVar11 = *pcVar8;
    pcVar8 = pcVar8 + 1;
    pcVar11 = pcVar11 + 1;
  }
  iVar7 = -1;
  pcVar8 = local_104;
  do {
    pcVar11 = pcVar8;
    if (iVar7 == 0) break;
    iVar7 = iVar7 + -1;
    pcVar11 = pcVar8 + 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar11;
  } while (cVar1 != '\0');
  *(undefined4 *)(pcVar11 + -1) = DAT_005da758;
  uVar5 = 0xffffffff;
  *(undefined **)(pcVar11 + 3) = PTR_DAT_005da75c;
  pcVar8 = pcVar9;
  do {
    pcVar11 = pcVar8;
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pcVar11 = pcVar8 + 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar11;
  } while (cVar1 != '\0');
  uVar5 = ~uVar5;
  iVar7 = -1;
  pcVar8 = local_104;
  do {
    pcVar10 = pcVar8;
    if (iVar7 == 0) break;
    iVar7 = iVar7 + -1;
    pcVar10 = pcVar8 + 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar10;
  } while (cVar1 != '\0');
  pcVar8 = pcVar11 + -uVar5;
  pcVar11 = pcVar10 + -1;
  for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *(undefined4 *)pcVar11 = *(undefined4 *)pcVar8;
    pcVar8 = pcVar8 + 4;
    pcVar11 = pcVar11 + 4;
  }
  for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
    *pcVar11 = *pcVar8;
    pcVar8 = pcVar8 + 1;
    pcVar11 = pcVar11 + 1;
  }
  iVar7 = FUN_004a13f0(local_104,&DAT_005da760);
  if (iVar7 == 0) {
    if (DAT_00667eb8 != '\0') {
      uVar5 = 0xffffffff;
      pcVar8 = &DAT_00667eb8;
      do {
        pcVar11 = pcVar8;
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        pcVar11 = pcVar8 + 1;
        cVar1 = *pcVar8;
        pcVar8 = pcVar11;
      } while (cVar1 != '\0');
      uVar5 = ~uVar5;
      pcVar8 = pcVar11 + -uVar5;
      pcVar11 = local_104;
      for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
        *(undefined4 *)pcVar11 = *(undefined4 *)pcVar8;
        pcVar8 = pcVar8 + 4;
        pcVar11 = pcVar11 + 4;
      }
      for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
        *pcVar11 = *pcVar8;
        pcVar8 = pcVar8 + 1;
        pcVar11 = pcVar11 + 1;
      }
      iVar7 = -1;
      pcVar8 = local_104;
      do {
        pcVar11 = pcVar8;
        if (iVar7 == 0) break;
        iVar7 = iVar7 + -1;
        pcVar11 = pcVar8 + 1;
        cVar1 = *pcVar8;
        pcVar8 = pcVar11;
      } while (cVar1 != '\0');
      *(undefined2 *)(pcVar11 + -1) = DAT_005da764;
      uVar5 = 0xffffffff;
      pcVar8 = pcVar9;
      do {
        pcVar11 = pcVar8;
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        pcVar11 = pcVar8 + 1;
        cVar1 = *pcVar8;
        pcVar8 = pcVar11;
      } while (cVar1 != '\0');
      uVar5 = ~uVar5;
      iVar7 = -1;
      pcVar8 = local_104;
      do {
        pcVar10 = pcVar8;
        if (iVar7 == 0) break;
        iVar7 = iVar7 + -1;
        pcVar10 = pcVar8 + 1;
        cVar1 = *pcVar8;
        pcVar8 = pcVar10;
      } while (cVar1 != '\0');
      pcVar8 = pcVar11 + -uVar5;
      pcVar11 = pcVar10 + -1;
      for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
        *(undefined4 *)pcVar11 = *(undefined4 *)pcVar8;
        pcVar8 = pcVar8 + 4;
        pcVar11 = pcVar11 + 4;
      }
      for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
        *pcVar11 = *pcVar8;
        pcVar8 = pcVar8 + 1;
        pcVar11 = pcVar11 + 1;
      }
      iVar7 = FUN_004a13f0(local_104,&DAT_005da768,0);
      if (iVar7 != 0) goto LAB_00498693;
    }
    uVar5 = 0xffffffff;
    pcVar8 = &DAT_0065b8f4;
    do {
      pcVar11 = pcVar8;
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pcVar11 = pcVar8 + 1;
      cVar1 = *pcVar8;
      pcVar8 = pcVar11;
    } while (cVar1 != '\0');
    uVar5 = ~uVar5;
    pcVar8 = pcVar11 + -uVar5;
    pcVar11 = local_104;
    for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
      *(undefined4 *)pcVar11 = *(undefined4 *)pcVar8;
      pcVar8 = pcVar8 + 4;
      pcVar11 = pcVar11 + 4;
    }
    for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
      *pcVar11 = *pcVar8;
      pcVar8 = pcVar8 + 1;
      pcVar11 = pcVar11 + 1;
    }
    iVar7 = -1;
    pcVar8 = local_104;
    do {
      pcVar11 = pcVar8;
      if (iVar7 == 0) break;
      iVar7 = iVar7 + -1;
      pcVar11 = pcVar8 + 1;
      cVar1 = *pcVar8;
      pcVar8 = pcVar11;
    } while (cVar1 != '\0');
    *(undefined4 *)(pcVar11 + -1) = DAT_005da76c;
    uVar5 = 0xffffffff;
    pcVar11[3] = DAT_005da770;
    do {
      pcVar8 = pcVar9;
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pcVar8 = pcVar9 + 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar8;
    } while (cVar1 != '\0');
    uVar5 = ~uVar5;
    iVar7 = -1;
    pcVar9 = local_104;
    do {
      pcVar11 = pcVar9;
      if (iVar7 == 0) break;
      iVar7 = iVar7 + -1;
      pcVar11 = pcVar9 + 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar11;
    } while (cVar1 != '\0');
    pcVar9 = pcVar8 + -uVar5;
    pcVar8 = pcVar11 + -1;
    for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
      *(undefined4 *)pcVar8 = *(undefined4 *)pcVar9;
      pcVar9 = pcVar9 + 4;
      pcVar8 = pcVar8 + 4;
    }
    for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
      *pcVar8 = *pcVar9;
      pcVar9 = pcVar9 + 1;
      pcVar8 = pcVar8 + 1;
    }
    iVar7 = FUN_004a13f0(local_104,&DAT_005da774);
    if (iVar7 == 0) {
      return 0;
    }
  }
LAB_00498693:
  FUN_004a16c0(iVar7,0,0);
  iVar2 = FUN_004a17b0(iVar7);
  if (iVar2 < 1) {
    puVar3 = (undefined4 *)FUN_00482ef0(4);
    iVar2 = 4;
    *puVar3 = 0;
  }
  else {
    puVar3 = (undefined4 *)FUN_00482ef0(iVar2);
    FUN_004a15a0(puVar3,iVar2,1,iVar7);
  }
  local_110 = puVar3;
  local_10c = puVar3;
  local_108 = iVar2;
  uVar4 = FUN_00498780_TSector_LoadFromStream(&local_110,param_2);
  FUN_004a1540(iVar7);
  FUN_00482f80(puVar3);
  return uVar4;
}


