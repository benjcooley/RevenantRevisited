// FUN_0041fc00 @ 0041fc00 size=353

undefined4 FUN_0041fc00(undefined4 param_1,int param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  undefined4 uVar5;
  char *pcVar6;
  char *pcVar7;
  undefined4 *puVar8;
  char acStack_28 [40];
  
  if (*(int *)(param_2 + 0x10) != 4) {
    return 4;
  }
  uVar3 = 0xffffffff;
  pcVar6 = *(char **)(param_2 + 0x28);
  do {
    pcVar7 = pcVar6;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar7 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar7;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  pcVar6 = pcVar7 + -uVar3;
  pcVar7 = acStack_28;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar7 = *(undefined4 *)pcVar6;
    pcVar6 = pcVar6 + 4;
    pcVar7 = pcVar7 + 4;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar7 = *pcVar6;
    pcVar6 = pcVar6 + 1;
    pcVar7 = pcVar7 + 1;
  }
  FUN_00479580();
  if ((*(int *)(param_2 + 0x10) == 9) || (*(int *)(param_2 + 0x10) == 10)) {
    uVar5 = FUN_004975d0(acStack_28);
    FUN_0041ee50(s__s____d_005cad90,acStack_28,uVar5);
    return 0;
  }
  iVar2 = FUN_00479700(&DAT_005cad9c,0);
  if (iVar2 != 0) {
    FUN_00479580();
  }
  iVar2 = FUN_00479700(&DAT_005cada0,0);
  if ((iVar2 == 0) && (iVar2 = FUN_00479700(&DAT_005cada4,0), iVar2 == 0)) {
    iVar2 = FUN_00479700(&PTR_DAT_005cadac,0);
    if ((iVar2 == 0) && (iVar2 = FUN_00479700(s_false_005cadb0,0), iVar2 == 0)) {
      if (*(int *)(param_2 + 0x10) != 8) {
        return 4;
      }
      uVar5 = *(undefined4 *)(param_2 + 0x14);
      goto LAB_0041fcdd;
    }
    uVar5 = 0;
  }
  else {
    uVar5 = 1;
  }
  FUN_00479580();
LAB_0041fcdd:
  uVar3 = 0;
  if (0 < (int)DAT_0065def0) {
    puVar8 = &DAT_00661ef4;
    do {
      iVar2 = FUN_0059a530(acStack_28,*puVar8);
      if (iVar2 == 0) goto LAB_0041fd11;
      uVar3 = uVar3 + 1;
      puVar8 = puVar8 + 1;
    } while ((int)uVar3 < (int)DAT_0065def0);
  }
  uVar3 = 0xffffffff;
LAB_0041fd11:
  if ((uVar3 < DAT_0065def0) && (-1 < (int)uVar3)) {
    (&DAT_0065def4)[uVar3] = uVar5;
  }
  FUN_00479580();
  return 0;
}


