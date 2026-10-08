// FUN_0048cb50 @ 0048cb50 size=169

int __thiscall FUN_0048cb50(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  
  switch(param_2) {
  case 1:
    iVar1 = 0;
    if (0 < *(int *)(param_1 + 0x2c)) {
      piVar2 = *(int **)(param_1 + 0x3c);
      piVar3 = piVar2;
      while (param_3 != *(int *)(*piVar3 + 0xc4)) {
        iVar1 = iVar1 + 1;
        piVar3 = piVar3 + 1;
        if (*(int *)(param_1 + 0x2c) <= iVar1) {
          return 0;
        }
      }
LAB_0048cbf0:
      return piVar2[iVar1];
    }
    break;
  case 2:
    iVar1 = 0;
    if (0 < *(int *)(param_1 + 0x40)) {
      piVar2 = *(int **)(param_1 + 0x50);
      piVar3 = piVar2;
      while (param_3 != *(int *)(*piVar3 + 0xc0)) {
        iVar1 = iVar1 + 1;
        piVar3 = piVar3 + 1;
        if (*(int *)(param_1 + 0x40) <= iVar1) {
          return 0;
        }
      }
      goto LAB_0048cbf0;
    }
    break;
  case 0xb:
  case 0xc:
    iVar1 = 0;
    if (0 < *(int *)(param_1 + 0x18)) {
      piVar2 = *(int **)(param_1 + 0x28);
      piVar3 = piVar2;
      do {
        if (param_3 == *(int *)(*piVar3 + 0xc0)) goto LAB_0048cbf0;
        iVar1 = iVar1 + 1;
        piVar3 = piVar3 + 1;
      } while (iVar1 < *(int *)(param_1 + 0x18));
    }
  }
  return 0;
}


