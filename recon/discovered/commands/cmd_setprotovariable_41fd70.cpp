// FUN_0041fd70 @ 0041fd70 size=185

undefined4 FUN_0041fd70(undefined4 param_1,int param_2,undefined4 param_3)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  char *pcVar6;
  char *pcVar7;
  char acStack_28 [40];
  
  iVar2 = param_2;
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
  pcVar7 = acStack_28;
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
  iVar3 = FUN_00479700(&DAT_005cadb8,0);
  if (iVar3 != 0) {
    FUN_00479580();
  }
  iVar3 = FUN_00497b40(acStack_28,param_1);
  if (iVar3 == 0) {
    FUN_0041f230(iVar2,&param_2,param_1,param_3);
    FUN_00497700(acStack_28,param_2,param_1);
  }
  else if (iVar3 == 1) {
    FUN_00497910(acStack_28,*(undefined4 *)(iVar2 + 0x28),param_1);
    return 0;
  }
  return 0;
}


