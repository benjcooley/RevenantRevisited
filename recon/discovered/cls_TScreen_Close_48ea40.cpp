// FUN_0048ea40 @ 0048ea40 size=89

void __fastcall FUN_0048ea40(int *param_1)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  
  iVar2 = 0;
  if (0 < param_1[7]) {
    piVar3 = param_1 + 8;
    do {
      piVar1 = *(int **)(param_1[5] + *piVar3 * 4);
      if (piVar1 != (int *)0x0) {
        piVar1[0x17] = 0;
        (**(code **)(*piVar1 + 8))();
      }
      iVar2 = iVar2 + 1;
      piVar3 = piVar3 + 1;
    } while (iVar2 < param_1[7]);
  }
  param_1[0x15] = 1;
  if ((int *)param_1[0x11] != (int *)0x0) {
    (**(code **)(*(int *)param_1[0x11] + 0x2c))();
  }
  (**(code **)(*param_1 + 0x40))(0x100,0);
  return;
}


