// FUN_00423040 @ 00423040 size=153

undefined4 FUN_00423040(undefined4 param_1,int param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  char *pcVar5;
  undefined4 uVar6;
  char *pcVar7;
  
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  uVar3 = 0xffffffff;
  pcVar5 = *(char **)(param_2 + 0x28);
  do {
    pcVar7 = pcVar5;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar7 = pcVar5 + 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar7;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  pcVar5 = pcVar7 + -uVar3;
  pcVar7 = (char *)&DAT_00654a88;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar7 = *(undefined4 *)pcVar5;
    pcVar5 = pcVar5 + 4;
    pcVar7 = pcVar7 + 4;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar7 = *pcVar5;
    pcVar5 = pcVar5 + 1;
    pcVar7 = pcVar7 + 1;
  }
  FUN_00479580();
  uVar6 = 1;
  if (*(int *)(param_2 + 0x10) == 4) {
    iVar2 = FUN_00479700(s_noambient_005cb674,1);
    if (iVar2 != 0) {
      uVar6 = 0;
      FUN_00479580();
    }
  }
  FUN_0050cfc0(&DAT_00654a88,param_1,uVar6);
  FUN_0041ee50(s_Exit_from___s__added__005cb680,&DAT_00654a88);
  FUN_0050cca0();
  return 0;
}


