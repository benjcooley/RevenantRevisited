// FUN_00517050 @ 00517050 size=185

undefined4 __thiscall FUN_00517050(int param_1,char *param_2,uint param_3,undefined4 param_4)

{
  char cVar1;
  int *piVar2;
  undefined4 *puVar3;
  char *pcVar4;
  undefined4 uVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  char *pcVar9;
  char *pcVar10;
  
  iVar8 = 0;
  piVar2 = (int *)(param_1 + 0xd8);
  while (*piVar2 != 0) {
    iVar8 = iVar8 + 1;
    piVar2 = piVar2 + 1;
    if (4 < iVar8) {
      return 0;
    }
  }
  if (4 < iVar8) {
    return 0;
  }
  puVar3 = (undefined4 *)FUN_00482ef0(0x10);
  *(undefined4 **)(param_1 + 0xd8 + iVar8 * 4) = puVar3;
  uVar6 = 0xffffffff;
  pcVar4 = param_2;
  do {
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
  } while (cVar1 != '\0');
  pcVar4 = (char *)FUN_00482ef0(~uVar6);
  uVar6 = 0xffffffff;
  do {
    pcVar9 = param_2;
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    pcVar9 = param_2 + 1;
    cVar1 = *param_2;
    param_2 = pcVar9;
  } while (cVar1 != '\0');
  uVar6 = ~uVar6;
  pcVar9 = pcVar9 + -uVar6;
  pcVar10 = pcVar4;
  for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
    *(undefined4 *)pcVar10 = *(undefined4 *)pcVar9;
    pcVar9 = pcVar9 + 4;
    pcVar10 = pcVar10 + 4;
  }
  for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
    *pcVar10 = *pcVar9;
    pcVar9 = pcVar9 + 1;
    pcVar10 = pcVar10 + 1;
  }
  *puVar3 = pcVar4;
  if (0x4e1e < param_3) {
    param_3 = 19999;
  }
  puVar3[1] = param_3;
  puVar3[2] = param_4;
  uVar5 = FUN_0047e940();
  *(undefined4 *)(*(int *)(param_1 + 0xd8 + iVar8 * 4) + 0xc) = uVar5;
  return 1;
}


