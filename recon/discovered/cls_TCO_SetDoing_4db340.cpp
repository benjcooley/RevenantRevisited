// FUN_004db340_TComplexObject_SetDoing @ 004db340 size=94

void __thiscall FUN_004db340_TComplexObject_SetDoing(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0xd8);
  if (param_2 != iVar1) {
    if ((((iVar1 != 0) && (iVar1 != *(int *)(param_1 + 0xe0))) &&
        (iVar1 != *(int *)(param_1 + 0xdc))) && (iVar1 != 0)) {
      if (*(int *)(iVar1 + 0x5c) != 0) {
        FUN_00482f80(*(int *)(iVar1 + 0x5c));
      }
      FUN_004830f0(iVar1);
    }
    *(int *)(param_1 + 0xd8) = param_2;
  }
  *(uint *)(*(int *)(param_1 + 0xe0) + 0x60) =
       *(uint *)(*(int *)(param_1 + 0xe0) + 0x60) & 0xffffffdf;
  return;
}


