// FUN_004227b0 @ 004227b0 size=701

undefined4 FUN_004227b0(int *param_1,int param_2)

{
  char cVar1;
  bool bVar2;
  int iVar3;
  int *piVar4;
  char *pcVar5;
  uint uVar6;
  uint uVar7;
  int unaff_EBX;
  int iVar8;
  char *pcVar9;
  undefined4 uStack_54;
  char acStack_50 [80];
  
  if ((*(int *)(param_2 + 0x10) != 4) ||
     ((iVar3 = FUN_00479700(&DAT_005cb57c,0), iVar3 == 0 &&
      (iVar3 = FUN_00479700(&DAT_005cb584,0), iVar3 == 0)))) {
    iVar3 = *(int *)(param_2 + 0x10);
    if (iVar3 == 9) {
      FUN_00422a70(param_1,(short)param_1[3]);
      return 1;
    }
    if ((iVar3 == 4) || (iVar3 == 2)) {
      iVar3 = (**(code **)(*param_1 + 0x138))(*(undefined4 *)(param_2 + 0x28),0xffffffff);
    }
    else {
      if (iVar3 != 8) {
        return 4;
      }
      iVar3 = *(int *)(param_2 + 0x14);
    }
    FUN_00479580();
    if (-1 < iVar3) {
      piVar4 = (int *)FUN_0046e8a0();
      iVar8 = (**(code **)(*piVar4 + 0x3c))();
      if (iVar3 < iVar8) {
        (**(code **)(*param_1 + 0x18))(iVar3);
        if (DAT_0067682c != 0) {
          FUN_00586fd0(param_1,iVar3);
        }
        FUN_00422a70(param_1,iVar3);
        return 1;
      }
    }
    FUN_0041ee50(s_Invalid_state_005cb59c);
    return 0;
  }
  iVar3 = FUN_00479700(&DAT_005cb58c,0);
  uStack_54 = iVar3;
  FUN_00479580();
  bVar2 = false;
  if ((*(int *)(param_2 + 0x10) == 4) || (*(int *)(param_2 + 0x10) == 2)) {
    uVar6 = 0xffffffff;
    pcVar5 = *(char **)(param_2 + 0x28);
    do {
      pcVar9 = pcVar5;
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      pcVar9 = pcVar5 + 1;
      cVar1 = *pcVar5;
      pcVar5 = pcVar9;
    } while (cVar1 != '\0');
    uVar6 = ~uVar6;
    pcVar5 = pcVar9 + -uVar6;
    pcVar9 = acStack_50;
    for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(undefined4 *)pcVar9 = *(undefined4 *)pcVar5;
      pcVar5 = pcVar5 + 4;
      pcVar9 = pcVar9 + 4;
    }
    for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
      *pcVar9 = *pcVar5;
      pcVar5 = pcVar5 + 1;
      pcVar9 = pcVar9 + 1;
    }
    FUN_00478a10();
    iVar3 = FUN_00479700(&DAT_005cb594,0);
    if (iVar3 != 0) {
      iVar3 = -1;
      pcVar5 = acStack_50;
      do {
        pcVar9 = pcVar5;
        if (iVar3 == 0) break;
        iVar3 = iVar3 + -1;
        pcVar9 = pcVar5 + 1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar9;
      } while (cVar1 != '\0');
      *(undefined2 *)(pcVar9 + -1) = DAT_005cb598;
      FUN_00479580();
    }
    FUN_0059bd3e(acStack_50);
    uVar6 = 0xffffffff;
    pcVar5 = acStack_50;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar5;
      pcVar5 = pcVar5 + 1;
    } while (cVar1 != '\0');
    uVar6 = ~uVar6;
    if ((0 < (int)(uVar6 - 1)) && (acStack_50[uVar6 - 2] == '*')) {
      acStack_50[uVar6 - 2] = '\0';
    }
    bVar2 = true;
  }
  else if (iVar3 != 0) {
    return 4;
  }
  iVar8 = 0;
  piVar4 = (int *)FUN_0046e8a0();
  iVar3 = (**(code **)(*piVar4 + 0x3c))();
  if (iVar3 < 1) {
    return 1;
  }
  do {
    piVar4 = (int *)FUN_0046e8a0();
    pcVar5 = (char *)(**(code **)(*piVar4 + 0x88))(iVar8);
    if (bVar2) {
      uVar6 = 0xffffffff;
      do {
        pcVar9 = pcVar5;
        if (uVar6 == 0) break;
        uVar6 = uVar6 - 1;
        pcVar9 = pcVar5 + 1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar9;
      } while (cVar1 != '\0');
      uVar6 = ~uVar6;
      pcVar5 = pcVar9 + -uVar6;
      pcVar9 = (char *)&DAT_00654a88;
      for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
        *(undefined4 *)pcVar9 = *(undefined4 *)pcVar5;
        pcVar5 = pcVar5 + 4;
        pcVar9 = pcVar9 + 4;
      }
      for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
        *pcVar9 = *pcVar5;
        pcVar5 = pcVar5 + 1;
        pcVar9 = pcVar9 + 1;
      }
      FUN_0059bd3e(&DAT_00654a88);
      iVar3 = FUN_0058ad30(&DAT_00654a88,&uStack_54);
      if (iVar3 == 0) goto LAB_0042298a;
LAB_00422980:
      FUN_00422a70(param_1,iVar8);
    }
    else {
      if (unaff_EBX == 0) goto LAB_00422980;
      if ((int)param_1 < 1) {
        iVar3 = FUN_0059a530(pcVar5,&uStack_54);
        if (iVar3 != 0) goto LAB_00422980;
      }
      else {
        iVar3 = FUN_0059a600(pcVar5,&uStack_54,param_1);
        if (iVar3 == 0) goto LAB_00422980;
      }
    }
LAB_0042298a:
    iVar8 = iVar8 + 1;
    piVar4 = (int *)FUN_0046e8a0();
    iVar3 = (**(code **)(*piVar4 + 0x3c))();
    if (iVar3 <= iVar8) {
      return 1;
    }
  } while( true );
}


