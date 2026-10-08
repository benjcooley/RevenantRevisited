// FUN_00428d40 @ 00428d40 size=263

undefined4 FUN_00428d40(undefined4 param_1,int param_2)

{
  char cVar1;
  char *pcVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  char *pcVar6;
  char acStack_104 [259];
  undefined1 uStack_1;
  
  if (*(int *)(param_2 + 0x10) != 2) {
    return 4;
  }
  pcVar2 = (char *)FUN_0059b6bc(*(undefined4 *)(param_2 + 0x28));
  pcVar6 = pcVar2;
  if (*pcVar2 == '\"') {
    pcVar6 = pcVar2 + 1;
  }
  _strncpy(acStack_104,pcVar6,0x103);
  uStack_1 = 0;
  FUN_00482f80(pcVar2);
  uVar5 = 0xffffffff;
  pcVar6 = acStack_104;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  if (acStack_104[~uVar5 - 2] == '\"') {
    uVar5 = 0xffffffff;
    pcVar6 = acStack_104;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    acStack_104[~uVar5 - 2] = '\0';
  }
  uVar3 = FUN_00460d60(acStack_104,0);
  iVar4 = FUN_004609f0(uVar3);
  if (iVar4 == 0) {
    FUN_0041ee50(s_Could_not_switch_to_module___s___005ccc74,acStack_104);
    return 0;
  }
  FUN_0041ee50(s_Switching_to_module___s_____005ccc38,acStack_104);
  FUN_00482160(s_Options_005ccc58);
  FUN_004824f0(s_LastEditedModule_005ccc60,acStack_104);
  DAT_0065a298 = 1;
  return 0;
}


