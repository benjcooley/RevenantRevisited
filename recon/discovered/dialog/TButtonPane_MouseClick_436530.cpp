// FUN_00436530 @ 00436530 size=297

void __thiscall FUN_00436530(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  if (*(int **)(param_1 + 0xa8) != (int *)0x0) {
    iVar2 = (**(code **)(**(int **)(param_1 + 0xa8) + 0x54))(param_3,param_4);
    if (iVar2 == 0) {
      if ((*(int *)(param_1 + 0xac) == 0) || (((param_2 != 1 && (param_2 != 3)) && (param_2 != 2))))
      {
        if (*(int *)(param_1 + 0xa4) != *(int *)(param_1 + 0xa8)) {
          return;
        }
      }
      else {
        (**(code **)(**(int **)(param_1 + 0xa8) + 0x5c))(*(int **)(param_1 + 0xa8),0x65);
        *(undefined4 *)(param_1 + 0xa8) = 0;
      }
    }
    if (*(int **)(param_1 + 0xa8) != (int *)0x0) {
      (**(code **)(**(int **)(param_1 + 0xa8) + 0x38))(param_2,param_3,param_4);
      return;
    }
  }
  if (*(int **)(param_1 + 0xa4) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0xa4) + 0x38))(param_2,param_3,param_4);
    return;
  }
  iVar2 = 0;
  if (0 < *(int *)(param_1 + 0x88)) {
    while( true ) {
      piVar1 = *(int **)(*(int *)(param_1 + 0x98) + iVar2 * 4);
      if ((((piVar1[5] & 2U) == 0) && (piVar1[2] != 0)) &&
         (((piVar1[5] & 4U) == 0 &&
          (iVar3 = (**(code **)(*piVar1 + 0x54))(param_3,param_4), iVar3 != 0)))) break;
      iVar2 = iVar2 + 1;
      if (*(int *)(param_1 + 0x88) <= iVar2) {
        return;
      }
    }
    if (param_2 == 1) {
      if (piVar1 == (int *)0x0) {
        iVar2 = -1;
      }
      else {
        iVar2 = piVar1[3];
      }
      FUN_004369f0(iVar2);
    }
    (**(code **)(*piVar1 + 0x38))(param_2,param_3,param_4);
  }
  return;
}


