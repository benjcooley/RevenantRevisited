// FUN_004dbb80 @ 004dbb80 size=228

void __thiscall FUN_004dbb80(int param_1,int param_2)

{
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  undefined1 *puVar4;
  byte *pbVar5;
  byte bVar6;
  int iVar7;
  uint uVar8;
  char *pcVar9;
  undefined1 *puVar10;
  
  if ((*(int *)(param_2 + 4) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 0xc) < 1) {
    FUN_0049cc70(1);
  }
  puVar10 = *(undefined1 **)(param_2 + 8);
  *puVar10 = 0;
  *(undefined1 **)(param_2 + 8) = puVar10 + 1;
  FUN_00472980(param_2);
  uVar2 = **(undefined1 **)(param_1 + 0xe0);
  if ((*(int *)(param_2 + 4) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 0xc) < 1) {
    FUN_0049cc70(1);
  }
  puVar10 = *(undefined1 **)(param_2 + 8);
  iVar3 = *(int *)(param_1 + 0xe0);
  iVar7 = -1;
  *puVar10 = uVar2;
  puVar10 = puVar10 + 1;
  *(undefined1 **)(param_2 + 8) = puVar10;
  pcVar9 = (char *)(iVar3 + 4);
  do {
    if (iVar7 == 0) break;
    iVar7 = iVar7 + -1;
    cVar1 = *pcVar9;
    pcVar9 = pcVar9 + 1;
  } while (cVar1 != '\0');
  bVar6 = ~(byte)iVar7 - 1;
  if ((*(int *)(param_2 + 4) - (int)puVar10) + *(int *)(param_2 + 0xc) < 1) {
    FUN_0049cc70(1);
  }
  pbVar5 = *(byte **)(param_2 + 8);
  puVar10 = (undefined1 *)(*(int *)(param_1 + 0xe0) + 4);
  *pbVar5 = bVar6;
  pbVar5 = pbVar5 + 1;
  *(byte **)(param_2 + 8) = pbVar5;
  if (bVar6 != 0) {
    uVar8 = (uint)bVar6;
    do {
      uVar2 = *puVar10;
      if ((*(int *)(param_2 + 4) - (int)pbVar5) + *(int *)(param_2 + 0xc) < 1) {
        FUN_0049cc70(1);
      }
      puVar4 = *(undefined1 **)(param_2 + 8);
      *puVar4 = uVar2;
      pbVar5 = puVar4 + 1;
      puVar10 = puVar10 + 1;
      uVar8 = uVar8 - 1;
      *(byte **)(param_2 + 8) = pbVar5;
    } while (uVar8 != 0);
  }
  return;
}


