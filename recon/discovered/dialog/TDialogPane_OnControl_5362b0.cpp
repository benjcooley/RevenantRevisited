// FUN_005362b0 @ 005362b0 size=110

undefined4 __thiscall FUN_005362b0(int param_1,int param_2,int param_3)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  
  if ((param_3 == 3000) && (iVar3 = 0, 0 < *(int *)(param_1 + 0x1d8))) {
    puVar2 = (undefined4 *)(param_1 + 0x1b8);
    do {
      iVar1 = FUN_0059a530(param_2 + 0x18,*puVar2);
      if (iVar1 == 0) {
        *(int *)(param_1 + 0x1dc) = iVar3;
        *(undefined4 *)(param_1 + 0x1e4) = 1;
      }
      iVar3 = iVar3 + 1;
      puVar2 = puVar2 + 1;
    } while (iVar3 < *(int *)(param_1 + 0x1d8));
    return 1;
  }
  return 1;
}


