// FUN_0048ff00 @ 0048ff00 size=289

void __fastcall FUN_0048ff00(int param_1)

{
  int *piVar1;
  undefined *puVar2;
  int iVar3;
  uint uVar4;
  
  if (DAT_006682bc == 0) {
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    iVar3 = *(int *)(param_1 + 0x1c);
    if ((0 < iVar3) && ((*(byte *)(param_1 + 0x2c + iVar3 * 4) & 0x10) != 0)) {
      piVar1 = *(int **)(*(int *)(param_1 + 0x14) + *(int *)(param_1 + 0x1c + iVar3 * 4) * 4);
      if (piVar1 != (int *)0x0) {
        iVar3 = (**(code **)(*piVar1 + 0x3c))();
        if ((iVar3 == 0) && (piVar1[0x11] == 0)) {
          DAT_00666644 = piVar1;
          if (*(int *)(param_1 + 0x50) != 0) {
            (**(code **)(*piVar1 + 0x28))();
          }
          (**(code **)(*piVar1 + 0x24))(0);
          (**(code **)(*piVar1 + 0x50))();
          (**(code **)(*piVar1 + 0x2c))(0);
        }
      }
      puVar2 = PTR_DAT_005d79e0;
      *(undefined4 *)(param_1 + 0x50) = 0;
      (**(code **)(*(int *)puVar2 + 0x24))();
      return;
    }
    uVar4 = 0;
    if (0 < *(int *)(param_1 + 4)) {
      do {
        if (((*(int *)(param_1 + 0x14) != 0) && (uVar4 < *(uint *)(param_1 + 4))) &&
           (*(int *)(*(int *)(param_1 + 0x14) + uVar4 * 4) != 0)) {
          iVar3 = (**(code **)(**(int **)(*(int *)(param_1 + 0x14) + uVar4 * 4) + 0x3c))();
          if ((iVar3 == 0) &&
             (piVar1 = *(int **)(*(int *)(param_1 + 0x14) + uVar4 * 4), piVar1[0x11] == 0)) {
            if (*(int *)(param_1 + 0x50) != 0) {
              (**(code **)(*piVar1 + 0x28))();
            }
            DAT_00666644 = *(int **)(*(int *)(param_1 + 0x14) + uVar4 * 4);
            (**(code **)(*DAT_00666644 + 0x24))(0);
            (**(code **)(**(int **)(*(int *)(param_1 + 0x14) + uVar4 * 4) + 0x50))();
            (**(code **)(**(int **)(*(int *)(param_1 + 0x14) + uVar4 * 4) + 0x2c))(0);
          }
        }
        uVar4 = uVar4 + 1;
      } while ((int)uVar4 < *(int *)(param_1 + 4));
    }
    puVar2 = PTR_DAT_005d79e0;
    *(undefined4 *)(param_1 + 0x50) = 0;
    (**(code **)(*(int *)puVar2 + 0x24))();
  }
  return;
}


