// FUN_004361f0_DefWidget_DispatchInput @ 004361f0 size=333

void __thiscall FUN_004361f0_DefWidget_DispatchInput(int param_1,int param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  
  if ((((DAT_0065c9e0 != 0) && (param_3 != 0)) && (param_2 == 0x52)) &&
     (*(char *)(param_1 + 0xd0) != '\0')) {
    FUN_004377c0_DefWidget_LoadFile(param_1 + 0xd0);
  }
  if (*(int **)(param_1 + 0xa0) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0xa0) + 0x30))(param_2,param_3);
    return;
  }
  if (((*(byte *)(param_1 + 0x60) & 8) == 0) ||
     ((piVar3 = *(int **)(param_1 + 0x9c), piVar3 == (int *)0x0 &&
      (uVar1 = *(int *)(param_1 + 0x88) - 1,
      piVar3 = *(int **)(*(int *)(param_1 + 0x98) + (uVar1 & ((int)uVar1 < 0) - 1) * 4),
      piVar3 == (int *)0x0)))) goto LAB_004362e4;
  if (param_3 != 0) {
    if ((param_2 == 0x26) || (param_2 == 0x25)) {
      iVar4 = piVar3[3];
      do {
        iVar4 = iVar4 + -1;
        if (iVar4 < 0) {
          iVar4 = *(int *)(param_1 + 0x88) + -1;
        }
        iVar2 = FUN_004369f0(iVar4);
      } while ((iVar2 == 0) && (iVar4 != piVar3[3]));
      goto LAB_004362e4;
    }
    if ((param_2 == 0x28) || (param_2 == 0x27)) {
      iVar4 = piVar3[3];
      do {
        iVar4 = iVar4 + 1;
        if (*(int *)(param_1 + 0x88) <= iVar4) {
          iVar4 = 0;
        }
        iVar2 = FUN_004369f0(iVar4);
      } while ((iVar2 == 0) && (iVar4 != piVar3[3]));
      goto LAB_004362e4;
    }
  }
  if ((param_2 == 0xd) && (piVar3 != (int *)0x0)) {
    (**(code **)(*piVar3 + 0x2c))(param_3);
  }
LAB_004362e4:
  iVar4 = 0;
  if (0 < *(int *)(param_1 + 0x88)) {
    while( true ) {
      piVar3 = *(int **)(*(int *)(param_1 + 0x98) + iVar4 * 4);
      if (((((piVar3[5] & 2U) == 0) && (piVar3[2] != 0)) && ((piVar3[5] & 4U) == 0)) &&
         (iVar2 = (**(code **)(*piVar3 + 0x58))(param_2,param_3), iVar2 != 0)) break;
      iVar4 = iVar4 + 1;
      if (*(int *)(param_1 + 0x88) <= iVar4) {
        return;
      }
    }
    (**(code **)(*piVar3 + 0x2c))(param_3);
  }
  return;
}


