// FUN_0042d0d0 @ 0042d0d0 size=132

int __thiscall FUN_0042d0d0(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  
  if (((*(uint *)(param_1 + 0x14) & 2) == 0) && (*(int *)(param_1 + 8) != 0)) {
    iVar1 = *(int *)(param_1 + 0x60);
    if ((param_2 < iVar1) ||
       (((param_3 < *(int *)(param_1 + 100) || (*(int *)(param_1 + 0x68) + iVar1 <= param_2)) ||
        (*(int *)(param_1 + 0x6c) + *(int *)(param_1 + 100) <= param_3)))) {
      iVar2 = 0;
    }
    else {
      iVar2 = 1;
    }
    if ((((*(uint *)(param_1 + 0x14) & 0x20000) != 0) && (iVar2 != 0)) &&
       ((*(int *)(param_1 + 0x9c) != 0 && (*(int *)(param_1 + 0xac) != 0)))) {
      iVar2 = FUN_004a3060(param_2 - iVar1,param_3 - *(int *)(param_1 + 100));
    }
    return iVar2;
  }
  return 0;
}


