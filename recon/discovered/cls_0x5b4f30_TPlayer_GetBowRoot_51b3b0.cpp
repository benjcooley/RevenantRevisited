// FUN_0051b3b0_GetBowRoot @ 0051b3b0 size=64

undefined * __thiscall FUN_0051b3b0_GetBowRoot(int param_1,int *param_2)

{
  int iVar1;
  
  if (((param_2 != (int *)0x0) || (param_2 = *(int **)(param_1 + 700), param_2 != (int *)0x0)) &&
     ((short)param_2[1] == 0x17)) {
    iVar1 = (**(code **)(*param_2 + 0x1f0))();
    if (iVar1 == 7) {
      return &DAT_005e2934;
    }
    return &DAT_005e293c;
  }
  return &DAT_0066da54;
}


