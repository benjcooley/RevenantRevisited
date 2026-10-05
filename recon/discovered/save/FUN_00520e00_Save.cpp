// FUN_00520e00 @ 00520e00 size=162

void __thiscall FUN_00520e00(int param_1,int param_2)

{
  char cVar1;
  undefined1 uVar2;
  short *psVar3;
  undefined1 *puVar4;
  short sVar5;
  int iVar6;
  char *pcVar7;
  
  FUN_00472980(param_2);
  if (*(char **)(param_1 + 0xd8) == (char *)0x0) {
    sVar5 = 0;
  }
  else {
    iVar6 = -1;
    pcVar7 = *(char **)(param_1 + 0xd8);
    do {
      if (iVar6 == 0) break;
      iVar6 = iVar6 + -1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar7 + 1;
    } while (cVar1 != '\0');
    sVar5 = ~(ushort)iVar6 - 1;
  }
  if ((*(int *)(param_2 + 4) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 0xc) < 2) {
    FUN_0049cc70(2);
  }
  psVar3 = *(short **)(param_2 + 8);
  *psVar3 = sVar5;
  *(short **)(param_2 + 8) = psVar3 + 1;
  if (sVar5 != 0) {
    iVar6 = 0;
    if (0 < sVar5) {
      do {
        uVar2 = *(undefined1 *)(iVar6 + *(int *)(param_1 + 0xd8));
        if ((*(int *)(param_2 + 4) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 0xc) < 1) {
          FUN_0049cc70(1);
        }
        puVar4 = *(undefined1 **)(param_2 + 8);
        *puVar4 = uVar2;
        iVar6 = iVar6 + 1;
        *(undefined1 **)(param_2 + 8) = puVar4 + 1;
      } while (iVar6 < sVar5);
    }
  }
  return;
}


