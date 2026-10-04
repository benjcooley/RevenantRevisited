// FUN_00428c30 @ 00428c30 size=272

undefined4 FUN_00428c30(undefined4 param_1,int param_2)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  char *pcVar6;
  char acStack_104 [259];
  undefined1 uStack_1;
  
  uVar5 = 0;
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
  uVar4 = 0xffffffff;
  pcVar6 = acStack_104;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  if (acStack_104[~uVar4 - 2] == '\"') {
    uVar4 = 0xffffffff;
    pcVar6 = acStack_104;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    acStack_104[~uVar4 - 2] = '\0';
  }
  FUN_00479580();
  if (((*(int *)(param_2 + 0x10) != 9) && (*(int *)(param_2 + 0x10) != 10)) &&
     (iVar3 = FUN_00479700(s_multi_005ccbe0,0), iVar3 != 0)) {
    uVar5 = 1;
  }
  FUN_00479580();
  iVar3 = FUN_00460dc0(acStack_104,uVar5);
  if (iVar3 == 0) {
    FUN_0041ee50(s_The_desired_module_could_not_be_c_005ccc0c);
    return 0;
  }
  FUN_0041ee50(s_Module___s__created_successfully_005ccbe8,acStack_104);
  return 0;
}


