// FUN_004288f0 @ 004288f0 size=300

undefined4 FUN_004288f0(int param_1,int param_2)

{
  char cVar1;
  int *piVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  char *pcVar6;
  char *pcVar7;
  char *pcVar8;
  undefined4 uStack_208;
  char acStack_204 [260];
  char acStack_100 [256];
  
  if (param_1 == 0) {
    return 0x20;
  }
  if (*(int *)(param_2 + 0x28) == 0) {
    return 4;
  }
  uStack_208 = acStack_204;
  piVar2 = (int *)FUN_0046e8a0();
  if (piVar2 == (int *)0x0) {
    return 0x20;
  }
  uVar3 = 0xffffffff;
  pcVar8 = *(char **)(param_2 + 0x28);
  do {
    pcVar6 = pcVar8;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar6 = pcVar8 + 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar6;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  pcVar8 = pcVar6 + -uVar3;
  pcVar6 = acStack_204;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar6 = *(undefined4 *)pcVar8;
    pcVar8 = pcVar8 + 4;
    pcVar6 = pcVar6 + 4;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar6 = *pcVar8;
    pcVar8 = pcVar8 + 1;
    pcVar6 = pcVar6 + 1;
  }
  if (acStack_204[0] == '\"') {
    uStack_208 = acStack_204 + 1;
  }
  uVar3 = 0xffffffff;
  pcVar8 = acStack_204;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar8 + 1;
  } while (cVar1 != '\0');
  if (*(char *)((int)&uStack_208 + ~uVar3 + 2) == '\"') {
    uVar3 = 0xffffffff;
    pcVar8 = acStack_204;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar8;
      pcVar8 = pcVar8 + 1;
    } while (cVar1 != '\0');
    *(undefined1 *)((int)&uStack_208 + ~uVar3 + 2) = 0;
  }
  FUN_00479580();
  if ((*(int *)(param_2 + 0x10) == 9) || (*(int *)(param_2 + 0x10) == 10)) {
    iVar5 = *piVar2;
    pcVar8 = (char *)0x0;
  }
  else {
    uVar3 = 0xffffffff;
    pcVar8 = *(char **)(param_2 + 0x28);
    do {
      pcVar6 = pcVar8;
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      pcVar6 = pcVar8 + 1;
      cVar1 = *pcVar8;
      pcVar8 = pcVar6;
    } while (cVar1 != '\0');
    uVar3 = ~uVar3;
    pcVar6 = pcVar6 + -uVar3;
    pcVar7 = acStack_100;
    for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined4 *)pcVar7 = *(undefined4 *)pcVar6;
      pcVar6 = pcVar6 + 4;
      pcVar7 = pcVar7 + 4;
    }
    pcVar8 = acStack_100;
    for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
      *pcVar7 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      pcVar7 = pcVar7 + 1;
    }
    iVar5 = *piVar2;
  }
  (**(code **)(iVar5 + 0xe8))(uStack_208,pcVar8,1);
  FUN_004795a0();
  return 0;
}


