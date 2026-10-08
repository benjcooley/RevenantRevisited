// FUN_004369f0 @ 004369f0 size=236

undefined4 __thiscall FUN_004369f0(int param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  if ((param_2 < 0) || (*(int *)(param_1 + 0x88) <= param_2)) {
    piVar1 = *(int **)(param_1 + 0x9c);
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 0x1c))(piVar1[5] & 0xfffffff7);
    }
    *(undefined4 *)(param_1 + 0x9c) = 0;
  }
  else {
    piVar1 = *(int **)(*(int *)(param_1 + 0x98) + param_2 * 4);
    if (*(int **)(param_1 + 0x9c) == piVar1) {
      return 1;
    }
    iVar2 = FUN_0049c430(s_frontend_move_005cd0e0);
    if (-1 < iVar2) {
      iVar3 = FUN_0049b650(iVar2);
      if (iVar3 != 0) {
        FUN_0049b990(iVar2,0x7f,1,0,0x50,700);
      }
    }
    (**(code **)(*piVar1 + 0x1c))(piVar1[5] | 8);
    if ((*(byte *)(*(int *)(*(int *)(param_1 + 0x98) + param_2 * 4) + 0x14) & 8) != 0) {
      piVar1 = *(int **)(param_1 + 0x9c);
      if (piVar1 != (int *)0x0) {
        (**(code **)(*piVar1 + 0x1c))(piVar1[5] & 0xfffffff7);
      }
      *(undefined4 *)(param_1 + 0x9c) = *(undefined4 *)(*(int *)(param_1 + 0x98) + param_2 * 4);
      return 1;
    }
  }
  return 0;
}


