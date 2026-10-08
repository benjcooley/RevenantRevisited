// FUN_0042ca50 @ 0042ca50 size=243

void __thiscall FUN_0042ca50(int *param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int *piVar5;
  
  uVar1 = param_1[5];
  if (((uVar1 ^ param_2) & 0x70000) != 0) {
    param_1[0x25] = 0;
    param_2 = param_2 | 0x20;
  }
  if (((param_2 & 0x10) == 0) || (uVar4 = param_2, (param_2 & 6) != 0)) {
    uVar4 = param_2 & 0xfffffff7;
  }
  if (((uVar4 ^ param_1[5]) & 0xe) != 0) {
    uVar4 = uVar4 | 0x20;
  }
  param_1[5] = uVar4;
  if ((((uVar1 ^ param_2) & 0x10000) != 0) && ((uVar4 & 0x40000) != 0)) {
    if (((param_2 & 0x10000) != 0) &&
       (((0 < param_1[0x1d] && (iVar2 = param_1[2], iVar2 != 0)) &&
        (iVar3 = 0, 0 < *(int *)(iVar2 + 0x88))))) {
      do {
        if ((iVar3 < 0) || (*(int *)(iVar2 + 0x88) <= iVar3)) {
          piVar5 = (int *)0x0;
        }
        else {
          piVar5 = *(int **)(*(int *)(iVar2 + 0x98) + iVar3 * 4);
        }
        if (((piVar5 != param_1) && (piVar5[4] == 4)) && (piVar5[0x1d] == param_1[0x1d])) {
          (**(code **)(*piVar5 + 0x1c))(piVar5[5] & 0xfffeffff);
          (**(code **)(*piVar5 + 0x1c))(piVar5[5] | 0x20);
        }
        iVar2 = param_1[2];
        iVar3 = iVar3 + 1;
      } while (iVar3 < *(int *)(iVar2 + 0x88));
    }
    FUN_0042a820(0xbba - (uint)((param_2 & 0x10000) != 0));
  }
  return;
}


