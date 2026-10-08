// FUN_00516e00 @ 00516e00 size=163

void __thiscall FUN_00516e00(int param_1,int param_2,int param_3,int param_4)

{
  char cVar1;
  char *pcVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 *puVar5;
  char *pcVar6;
  int *piVar7;
  char acStack_50 [80];
  
  FUN_00472430(param_2,param_3,param_4);
  cVar1 = **(char **)(param_2 + 4);
  *(char **)(param_2 + 4) = *(char **)(param_2 + 4) + 1;
  if (cVar1 != '\0') {
    param_4 = (int)cVar1;
    piVar7 = (int *)(param_1 + 0xd8);
    do {
      pcVar6 = acStack_50;
      do {
        pcVar2 = *(char **)(param_2 + 4);
        cVar1 = *pcVar2;
        *pcVar6 = cVar1;
        pcVar6 = pcVar6 + 1;
        *(char **)(param_2 + 4) = pcVar2 + 1;
      } while (cVar1 != '\0');
      uVar3 = *(undefined4 *)(pcVar2 + 1);
      uVar4 = *(undefined4 *)(pcVar2 + 5);
      *(char **)(param_2 + 4) = pcVar2 + 9;
      FUN_00517050(acStack_50,uVar4,uVar3);
      if (0xe < param_3) {
        puVar5 = *(undefined4 **)(param_2 + 4);
        *(undefined4 *)(*piVar7 + 0xc) = *puVar5;
        *(undefined4 **)(param_2 + 4) = puVar5 + 1;
      }
      piVar7 = piVar7 + 1;
      param_4 = param_4 + -1;
    } while (param_4 != 0);
  }
  puVar5 = *(undefined4 **)(param_2 + 4);
  *(undefined4 *)(param_1 + 0xec) = *puVar5;
  *(undefined4 **)(param_2 + 4) = puVar5 + 1;
  return;
}


