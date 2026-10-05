// FUN_0041cb40 @ 0041cb40 size=55

void __thiscall FUN_0041cb40(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *param_1;
  if (param_2 < iVar2) {
    iVar1 = param_1[4];
    if (*(int *)(iVar1 + param_2 * 4) != 0) {
      param_1[1] = param_1[1] + -1;
    }
    *(undefined4 *)(iVar1 + param_2 * 4) = 0;
    while ((0 < iVar2 && (*(int *)(iVar1 + -4 + *param_1 * 4) == 0))) {
      iVar2 = *param_1 + -1;
      *param_1 = iVar2;
    }
  }
  return;
}


