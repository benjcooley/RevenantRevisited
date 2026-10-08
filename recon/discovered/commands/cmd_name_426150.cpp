// FUN_00426150 @ 00426150 size=208

undefined4 FUN_00426150(int param_1,int param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  char *pcVar5;
  char *pcVar6;
  
  if ((*(int *)(param_2 + 0x10) != 4) && (*(int *)(param_2 + 0x10) != 2)) {
    return 4;
  }
  iVar2 = FUN_00479700(&DAT_005cbf70,0);
  if (iVar2 == 0) {
    iVar2 = FUN_00479700(s_clear_005cbf78,0);
    if (iVar2 != 0) {
      FUN_0046e6f0(&DAT_0065549c);
      FUN_00479580();
      return 0;
    }
    FUN_0046e6f0(*(undefined4 *)(param_2 + 0x28));
    if (*(short *)(param_1 + 4) == 0xb) {
      FUN_00587a20(param_1);
    }
    FUN_00479580();
    return 0;
  }
  FUN_00479580();
  if (*(int *)(param_2 + 0x10) != 4) {
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
  pcVar6 = (char *)**(undefined4 **)(param_1 + 0x4c);
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
  return 0;
}


