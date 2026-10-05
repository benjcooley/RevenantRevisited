// FUN_0041c840 @ 0041c840 size=205

int __thiscall FUN_0041c840(int *param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int *piVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  bool bVar10;
  
  if (param_1[2] <= *param_1) {
    puVar3 = (undefined4 *)FUN_00482fb0((param_1[3] + param_1[2]) * 4);
    uVar1 = param_1[2];
    puVar8 = (undefined4 *)param_1[4];
    puVar9 = puVar3;
    for (uVar5 = uVar1 & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
      *puVar9 = *puVar8;
      puVar8 = puVar8 + 1;
      puVar9 = puVar9 + 1;
    }
    for (iVar6 = 0; iVar6 != 0; iVar6 = iVar6 + -1) {
      *(undefined1 *)puVar9 = *(undefined1 *)puVar8;
      puVar8 = (undefined4 *)((int)puVar8 + 1);
      puVar9 = (undefined4 *)((int)puVar9 + 1);
    }
    puVar8 = puVar3 + uVar1;
    for (uVar5 = param_1[3] & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
      *puVar8 = 0;
      puVar8 = puVar8 + 1;
    }
    for (iVar6 = 0; iVar6 != 0; iVar6 = iVar6 + -1) {
      *(undefined1 *)puVar8 = 0;
      puVar8 = (undefined4 *)((int)puVar8 + 1);
    }
    FUN_004830f0(param_1[4]);
    param_1[4] = (int)puVar3;
    param_1[2] = param_1[2] + param_1[3];
  }
  if (param_1[1] < *param_1) {
    iVar6 = *param_1;
    piVar7 = (int *)param_1[4];
    iVar4 = 0;
    if (-1 < iVar6) {
      bVar10 = SBORROW4(0,iVar6);
      iVar6 = -iVar6;
      do {
        if ((bVar10 == iVar6 < 0) || (*piVar7 == 0)) {
          *piVar7 = param_2;
          if (iVar4 < *param_1) {
            return iVar4;
          }
          *param_1 = iVar4 + 1;
          param_1[1] = iVar4 + 1;
          return iVar4;
        }
        iVar2 = *param_1;
        iVar4 = iVar4 + 1;
        piVar7 = piVar7 + 1;
        bVar10 = SBORROW4(iVar4,iVar2);
        iVar6 = iVar4 - iVar2;
      } while (iVar4 <= iVar2);
    }
    return -1;
  }
  *(int *)(param_1[4] + *param_1 * 4) = param_2;
  iVar6 = *param_1;
  iVar4 = iVar6 + 1;
  *param_1 = iVar4;
  param_1[1] = iVar4;
  return iVar6;
}


