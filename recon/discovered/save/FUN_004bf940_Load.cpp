// FUN_004bf940 @ 004bf940 size=64

void __thiscall FUN_004bf940(int *param_1,int param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  
  FUN_00472430(param_2,param_3,param_4);
  if (param_3 < 5) {
    iVar1 = *param_1;
    iVar2 = **(int **)(param_2 + 4);
    *(int **)(param_2 + 4) = *(int **)(param_2 + 4) + 1;
    (**(code **)(iVar1 + 0xe0))(s_Amount_005df378,iVar2 + 1);
  }
  return;
}


