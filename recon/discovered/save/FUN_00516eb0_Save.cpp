// FUN_00516eb0 @ 00516eb0 size=406

void __thiscall FUN_00516eb0(int param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  int *piVar3;
  char *pcVar4;
  undefined4 *puVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  char cVar9;
  int *piVar10;
  char *pcVar11;
  int iStack_58;
  char acStack_50 [80];
  
  FUN_00472980(param_2);
  piVar10 = (int *)(param_1 + 0xd8);
  cVar9 = '\0';
  iVar6 = 5;
  piVar3 = piVar10;
  do {
    if (*piVar3 != 0) {
      cVar9 = cVar9 + '\x01';
    }
    piVar3 = piVar3 + 1;
    iVar6 = iVar6 + -1;
  } while (iVar6 != 0);
  if ((*(int *)(param_2 + 0xc) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 4) < 1) {
    FUN_0049cc70(1);
  }
  pcVar11 = *(char **)(param_2 + 8);
  iStack_58 = 5;
  *pcVar11 = cVar9;
  *(char **)(param_2 + 8) = pcVar11 + 1;
  do {
    if ((undefined4 *)*piVar10 != (undefined4 *)0x0) {
      uVar7 = 0xffffffff;
      pcVar11 = *(char **)*piVar10;
      do {
        pcVar4 = pcVar11;
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        pcVar4 = pcVar11 + 1;
        cVar9 = *pcVar11;
        pcVar11 = pcVar4;
      } while (cVar9 != '\0');
      uVar7 = ~uVar7;
      pcVar11 = pcVar4 + -uVar7;
      pcVar4 = acStack_50;
      for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
        *(undefined4 *)pcVar4 = *(undefined4 *)pcVar11;
        pcVar11 = pcVar11 + 4;
        pcVar4 = pcVar4 + 4;
      }
      for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
        *pcVar4 = *pcVar11;
        pcVar11 = pcVar11 + 1;
        pcVar4 = pcVar4 + 1;
      }
      pcVar11 = acStack_50;
      do {
        cVar9 = *pcVar11;
        pcVar11 = pcVar11 + 1;
        if ((*(int *)(param_2 + 0xc) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 4) < 1) {
          FUN_0049cc70(1);
        }
        pcVar4 = *(char **)(param_2 + 8);
        *pcVar4 = cVar9;
        pcVar4 = pcVar4 + 1;
        *(char **)(param_2 + 8) = pcVar4;
      } while (cVar9 != '\0');
      uVar1 = *(undefined4 *)(*piVar10 + 8);
      if ((*(int *)(param_2 + 0xc) - (int)pcVar4) + *(int *)(param_2 + 4) < 4) {
        FUN_0049cc70(4);
      }
      puVar5 = *(undefined4 **)(param_2 + 8);
      iVar6 = *piVar10;
      iVar2 = *(int *)(param_2 + 0xc);
      *puVar5 = uVar1;
      puVar5 = puVar5 + 1;
      uVar1 = *(undefined4 *)(iVar6 + 4);
      *(undefined4 **)(param_2 + 8) = puVar5;
      if ((iVar2 - (int)puVar5) + *(int *)(param_2 + 4) < 4) {
        FUN_0049cc70(4);
      }
      puVar5 = *(undefined4 **)(param_2 + 8);
      iVar6 = *piVar10;
      iVar2 = *(int *)(param_2 + 0xc);
      *puVar5 = uVar1;
      puVar5 = puVar5 + 1;
      uVar1 = *(undefined4 *)(iVar6 + 0xc);
      *(undefined4 **)(param_2 + 8) = puVar5;
      if ((iVar2 - (int)puVar5) + *(int *)(param_2 + 4) < 4) {
        FUN_0049cc70(4);
      }
      puVar5 = *(undefined4 **)(param_2 + 8);
      *puVar5 = uVar1;
      *(undefined4 **)(param_2 + 8) = puVar5 + 1;
    }
    piVar10 = piVar10 + 1;
    iStack_58 = iStack_58 + -1;
    if (iStack_58 == 0) {
      uVar1 = *(undefined4 *)(param_1 + 0xec);
      if ((*(int *)(param_2 + 0xc) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 4) < 4) {
        FUN_0049cc70(4);
      }
      puVar5 = *(undefined4 **)(param_2 + 8);
      *puVar5 = uVar1;
      *(undefined4 **)(param_2 + 8) = puVar5 + 1;
      return;
    }
  } while( true );
}


