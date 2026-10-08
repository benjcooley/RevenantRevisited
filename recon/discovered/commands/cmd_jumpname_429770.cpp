// FUN_00429770 @ 00429770 size=162

undefined4 FUN_00429770(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  char *pcVar6;
  char acStack_100 [256];
  
  if (param_1 != 0) {
    iVar3 = FUN_0041e690(*(undefined4 *)(param_2 + 0x28),param_3,param_4);
    if (iVar3 == 0) {
      return 0;
    }
    pcVar2 = *(char **)(iVar3 + 0x38);
    uVar4 = 0xffffffff;
    iVar3 = 0;
    pcVar6 = pcVar2;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    iVar5 = 0;
    if (0 < (int)(~uVar4 - 1)) {
      do {
        if (0xfd < iVar3) break;
        cVar1 = pcVar2[iVar5];
        if (cVar1 != ' ') {
          acStack_100[iVar3] = cVar1;
          iVar3 = iVar3 + 1;
        }
        iVar5 = iVar5 + 1;
      } while (iVar5 < (int)(~uVar4 - 1));
    }
    acStack_100[iVar3] = '\0';
    FUN_00471290(acStack_100);
    FUN_00479580();
  }
  return 0x2000;
}


