// FUN_005295e0 @ 005295e0 size=397

void FUN_005295e0(short *param_1,int param_2,int *param_3,int param_4)

{
  short *psVar1;
  int iVar2;
  short sVar3;
  int iVar4;
  ushort *puVar5;
  undefined2 *puVar6;
  ushort uVar7;
  short sVar8;
  short *psVar9;
  
  psVar9 = param_1 + 1;
  param_2 = param_2 * 2;
  iVar4 = (int)psVar9 - (int)param_1;
  sVar3 = *param_1;
  do {
    if (param_2 < iVar4) {
LAB_00529749:
      *param_3 = (*(int *)(param_4 + 8) - *(int *)(param_4 + 4)) / 2;
      FUN_0049cdd0();
      return;
    }
    sVar8 = *psVar9;
    psVar9 = psVar9 + 1;
    if (param_2 < (int)psVar9 - (int)param_1) {
      if ((*(int *)(param_4 + 0xc) - *(int *)(param_4 + 8)) + *(int *)(param_4 + 4) < 2) {
        FUN_0049cc70(2);
      }
      puVar6 = *(undefined2 **)(param_4 + 8);
      iVar4 = *(int *)(param_4 + 0xc);
      iVar2 = *(int *)(param_4 + 4);
      *puVar6 = 0;
      puVar6 = puVar6 + 1;
      *(undefined2 **)(param_4 + 8) = puVar6;
      if ((iVar4 - (int)puVar6) + iVar2 < 2) {
        FUN_0049cc70(2);
      }
      psVar9 = *(short **)(param_4 + 8);
      *psVar9 = sVar3;
      *(short **)(param_4 + 8) = psVar9 + 1;
      goto LAB_00529749;
    }
    if (sVar3 == sVar8) {
      uVar7 = 0;
      do {
        if ((0x7ffe < uVar7) || (param_2 < (int)psVar9 - (int)param_1)) break;
        sVar8 = *psVar9;
        uVar7 = uVar7 + 1;
        if (param_2 <= (int)psVar9 - (int)param_1) {
          psVar9 = psVar9 + 1;
          break;
        }
        psVar9 = psVar9 + 1;
      } while (sVar3 == sVar8);
      if ((*(int *)(param_4 + 0xc) - *(int *)(param_4 + 8)) + *(int *)(param_4 + 4) < 2) {
        FUN_0049cc70(2);
      }
      puVar5 = *(ushort **)(param_4 + 8);
      *puVar5 = uVar7 | 0x8000;
      puVar5 = puVar5 + 1;
      *(ushort **)(param_4 + 8) = puVar5;
      iVar4 = (*(int *)(param_4 + 0xc) - (int)puVar5) + *(int *)(param_4 + 4);
    }
    else {
      if ((*(int *)(param_4 + 0xc) - *(int *)(param_4 + 8)) + *(int *)(param_4 + 4) < 2) {
        FUN_0049cc70(2);
      }
      puVar6 = *(undefined2 **)(param_4 + 8);
      iVar4 = *(int *)(param_4 + 0xc);
      *puVar6 = 0;
      puVar6 = puVar6 + 1;
      *(undefined2 **)(param_4 + 8) = puVar6;
      iVar4 = (iVar4 - (int)puVar6) + *(int *)(param_4 + 4);
    }
    if (iVar4 < 2) {
      FUN_0049cc70(2);
    }
    psVar1 = *(short **)(param_4 + 8);
    *psVar1 = sVar3;
    *(short **)(param_4 + 8) = psVar1 + 1;
    iVar4 = (int)psVar9 - (int)param_1;
    sVar3 = sVar8;
  } while( true );
}


