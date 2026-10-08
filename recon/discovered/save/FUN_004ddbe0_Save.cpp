// FUN_004ddbe0 @ 004ddbe0 size=83

void __thiscall FUN_004ddbe0(int param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < *(int *)(param_1 + 0x68)) {
    do {
      if ((-1 < iVar2) &&
         (puVar1 = *(undefined4 **)(*(int *)(param_1 + 0x78) + iVar2 * 4),
         puVar1 != (undefined4 *)0x0)) {
        (**(code **)*puVar1)(1);
      }
      FUN_0041cb40(iVar2);
      iVar2 = iVar2 + 1;
    } while (iVar2 < *(int *)(param_1 + 0x68));
  }
  *(int *)(param_1 + 0x68) = 0;
  *(undefined4 *)(param_1 + 0x6c) = 0;
  FUN_00472980(param_2);
  return;
}


