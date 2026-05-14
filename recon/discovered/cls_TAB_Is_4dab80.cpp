// FUN_004dab80_TActionBlock_Is @ 004dab80 size=168

undefined4 __thiscall FUN_004dab80_TActionBlock_Is(int param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  char *pcVar5;
  
  pcVar5 = (char *)(param_1 + 4);
  cVar1 = *(char *)(param_1 + 4);
  while ((cVar1 != '\0' && (cVar2 = *param_2, cVar2 != '\0'))) {
    if ((cVar2 != '?') &&
       ((((cVar2 != '*' && (cVar2 != '[')) && (cVar2 != ']')) &&
        ((cVar2 != '#' || (iVar3 = FUN_0058af1a((int)cVar1), iVar3 == 0)))))) {
      iVar3 = FUN_0058cc2c((int)*pcVar5);
      iVar4 = FUN_0058cc2c((int)*param_2);
      if (iVar4 != iVar3) break;
    }
    if (*param_2 == '[') {
      cVar1 = param_2[1];
      param_2 = param_2 + 1;
      if (cVar1 != '\0') {
        do {
          if (cVar1 == ']') break;
          if (*pcVar5 == cVar1) {
            pcVar5 = pcVar5 + 1;
            break;
          }
        } while (cVar1 != '\0');
        if (cVar1 != '\0') goto LAB_004dabfe;
      }
    }
    else {
      pcVar5 = pcVar5 + 1;
      if (*param_2 != '*') {
LAB_004dabfe:
        param_2 = param_2 + 1;
      }
    }
    cVar1 = *pcVar5;
  }
  if (*param_2 == '*') {
    param_2 = param_2 + 1;
  }
  if ((*pcVar5 == '\0') && (*param_2 == '\0')) {
    return 1;
  }
  return 0;
}


