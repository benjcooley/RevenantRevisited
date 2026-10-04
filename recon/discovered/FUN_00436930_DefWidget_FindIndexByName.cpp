// FUN_00436930 @ 00436930 size=75

int __thiscall FUN_00436930(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < *(int *)(param_1 + 0x88)) {
    do {
      iVar1 = FUN_0059a530(*(int *)(*(int *)(param_1 + 0x98) + iVar2 * 4) + 0x18,param_2);
      if (iVar1 == 0) {
        return iVar2;
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < *(int *)(param_1 + 0x88));
  }
  return -1;
}


