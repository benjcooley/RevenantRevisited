// FUN_00472110 @ 00472110 size=508

void FUN_00472110(int *param_1,int param_2)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 *puVar6;
  undefined4 *puVar7;
  int iVar8;
  int iVar9;
  short *psVar10;
  uint uVar11;
  
  FUN_0049cc70(0x400);
  if ((param_1 == (int *)0x0) || (((DAT_0065a250 & 1) != 0 && ((param_1[2] & 0x80000U) != 0)))) {
    if ((*(int *)(param_2 + 0xc) + *(int *)(param_2 + 4)) - *(int *)(param_2 + 8) < 2) {
      FUN_0049cc70(2);
    }
    puVar6 = *(undefined2 **)(param_2 + 8);
    *puVar6 = 0xffff;
    *(undefined2 **)(param_2 + 8) = puVar6 + 1;
  }
  else {
    uVar5 = (**(code **)(*param_1 + 0x15c))();
    if ((*(int *)(param_2 + 0xc) - *(int *)(param_2 + 8)) + *(int *)(param_2 + 4) < 2) {
      FUN_0049cc70(2);
    }
    puVar6 = *(undefined2 **)(param_2 + 8);
    iVar8 = *(int *)(param_2 + 0xc);
    iVar9 = *(int *)(param_2 + 4);
    *puVar6 = uVar5;
    iVar4 = param_1[1];
    puVar6 = puVar6 + 1;
    *(undefined2 **)(param_2 + 8) = puVar6;
    if ((iVar8 - (int)puVar6) + iVar9 < 2) {
      FUN_0049cc70(2);
    }
    puVar6 = *(undefined2 **)(param_2 + 8);
    iVar8 = param_1[0x13];
    iVar9 = *(int *)(param_2 + 0xc);
    *puVar6 = (short)iVar4;
    uVar2 = *(undefined4 *)(iVar8 + 0x1c);
    puVar6 = puVar6 + 1;
    *(undefined2 **)(param_2 + 8) = puVar6;
    if ((iVar9 - (int)puVar6) + *(int *)(param_2 + 4) < 4) {
      FUN_0049cc70(4);
    }
    puVar7 = *(undefined4 **)(param_2 + 8);
    iVar8 = *(int *)(param_2 + 0xc);
    iVar9 = *(int *)(param_2 + 4);
    *puVar7 = uVar2;
    puVar7 = puVar7 + 1;
    *(undefined4 **)(param_2 + 8) = puVar7;
    if ((iVar8 - (int)puVar7) + iVar9 < 2) {
      FUN_0049cc70(2);
    }
    puVar6 = *(undefined2 **)(param_2 + 8);
    *puVar6 = 0;
    *(undefined2 **)(param_2 + 8) = puVar6 + 1;
    FUN_004779d0(2);
    puVar6 = *(undefined2 **)(param_2 + 8);
    iVar8 = *(int *)(param_2 + 4);
    iVar9 = *param_1;
    *puVar6 = 0;
    puVar6 = puVar6 + 1;
    *(undefined2 **)(param_2 + 8) = puVar6;
    iVar8 = (int)puVar6 - iVar8;
    (**(code **)(iVar9 + 0x164))(param_2);
    uVar2 = *(undefined4 *)(param_2 + 8);
    uVar3 = *(undefined4 *)(param_2 + 4);
    iVar9 = (**(code **)(*param_1 + 0x170))();
    if (iVar9 == 0) {
      iVar9 = param_1[0x1a];
    }
    else {
      (**(code **)(*param_1 + 0x170))();
      iVar9 = FUN_00470040();
    }
    if ((0 < iVar9) && ((DAT_0065a250 & 2) == 0)) {
      iVar9 = (**(code **)(*param_1 + 0x170))();
      if (iVar9 == 0) {
        (**(code **)(*param_1 + 0x16c))(param_2);
      }
    }
    iVar9 = *(int *)(param_2 + 4);
    uVar1 = iVar8 - 4;
    uVar11 = *(int *)(param_2 + 8) - iVar9;
    if (uVar1 < *(uint *)(param_2 + 0xc)) {
      *(uint *)(param_2 + 8) = uVar1 + iVar9;
    }
    if ((int)((iVar9 - *(int *)(param_2 + 8)) + *(uint *)(param_2 + 0xc)) < 2) {
      FUN_0049cc70(2);
    }
    iVar9 = *(int *)(param_2 + 0xc);
    psVar10 = *(short **)(param_2 + 8) + 1;
    **(short **)(param_2 + 8) = (short)uVar11 - (short)iVar8;
    *(short **)(param_2 + 8) = psVar10;
    if ((iVar9 - (int)psVar10) + *(int *)(param_2 + 4) < 2) {
      FUN_0049cc70(2);
    }
    psVar10 = *(short **)(param_2 + 8);
    *psVar10 = (short)uVar11 - ((short)uVar2 - (short)uVar3);
    *(short **)(param_2 + 8) = psVar10 + 1;
    if (uVar11 < *(uint *)(param_2 + 0xc)) {
      *(uint *)(param_2 + 8) = uVar11 + *(int *)(param_2 + 4);
      return;
    }
  }
  return;
}


