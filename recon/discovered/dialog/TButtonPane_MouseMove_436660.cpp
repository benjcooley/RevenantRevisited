// FUN_00436660 @ 00436660 size=205

void __thiscall FUN_00436660(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int *piVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  if (*(int **)(param_1 + 0xa4) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0xa4) + 0x3c))(param_2,param_3,param_4);
    return;
  }
  bVar2 = false;
  if (0 < *(int *)(param_1 + 0x88)) {
    do {
      piVar1 = *(int **)(*(int *)(param_1 + 0x98) + iVar4 * 4);
      if (((((piVar1[5] & 2U) == 0) && (piVar1[2] != 0)) && ((piVar1[5] & 4U) == 0)) &&
         (iVar3 = (**(code **)(*piVar1 + 0x54))(param_3,param_4), iVar3 != 0)) {
        if ((*(byte *)(param_1 + 0x60) & 2) != 0) {
          if (piVar1 == (int *)0x0) {
            iVar3 = -1;
          }
          else {
            iVar3 = piVar1[3];
          }
          FUN_004369f0(iVar3);
          bVar2 = true;
        }
        (**(code **)(*piVar1 + 0x3c))(param_2,param_3,param_4);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(int *)(param_1 + 0x88));
    if (bVar2) {
      return;
    }
  }
  if (((*(uint *)(param_1 + 0x60) & 2) != 0) && ((*(uint *)(param_1 + 0x60) & 4) == 0)) {
    FUN_004369f0(0xffffffff);
  }
  return;
}


