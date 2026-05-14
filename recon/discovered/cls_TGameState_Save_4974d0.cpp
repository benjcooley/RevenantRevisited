// FUN_004974d0_TGameState_SaveStream @ 004974d0 size=230

undefined4 __thiscall FUN_004974d0_TGameState_SaveStream(int *param_1,int param_2)

{
  char cVar1;
  char *pcVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int *piVar8;
  char *pcVar9;
  byte local_34 [52];
  
  iVar5 = param_2;
  iVar7 = *param_1;
  if ((*(int *)(param_2 + 0xc) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 4) < 4) {
    FUN_0049cc70(4);
  }
  piVar8 = *(int **)(param_2 + 8);
  param_2 = 0;
  *piVar8 = iVar7;
  *(int **)(iVar5 + 8) = piVar8 + 1;
  if (0 < *param_1) {
    piVar8 = param_1 + 0x1001;
    do {
      pcVar2 = (char *)*piVar8;
      uVar6 = 0xffffffff;
      iVar7 = 0;
      pcVar9 = pcVar2;
      do {
        if (uVar6 == 0) break;
        uVar6 = uVar6 - 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar9 + 1;
      } while (cVar1 != '\0');
      if (0 < (int)(~uVar6 - 1)) {
        do {
          local_34[iVar7] = pcVar2[iVar7] | 0x80;
          uVar6 = 0xffffffff;
          iVar7 = iVar7 + 1;
          pcVar9 = pcVar2;
          do {
            if (uVar6 == 0) break;
            uVar6 = uVar6 - 1;
            cVar1 = *pcVar9;
            pcVar9 = pcVar9 + 1;
          } while (cVar1 != '\0');
        } while (iVar7 < (int)(~uVar6 - 1));
      }
      local_34[iVar7] = 0;
      FUN_0049ccc0(local_34);
      iVar7 = piVar8[-0x1000];
      if ((*(int *)(iVar5 + 0xc) - *(int *)(iVar5 + 8)) + *(int *)(iVar5 + 4) < 4) {
        FUN_0049cc70(4);
      }
      piVar3 = *(int **)(iVar5 + 8);
      piVar8 = piVar8 + 1;
      iVar4 = *param_1;
      *piVar3 = iVar7;
      *(int **)(iVar5 + 8) = piVar3 + 1;
      param_2 = param_2 + 1;
    } while (param_2 < iVar4);
  }
  return 1;
}


