// FUN_00423610 @ 00423610 size=144

undefined4 FUN_00423610(undefined4 param_1,int param_2)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  char *pcVar6;
  char *pcVar7;
  
  if ((*(int *)(param_2 + 0x10) != 2) && (*(int *)(param_2 + 0x10) != 4)) {
    return 4;
  }
  uVar4 = 0xffffffff;
  pcVar6 = *(char **)(param_2 + 0x28);
  do {
    pcVar7 = pcVar6;
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    pcVar7 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar7;
  } while (cVar1 != '\0');
  uVar4 = ~uVar4;
  pcVar6 = pcVar7 + -uVar4;
  pcVar7 = (char *)&DAT_00654a88;
  for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
    *(undefined4 *)pcVar7 = *(undefined4 *)pcVar6;
    pcVar6 = pcVar6 + 4;
    pcVar7 = pcVar7 + 4;
  }
  for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
    *pcVar7 = *pcVar6;
    pcVar6 = pcVar6 + 1;
    pcVar7 = pcVar7 + 1;
  }
  FUN_00479580();
  iVar2 = FUN_0049c430(&DAT_00654a88);
  if (-1 < iVar2) {
    iVar3 = FUN_0049b650(iVar2);
    if (iVar3 != 0) {
      iVar2 = FUN_0049b990(iVar2,0x7f,1,0,0x50,700);
      if (iVar2 != 0) {
        return 0;
      }
    }
  }
  return 4;
}


