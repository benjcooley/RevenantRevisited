// FUN_004236a0 @ 004236a0 size=102

undefined4 FUN_004236a0(int param_1,int param_2)

{
  char cVar1;
  uint uVar2;
  uint uVar3;
  char *pcVar4;
  char *pcVar5;
  
  if ((*(int *)(param_2 + 0x10) != 2) && (*(int *)(param_2 + 0x10) != 4)) {
    return 4;
  }
  uVar2 = 0xffffffff;
  pcVar4 = *(char **)(param_2 + 0x28);
  do {
    pcVar5 = pcVar4;
    if (uVar2 == 0) break;
    uVar2 = uVar2 - 1;
    pcVar5 = pcVar4 + 1;
    cVar1 = *pcVar4;
    pcVar4 = pcVar5;
  } while (cVar1 != '\0');
  uVar2 = ~uVar2;
  pcVar4 = pcVar5 + -uVar2;
  pcVar5 = (char *)&DAT_00654a88;
  for (uVar3 = uVar2 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined4 *)pcVar5 = *(undefined4 *)pcVar4;
    pcVar4 = pcVar4 + 4;
    pcVar5 = pcVar5 + 4;
  }
  for (uVar2 = uVar2 & 3; uVar2 != 0; uVar2 = uVar2 - 1) {
    *pcVar5 = *pcVar4;
    pcVar4 = pcVar4 + 1;
    pcVar5 = pcVar5 + 1;
  }
  FUN_00479580();
  if (param_1 == 0) {
    return 4;
  }
  FUN_00473990(&DAT_00654a88,0xffffffff,0x7f,0xffffffff);
  return 0;
}


