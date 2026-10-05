// FUN_00521c60 @ 00521c60 size=20

int __thiscall FUN_00521c60(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = *(int *)(*(int *)(param_1 + 0x10) + param_2 * 4);
  if (iVar1 == 0) {
    iVar1 = *(int *)(param_1 + 0x14);
  }
  return iVar1;
}


