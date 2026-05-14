// FUN_004dac80_TActionBlock_IsLeft @ 004dac80 size=72

undefined4 __thiscall FUN_004dac80_TActionBlock_IsLeft(int param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  char *pcVar4;
  
  uVar3 = 0xffffffff;
  pcVar4 = param_2;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  iVar2 = FUN_0059a600(param_1 + 4,param_2,uVar3 - 1);
  if (((iVar2 == 0) && (*(char *)(uVar3 + 3 + param_1) == 'l')) &&
     (*(char *)(uVar3 + 4 + param_1) == '\0')) {
    return 1;
  }
  return 0;
}


