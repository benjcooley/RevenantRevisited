// FUN_0046dfb0 @ 0046dfb0 size=311

undefined4 __fastcall FUN_0046dfb0(byte *param_1)

{
  short sVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  int *piVar5;
  int iVar6;
  
  if (*(int *)(param_1 + 8) == 0) {
    return 0;
  }
  piVar5 = *(int **)(param_1 + 0x18);
  if ((piVar5 != (int *)0x0) && ((*param_1 & 1) != 0)) {
    iVar3 = (**(code **)(*piVar5 + 0x170))();
    if (iVar3 == 0) {
      iVar3 = piVar5[0x1a];
    }
    else {
      (**(code **)(*piVar5 + 0x170))();
      iVar3 = FUN_00470040();
    }
    if ((0 < iVar3) &&
       (((*param_1 & 2) != 0 ||
        (iVar3 = (**(code **)(**(int **)(param_1 + 0x18) + 0x170))(), iVar3 == 0)))) {
      iVar3 = (**(code **)(**(int **)(param_1 + 0x18) + 0x170))();
      piVar2 = *(int **)(param_1 + 0x18);
      if (iVar3 == 0) {
        *(int **)(param_1 + 8) = piVar2;
      }
      else {
        iVar3 = *piVar2;
        *(int **)(param_1 + 0x10) = piVar2;
        uVar4 = (**(code **)(iVar3 + 0x170))();
        *(undefined4 *)(param_1 + 0xc) = uVar4;
        *(undefined4 *)(param_1 + 8) = uVar4;
      }
      param_1[0x14] = 0;
      param_1[0x15] = 0;
      param_1[0x16] = 0;
      param_1[0x17] = 0;
      param_1[0x18] = 0;
      param_1[0x19] = 0;
      param_1[0x1a] = 0;
      param_1[0x1b] = 0;
    }
  }
  do {
    piVar2 = *(int **)(param_1 + 8);
    iVar3 = (**(code **)(*piVar2 + 0x170))();
    if (iVar3 == 0) {
      iVar3 = piVar2[0x1a];
    }
    else {
      (**(code **)(*piVar2 + 0x170))();
      iVar3 = FUN_00470040();
    }
    iVar6 = *(int *)(param_1 + 0x14);
    if (iVar6 < iVar3) {
      piVar2 = *(int **)(param_1 + 8);
      *(int *)(param_1 + 0x14) = iVar6 + 1;
      iVar3 = (**(code **)(*piVar2 + 0x170))();
      if (iVar3 == 0) {
        uVar4 = *(undefined4 *)(piVar2[0x1e] + iVar6 * 4);
      }
      else {
        (**(code **)(*piVar2 + 0x170))(iVar6);
        uVar4 = FUN_00470110(iVar6);
      }
      *(undefined4 *)(param_1 + 0x18) = uVar4;
    }
    else {
      if ((piVar5 == (int *)0x0) || (*(int *)(param_1 + 8) == *(int *)(param_1 + 4))) {
        param_1[0x18] = 0;
        param_1[0x19] = 0;
        param_1[0x1a] = 0;
        param_1[0x1b] = 0;
        goto LAB_0046e0df;
      }
      piVar5 = (int *)piVar5[0x19];
      if (piVar5 == *(int **)(param_1 + 0xc)) {
        piVar5 = *(int **)(param_1 + 0x10);
      }
      sVar1 = *(short *)((int)piVar5 + 0x7e);
      *(int *)(param_1 + 8) = piVar5[0x19];
      *(int *)(param_1 + 0x14) = sVar1 + 1;
      param_1[0x18] = 0;
      param_1[0x19] = 0;
      param_1[0x1a] = 0;
      param_1[0x1b] = 0;
    }
    if (*(int *)(param_1 + 0x18) != 0) {
LAB_0046e0df:
      return *(undefined4 *)(param_1 + 0x18);
    }
  } while( true );
}


