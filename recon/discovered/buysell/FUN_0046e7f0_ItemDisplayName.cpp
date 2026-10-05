// FUN_0046e7f0 @ 0046e7f0 size=166

void FUN_0046e7f0(char *param_1,int param_2,char *param_3)

{
  char cVar1;
  char *pcVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  
  if ((param_1 != (char *)0x0) && (param_3 != (char *)0x0)) {
    uVar3 = 0xffffffff;
    iVar4 = 0;
    pcVar2 = param_3;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar2;
      pcVar2 = pcVar2 + 1;
    } while (cVar1 != '\0');
    iVar5 = 0;
    if (0 < (int)(~uVar3 - 1)) {
      do {
        if (param_2 + -2 <= iVar4) break;
        cVar1 = param_3[iVar5];
        if (((('`' < cVar1) && (cVar1 < '{')) || (('@' < cVar1 && (cVar1 < '[')))) ||
           (('/' < cVar1 && (cVar1 < ':')))) {
          param_1[iVar4] = cVar1;
          iVar4 = iVar4 + 1;
        }
        iVar5 = iVar5 + 1;
      } while (iVar5 < (int)(~uVar3 - 1));
    }
    param_1[iVar4] = '\0';
    pcVar2 = (char *)FUN_0049d800(param_1);
    if ((pcVar2 != (char *)0x0) && (*pcVar2 == '[')) {
      _strncpy(param_1,param_3,param_2 - 1);
      param_1[param_2 + -1] = '\0';
      return;
    }
    _strncpy(param_1,pcVar2,param_2 - 1);
    param_1[param_2 + -1] = '\0';
  }
  return;
}


