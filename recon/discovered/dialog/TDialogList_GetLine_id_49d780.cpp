// FUN_0049d780 @ 0049d780 size=55

char * __thiscall FUN_0049d780(int *param_1,uint param_2)

{
  int iVar1;
  
  iVar1 = *param_1;
  if ((uint)(param_1[5] + iVar1) <= param_2) {
    return s__badid__005dab4c;
  }
  if ((int)param_2 < iVar1) {
    return *(char **)(*(int *)(param_1[4] + param_2 * 4) + 4);
  }
  return *(char **)(*(int *)(param_1[9] + (param_2 - iVar1) * 4) + 4);
}


