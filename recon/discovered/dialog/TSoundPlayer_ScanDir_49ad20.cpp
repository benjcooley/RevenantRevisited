// FUN_0049ad20 @ 0049ad20 size=686

undefined4 FUN_0049ad20(char *param_1)

{
  char cVar1;
  int iVar2;
  undefined1 *puVar3;
  undefined4 *puVar4;
  char *pcVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  char *pcVar9;
  undefined4 *puVar10;
  char *pcVar11;
  char local_4ac [128];
  undefined1 local_42c [16];
  undefined4 local_41c;
  char local_418 [528];
  char local_208 [260];
  char local_104 [260];
  
  uVar6 = 0xffffffff;
  pcVar9 = param_1;
  do {
    pcVar5 = pcVar9;
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    pcVar5 = pcVar9 + 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar5;
  } while (cVar1 != '\0');
  uVar6 = ~uVar6;
  pcVar9 = pcVar5 + -uVar6;
  pcVar5 = local_208;
  for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
    *(undefined4 *)pcVar5 = *(undefined4 *)pcVar9;
    pcVar9 = pcVar9 + 4;
    pcVar5 = pcVar5 + 4;
  }
  for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
    *pcVar5 = *pcVar9;
    pcVar9 = pcVar9 + 1;
    pcVar5 = pcVar5 + 1;
  }
  uVar6 = 0xffffffff;
  pcVar9 = param_1;
  do {
    pcVar5 = pcVar9;
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    pcVar5 = pcVar9 + 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar5;
  } while (cVar1 != '\0');
  uVar6 = ~uVar6;
  pcVar9 = pcVar5 + -uVar6;
  pcVar5 = local_104;
  for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
    *(undefined4 *)pcVar5 = *(undefined4 *)pcVar9;
    pcVar9 = pcVar9 + 4;
    pcVar5 = pcVar5 + 4;
  }
  for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
    *pcVar5 = *pcVar9;
    pcVar9 = pcVar9 + 1;
    pcVar5 = pcVar5 + 1;
  }
  iVar8 = -1;
  pcVar9 = local_208;
  do {
    pcVar5 = pcVar9;
    if (iVar8 == 0) break;
    iVar8 = iVar8 + -1;
    pcVar5 = pcVar9 + 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar5;
  } while (cVar1 != '\0');
  *(undefined4 *)(pcVar5 + -1) = DAT_005da7d0;
  iVar8 = -1;
  *(undefined2 *)(pcVar5 + 3) = DAT_005da7d4;
  pcVar9 = local_104;
  do {
    pcVar5 = pcVar9;
    if (iVar8 == 0) break;
    iVar8 = iVar8 + -1;
    pcVar5 = pcVar9 + 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar5;
  } while (cVar1 != '\0');
  *(undefined4 *)(pcVar5 + -1) = DAT_005da7d8;
  *(undefined2 *)(pcVar5 + 3) = DAT_005da7dc;
  iVar2 = FUN_004a19d0(local_104,local_42c);
  iVar8 = iVar2;
  while (iVar8 != -1) {
    uVar6 = 0xffffffff;
    pcVar9 = local_418;
    do {
      pcVar5 = pcVar9;
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      pcVar5 = pcVar9 + 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar5;
    } while (cVar1 != '\0');
    uVar6 = ~uVar6;
    pcVar9 = pcVar5 + -uVar6;
    pcVar5 = local_4ac;
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined4 *)pcVar5 = *(undefined4 *)pcVar9;
      pcVar9 = pcVar9 + 4;
      pcVar5 = pcVar5 + 4;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *pcVar5 = *pcVar9;
      pcVar9 = pcVar9 + 1;
      pcVar5 = pcVar5 + 1;
    }
    puVar3 = (undefined1 *)FUN_0058ade0(local_4ac,0x2e);
    if (puVar3 != (undefined1 *)0x0) {
      *puVar3 = 0;
    }
    puVar4 = (undefined4 *)FUN_00482fb0(0x1c);
    puVar10 = puVar4;
    for (iVar8 = 7; iVar8 != 0; iVar8 = iVar8 + -1) {
      *puVar10 = 0;
      puVar10 = puVar10 + 1;
    }
    uVar6 = 0xffffffff;
    puVar4[2] = param_1;
    pcVar9 = local_4ac;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    pcVar5 = (char *)FUN_00482ef0(~uVar6);
    uVar6 = 0xffffffff;
    pcVar9 = local_4ac;
    do {
      pcVar11 = pcVar9;
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      pcVar11 = pcVar9 + 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar11;
    } while (cVar1 != '\0');
    uVar6 = ~uVar6;
    pcVar9 = pcVar11 + -uVar6;
    pcVar11 = pcVar5;
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined4 *)pcVar11 = *(undefined4 *)pcVar9;
      pcVar9 = pcVar9 + 4;
      pcVar11 = pcVar11 + 4;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *pcVar11 = *pcVar9;
      pcVar9 = pcVar9 + 1;
      pcVar11 = pcVar11 + 1;
    }
    *puVar4 = pcVar5;
    puVar4[1] = &DAT_005da7e0;
    puVar4[3] = local_41c;
    puVar4[6] = 0;
    FUN_0041c840(puVar4);
    iVar8 = FUN_004a1b20(iVar2,local_42c);
  }
  iVar2 = FUN_004a19d0(local_208,local_42c);
  iVar8 = iVar2;
  do {
    if (iVar8 == -1) {
      return 1;
    }
    uVar6 = 0xffffffff;
    pcVar9 = local_418;
    do {
      pcVar5 = pcVar9;
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      pcVar5 = pcVar9 + 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar5;
    } while (cVar1 != '\0');
    uVar6 = ~uVar6;
    pcVar9 = pcVar5 + -uVar6;
    pcVar5 = local_4ac;
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined4 *)pcVar5 = *(undefined4 *)pcVar9;
      pcVar9 = pcVar9 + 4;
      pcVar5 = pcVar5 + 4;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *pcVar5 = *pcVar9;
      pcVar9 = pcVar9 + 1;
      pcVar5 = pcVar5 + 1;
    }
    puVar3 = (undefined1 *)FUN_0058ade0(local_4ac,0x2e);
    if (puVar3 != (undefined1 *)0x0) {
      *puVar3 = 0;
    }
    puVar4 = (undefined4 *)FUN_00482fb0(0x1c);
    puVar10 = puVar4;
    for (iVar8 = 7; iVar8 != 0; iVar8 = iVar8 + -1) {
      *puVar10 = 0;
      puVar10 = puVar10 + 1;
    }
    uVar6 = 0xffffffff;
    puVar4[2] = param_1;
    pcVar9 = local_4ac;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    pcVar5 = (char *)FUN_00482ef0(~uVar6);
    uVar6 = 0xffffffff;
    pcVar9 = local_4ac;
    do {
      pcVar11 = pcVar9;
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      pcVar11 = pcVar9 + 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar11;
    } while (cVar1 != '\0');
    uVar6 = ~uVar6;
    pcVar9 = pcVar11 + -uVar6;
    pcVar11 = pcVar5;
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined4 *)pcVar11 = *(undefined4 *)pcVar9;
      pcVar9 = pcVar9 + 4;
      pcVar11 = pcVar11 + 4;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *pcVar11 = *pcVar9;
      pcVar9 = pcVar9 + 1;
      pcVar11 = pcVar11 + 1;
    }
    *puVar4 = pcVar5;
    puVar4[1] = &DAT_005da7e8;
    puVar4[3] = local_41c;
    puVar4[6] = 2;
    FUN_0041c840(puVar4);
    iVar8 = FUN_004a1b20(iVar2,local_42c);
  } while( true );
}


