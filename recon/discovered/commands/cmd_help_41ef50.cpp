// FUN_0041ef50 @ 0041ef50 size=346

undefined4 FUN_0041ef50(undefined4 param_1,int param_2)

{
  undefined **ppuVar1;
  char cVar2;
  int iVar3;
  undefined **ppuVar4;
  int iVar5;
  undefined4 *puVar6;
  char *pcVar7;
  char *pcVar8;
  char cStack_100;
  undefined4 uStack_ff;
  
  if (*(int *)(param_2 + 0x10) != 4) {
    FUN_0041ee50(s_The_following_commands_are_curre_005cabfc);
    cStack_100 = DAT_00655498;
    puVar6 = &uStack_ff;
    for (iVar5 = 0x3f; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar6 = 0;
      puVar6 = puVar6 + 1;
    }
    *(undefined2 *)puVar6 = 0;
    *(undefined1 *)((int)puVar6 + 2) = 0;
    iVar5 = 0;
    if (PTR_s_activate_005c6e88 != (undefined *)0x0) {
      ppuVar4 = &PTR_s_activate_005c6e88;
      do {
        FUN_0058b100(&cStack_100,s__s___10s_005cac30,&cStack_100,*ppuVar4);
        iVar5 = iVar5 + 1;
        if (4 < iVar5) {
          iVar5 = -1;
          pcVar7 = &cStack_100;
          do {
            pcVar8 = pcVar7;
            if (iVar5 == 0) break;
            iVar5 = iVar5 + -1;
            pcVar8 = pcVar7 + 1;
            cVar2 = *pcVar7;
            pcVar7 = pcVar8;
          } while (cVar2 != '\0');
          *(undefined2 *)(pcVar8 + -1) = DAT_005cac3c;
          FUN_0041ee50(&cStack_100);
          cStack_100 = '\0';
          iVar5 = 0;
        }
        ppuVar1 = ppuVar4 + 7;
        ppuVar4 = ppuVar4 + 7;
      } while (*ppuVar1 != (undefined *)0x0);
      if (iVar5 != 0) {
        iVar5 = -1;
        pcVar7 = &cStack_100;
        do {
          pcVar8 = pcVar7;
          if (iVar5 == 0) break;
          iVar5 = iVar5 + -1;
          pcVar8 = pcVar7 + 1;
          cVar2 = *pcVar7;
          pcVar7 = pcVar8;
        } while (cVar2 != '\0');
        *(undefined2 *)(pcVar8 + -1) = DAT_005cac40;
        FUN_0041ee50(&cStack_100);
      }
    }
    return 0;
  }
  iVar5 = 0;
  if (PTR_s_activate_005c6e88 != (undefined *)0x0) {
    ppuVar4 = &PTR_s_activate_005c6e88;
    do {
      iVar3 = FUN_00479700(*ppuVar4,1);
      if (iVar3 != 0) {
        FUN_0041ee50((&PTR_s_usage__<object>_activate_005c6ea0)[iVar5 * 7]);
        break;
      }
      ppuVar1 = ppuVar4 + 7;
      ppuVar4 = ppuVar4 + 7;
      iVar5 = iVar5 + 1;
    } while (*ppuVar1 != (undefined *)0x0);
  }
  if ((&PTR_s_activate_005c6e88)[iVar5 * 7] == (undefined *)0x0) {
    FUN_0041ee50(s_Command_not_found__005cabe8);
  }
  FUN_00479580();
  return 0;
}


