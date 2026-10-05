// FUN_0049b650 @ 0049b650 size=556

undefined4 __thiscall FUN_0049b650(int *param_1,int param_2)

{
  char cVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  char *pcVar9;
  char *pcVar10;
  char *pcVar11;
  int local_10c;
  int *local_108;
  char local_104 [260];
  
  if (((((DAT_00668114 != 0) || (param_1[7] < 1)) || (param_2 < 0)) ||
      ((param_1[7] <= param_2 || (*param_1 == 0)))) ||
     (puVar2 = *(undefined4 **)(param_1[0xb] + param_2 * 4), puVar2 == (undefined4 *)0x0)) {
    return 0;
  }
  if (puVar2[4] != 0) {
    return 1;
  }
  uVar6 = 0xffffffff;
  pcVar9 = (char *)puVar2[2];
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
  pcVar11 = local_104;
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
  uVar6 = 0xffffffff;
  pcVar9 = (char *)*puVar2;
  do {
    pcVar11 = pcVar9;
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    pcVar11 = pcVar9 + 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar11;
  } while (cVar1 != '\0');
  uVar6 = ~uVar6;
  iVar8 = -1;
  pcVar9 = local_104;
  do {
    pcVar10 = pcVar9;
    if (iVar8 == 0) break;
    iVar8 = iVar8 + -1;
    pcVar10 = pcVar9 + 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar10;
  } while (cVar1 != '\0');
  pcVar9 = pcVar11 + -uVar6;
  pcVar11 = pcVar10 + -1;
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
  uVar6 = 0xffffffff;
  pcVar9 = (char *)puVar2[1];
  do {
    pcVar11 = pcVar9;
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    pcVar11 = pcVar9 + 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar11;
  } while (cVar1 != '\0');
  uVar6 = ~uVar6;
  iVar8 = -1;
  pcVar9 = local_104;
  do {
    pcVar10 = pcVar9;
    if (iVar8 == 0) break;
    iVar8 = iVar8 + -1;
    pcVar10 = pcVar9 + 1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar10;
  } while (cVar1 != '\0');
  pcVar9 = pcVar11 + -uVar6;
  pcVar11 = pcVar10 + -1;
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
  iVar8 = FUN_004a1240(local_104,&DAT_005da844,0);
  if (iVar8 != 0) {
    uVar4 = FUN_004a17b0(iVar8);
    puVar2[3] = uVar4;
    uVar4 = FUN_00482fb0(uVar4);
    puVar2[4] = uVar4;
    iVar5 = FUN_004a15a0(uVar4,puVar2[3],1,iVar8);
    if (iVar5 != 1) {
      FUN_004830f0(puVar2[4]);
      puVar2[4] = 0;
      puVar2[3] = 0;
      return 0;
    }
    FUN_004a1540(iVar8);
    local_10c = 100;
    uVar6 = FUN_004835f0();
    if (uVar6 / 0x14 < (uint)(puVar2[3] + param_1[0x2f])) {
      do {
        if (local_10c < 1) break;
        iVar8 = 0;
        uVar6 = 0xffffffff;
        iVar5 = 0;
        if (0 < param_1[7]) {
          local_108 = (int *)param_1[0xb];
          do {
            iVar3 = *local_108;
            if (((iVar3 != 0) && (*(int *)(iVar3 + 0x10) != 0)) &&
               (((*(byte *)(iVar3 + 0x18) & 8) != 0 && (*(uint *)(iVar3 + 0x14) < uVar6)))) {
              uVar6 = *(uint *)(iVar3 + 0x14);
              iVar8 = iVar5;
            }
            iVar5 = iVar5 + 1;
            local_108 = local_108 + 1;
          } while (iVar5 < param_1[7]);
        }
        FUN_0049b8e0(iVar8,1);
        local_10c = local_10c + -1;
        uVar6 = FUN_004835f0();
      } while (uVar6 / 0x14 < (uint)(puVar2[3] + param_1[0x2f]));
    }
  }
  iVar8 = param_1[0x2f];
  puVar2[6] = puVar2[6] & 0xffffffef | 8;
  param_1[0x2f] = iVar8 + puVar2[3];
  return 1;
}


