// FUN_004210f0 @ 004210f0 size=29

undefined4 FUN_004210f0(undefined4 param_1,int param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  char *pcVar5;
  char *pcVar6;
  char acStack_50 [80];
  
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  uVar3 = 0xffffffff;
  pcVar5 = *(char **)(param_2 + 0x28);
  do {
    pcVar6 = pcVar5;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar6 = pcVar5 + 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar6;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  pcVar5 = pcVar6 + -uVar3;
  pcVar6 = acStack_50;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar6 = *(undefined4 *)pcVar5;
    pcVar5 = pcVar5 + 4;
    pcVar6 = pcVar6 + 4;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar6 = *pcVar5;
    pcVar5 = pcVar5 + 1;
    pcVar6 = pcVar6 + 1;
  }
  FUN_00479580();
  iVar2 = FUN_004d5b90(acStack_50,0,0,0);
  if (iVar2 == 0) {
    iVar2 = FUN_004d5c20(acStack_50,0,0,0);
    if (iVar2 == 0) {
      FUN_0041ee50(s_Spell_failed__005cafd8);
      return 0;
    }
  }
  FUN_0041ee50(s_Spell_cast__005cafe8);
  return 0;
}


