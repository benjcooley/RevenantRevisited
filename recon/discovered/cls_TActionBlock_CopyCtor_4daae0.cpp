// FUN_004daae0_TActionBlock_CopyCtor @ 004daae0 size=76

int * __thiscall FUN_004daae0_TActionBlock_CopyCtor(int *param_1,int *param_2,char *param_3,int param_4)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  int *piVar5;
  char *pcVar6;
  int *piVar7;
  
  piVar7 = param_1;
  for (iVar2 = 0x19; iVar2 != 0; iVar2 = iVar2 + -1) {
    *piVar7 = *param_2;
    param_2 = param_2 + 1;
    piVar7 = piVar7 + 1;
  }
  if (param_3 != (char *)0x0) {
    uVar3 = 0xffffffff;
    do {
      pcVar6 = param_3;
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      pcVar6 = param_3 + 1;
      cVar1 = *param_3;
      param_3 = pcVar6;
    } while (cVar1 != '\0');
    uVar3 = ~uVar3;
    piVar5 = (int *)(pcVar6 + -uVar3);
    piVar7 = param_1;
    for (uVar4 = uVar3 >> 2; piVar7 = piVar7 + 1, uVar4 != 0; uVar4 = uVar4 - 1) {
      *piVar7 = *piVar5;
      piVar5 = piVar5 + 1;
    }
    for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
      *(char *)piVar7 = (char)*piVar5;
      piVar5 = (int *)((int)piVar5 + 1);
      piVar7 = (int *)((int)piVar7 + 1);
    }
  }
  if (param_4 != 0) {
    *param_1 = param_4;
  }
  return param_1;
}


