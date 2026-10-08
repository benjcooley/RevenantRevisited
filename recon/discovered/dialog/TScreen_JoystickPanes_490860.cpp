// FUN_00490860 @ 00490860 size=254

void __thiscall FUN_00490860(int param_1,undefined4 param_2,undefined4 param_3)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  
  if (DAT_006682bc == 0) {
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    iVar2 = *(int *)(param_1 + 0x1c);
    if ((0 < iVar2) && ((*(byte *)(param_1 + 0x2c + iVar2 * 4) & 4) != 0)) {
      piVar1 = *(int **)(*(int *)(param_1 + 0x14) + *(int *)(param_1 + 0x1c + iVar2 * 4) * 4);
      if ((piVar1 != (int *)0x0) &&
         ((iVar2 = (**(code **)(*piVar1 + 0x3c))(), iVar2 == 0 && (piVar1[0x11] == 0)))) {
        DAT_00666644 = piVar1;
        (**(code **)(*piVar1 + 0x74))(param_2,param_3);
      }
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      return;
    }
    uVar3 = 0;
    if (0 < *(int *)(param_1 + 4)) {
      do {
        if ((((*(int *)(param_1 + 0x14) != 0) && (uVar3 < *(uint *)(param_1 + 4))) &&
            (*(int *)(*(int *)(param_1 + 0x14) + uVar3 * 4) != 0)) &&
           (((iVar2 = (**(code **)(**(int **)(*(int *)(param_1 + 0x14) + uVar3 * 4) + 0x3c))(),
             iVar2 == 0 &&
             (piVar1 = *(int **)(*(int *)(param_1 + 0x14) + uVar3 * 4), piVar1[0x11] == 0)) &&
            (iVar2 = (**(code **)(*piVar1 + 0x44))(), iVar2 == 0)))) {
          DAT_00666644 = *(int **)(*(int *)(param_1 + 0x14) + uVar3 * 4);
          (**(code **)(*DAT_00666644 + 0x24))(0);
          (**(code **)(**(int **)(*(int *)(param_1 + 0x14) + uVar3 * 4) + 0x74))(param_2,param_3);
        }
        uVar3 = uVar3 + 1;
      } while ((int)uVar3 < *(int *)(param_1 + 4));
    }
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  }
  return;
}


