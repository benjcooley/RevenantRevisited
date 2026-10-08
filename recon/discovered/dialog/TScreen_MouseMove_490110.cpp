// FUN_00490110 @ 00490110 size=198

void __fastcall FUN_00490110(int param_1)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  
  if (DAT_006682bc == 0) {
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    iVar2 = *(int *)(param_1 + 0x1c);
    if ((0 < iVar2) && ((*(byte *)(param_1 + 0x2c + iVar2 * 4) & 0x40) != 0)) {
      piVar1 = *(int **)(*(int *)(param_1 + 0x14) + *(int *)(param_1 + 0x1c + iVar2 * 4) * 4);
      if (piVar1 != (int *)0x0) {
        iVar2 = (**(code **)(*piVar1 + 0x3c))();
        if ((iVar2 == 0) && (piVar1[0x11] == 0)) {
          DAT_00666644 = piVar1;
          (**(code **)(*piVar1 + 0x1c))();
        }
      }
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      return;
    }
    uVar3 = 0;
    if (0 < *(int *)(param_1 + 4)) {
      do {
        if (((*(int *)(param_1 + 0x14) != 0) && (uVar3 < *(uint *)(param_1 + 4))) &&
           (*(int *)(*(int *)(param_1 + 0x14) + uVar3 * 4) != 0)) {
          iVar2 = (**(code **)(**(int **)(*(int *)(param_1 + 0x14) + uVar3 * 4) + 0x3c))();
          if ((iVar2 == 0) &&
             (piVar1 = *(int **)(*(int *)(param_1 + 0x14) + uVar3 * 4), piVar1[0x11] == 0)) {
            DAT_00666644 = piVar1;
            (**(code **)(*piVar1 + 0x1c))();
          }
        }
        uVar3 = uVar3 + 1;
      } while ((int)uVar3 < *(int *)(param_1 + 4));
    }
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  }
  return;
}


