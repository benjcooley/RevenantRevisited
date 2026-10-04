// FUN_0048ed90 @ 0048ed90 size=120

int __thiscall FUN_0048ed90(int param_1,int param_2,int param_3)

{
  int iVar1;
  
  if (*(int *)(param_2 + 0x40) == 0) {
    FUN_00481c10(s_Attempted_to_add_an_uninitialize_005d9d8c,0);
  }
  iVar1 = 0;
  if (0 < *(int *)(param_1 + 4)) {
    do {
      if (*(int *)(*(int *)(param_1 + 0x14) + iVar1 * 4) == param_2) {
        if (param_3 < 0) {
          return iVar1;
        }
        FUN_0041cb40(iVar1);
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < *(int *)(param_1 + 4));
  }
  if (param_3 < 0) {
    param_3 = *(int *)(param_1 + 4);
  }
  iVar1 = FUN_0041c910(param_2,param_3);
  *(int *)(param_2 + 0x3c) = param_1;
  *(undefined4 *)(param_2 + 0x44) = 1;
  return iVar1;
}


