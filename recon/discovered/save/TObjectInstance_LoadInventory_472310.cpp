// FUN_00472310 @ 00472310 size=112

void __thiscall FUN_00472310(undefined4 param_1,int param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  
  iVar1 = param_3;
  if (2 < param_3) {
    param_3 = **(int **)(param_2 + 4);
    *(int **)(param_2 + 4) = *(int **)(param_2 + 4) + 1;
    if ((param_3 < 0x801) && (0 < param_3)) {
      do {
        iVar3 = FUN_00471ce0(param_2,iVar1,param_4);
        if (iVar3 != 0) {
          uVar2 = FUN_0041c840(iVar3);
          *(undefined2 *)(iVar3 + 0x7e) = uVar2;
          *(undefined4 *)(iVar3 + 100) = param_1;
        }
        param_3 = param_3 + -1;
      } while (param_3 != 0);
    }
  }
  return;
}


