// FUN_0046e6f0 @ 0046e6f0 size=209

void __thiscall FUN_0046e6f0(int param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  char *pcVar7;
  char *pcVar8;
  
  iVar2 = *(int *)(param_1 + 0x38);
  if ((iVar2 != 0) && (iVar2 != **(int **)(param_1 + 0x4c))) {
    FUN_00482f80(iVar2);
    *(undefined4 *)(param_1 + 0x38) = 0;
  }
  if ((param_2 == (char *)0x0) || (*param_2 == '\0')) {
    *(undefined4 *)(param_1 + 0x38) = **(undefined4 **)(param_1 + 0x4c);
  }
  else {
    uVar5 = 0xffffffff;
    pcVar3 = param_2;
    do {
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      cVar1 = *pcVar3;
      pcVar3 = pcVar3 + 1;
    } while (cVar1 != '\0');
    pcVar3 = (char *)FUN_00482ef0(~uVar5);
    uVar5 = 0xffffffff;
    do {
      pcVar7 = param_2;
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pcVar7 = param_2 + 1;
      cVar1 = *param_2;
      param_2 = pcVar7;
    } while (cVar1 != '\0');
    uVar5 = ~uVar5;
    pcVar7 = pcVar7 + -uVar5;
    pcVar8 = pcVar3;
    for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
      *(undefined4 *)pcVar8 = *(undefined4 *)pcVar7;
      pcVar7 = pcVar7 + 4;
      pcVar8 = pcVar8 + 4;
    }
    for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
      *pcVar8 = *pcVar7;
      pcVar7 = pcVar7 + 1;
      pcVar8 = pcVar8 + 1;
    }
    *(char **)(param_1 + 0x38) = pcVar3;
  }
  iVar2 = *(int *)(param_1 + 0x84);
  if (iVar2 != 0) {
    *(undefined4 *)(iVar2 + 0xc) = 0;
    FUN_004922c0();
    FUN_004830f0(iVar2);
    *(undefined4 *)(param_1 + 0x84) = 0;
  }
  if (((*(uint *)(param_1 + 8) & 0x40000000) == 0) &&
     ((*(short *)(param_1 + 4) != 9 || (*(int *)(param_1 + 0x38) != **(int **)(param_1 + 0x4c))))) {
    uVar4 = FUN_00497370(param_1);
    FUN_00471150(uVar4);
  }
  return;
}


