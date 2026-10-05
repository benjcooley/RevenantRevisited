// FUN_0048fda0 @ 0048fda0 size=349

void __fastcall FUN_0048fda0(int param_1)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  if ((DAT_0066829c != 0) && (0 < *(int *)(param_1 + 0x1c))) {
    if (DAT_00667fcc == 0) goto LAB_0048fe30;
    if (((0x77 < *(int *)(param_1 + 0x48) - DAT_00668504) &&
        ((*(uint *)(DAT_00667fcc + 8) & 0x2800080) == 0)) &&
       ((*(byte *)(DAT_00667fcc + 0x36c) & 2) == 0)) {
      FUN_0051d680(*(uint *)(DAT_00667fcc + 0x36c) | 2);
    }
  }
  if (((DAT_00667fcc != 0) && (DAT_006682bc != 0)) &&
     (((*(byte *)(DAT_00667fcc + 0x36c) & 2) != 0 &&
      ((*(uint *)(DAT_00667fcc + 0x110) & 0x100000) != 0)))) {
    FUN_0051d680(*(uint *)(DAT_00667fcc + 0x36c) & 0xfffffffd);
  }
LAB_0048fe30:
  iVar2 = *(int *)(param_1 + 0x1c);
  if ((iVar2 < 1) || ((*(byte *)(param_1 + 0x2c + iVar2 * 4) & 8) == 0)) {
    uVar3 = 0;
    if (0 < *(int *)(param_1 + 4)) {
      do {
        if ((((*(int *)(param_1 + 0x14) != 0) && (uVar3 < *(uint *)(param_1 + 4))) &&
            (*(int *)(*(int *)(param_1 + 0x14) + uVar3 * 4) != 0)) &&
           ((iVar2 = (**(code **)(**(int **)(*(int *)(param_1 + 0x14) + uVar3 * 4) + 0x3c))(),
            iVar2 == 0 &&
            (piVar1 = *(int **)(*(int *)(param_1 + 0x14) + uVar3 * 4), piVar1[0x11] == 0)))) {
          DAT_00666644 = piVar1;
          (**(code **)(*piVar1 + 0x24))(0);
          (**(code **)(**(int **)(*(int *)(param_1 + 0x14) + uVar3 * 4) + 0x4c))();
        }
        uVar3 = uVar3 + 1;
      } while ((int)uVar3 < *(int *)(param_1 + 4));
    }
  }
  else {
    piVar1 = *(int **)(*(int *)(param_1 + 0x14) + *(int *)(param_1 + 0x1c + iVar2 * 4) * 4);
    if (((piVar1 != (int *)0x0) && (iVar2 = (**(code **)(*piVar1 + 0x3c))(), iVar2 == 0)) &&
       (piVar1[0x11] == 0)) {
      DAT_00666644 = piVar1;
      (**(code **)(*piVar1 + 0x24))(0);
      (**(code **)(*piVar1 + 0x4c))();
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      return;
    }
  }
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  return;
}


