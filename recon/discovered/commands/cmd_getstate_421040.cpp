// FUN_00421040 @ 00421040 size=108

undefined4 FUN_00421040(int param_1)

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  char *pcVar4;
  char acStack_400 [1024];
  
  FUN_0058b100(acStack_400,s_state_____s__005cafbc,*(int *)(param_1 + 0xd8) + 4);
  if (*(int *)(param_1 + 0xd8) == *(int *)(param_1 + 0xe0)) {
    iVar2 = -1;
    pcVar3 = acStack_400;
    do {
      pcVar4 = pcVar3;
      if (iVar2 == 0) break;
      iVar2 = iVar2 + -1;
      pcVar4 = pcVar3 + 1;
      cVar1 = *pcVar3;
      pcVar3 = pcVar4;
    } while (cVar1 != '\0');
    *(undefined4 *)(pcVar4 + -1) = DAT_005cafcc;
    *(undefined4 *)(pcVar4 + 3) = DAT_005cafd0;
  }
  FUN_0041ee50(acStack_400);
  return 0;
}


