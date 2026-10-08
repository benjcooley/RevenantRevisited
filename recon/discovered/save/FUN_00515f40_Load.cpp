// FUN_00515f40 @ 00515f40 size=83

void __thiscall FUN_00515f40(int *param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int unaff_retaddr;
  
  (**(code **)(*param_1 + 0x40))(param_1[2] & 0xfbffffff);
  FUN_00472430(unaff_retaddr,param_2,param_3);
  if (param_2 < 5) {
    iVar1 = **(int **)(unaff_retaddr + 4);
    *(int **)(unaff_retaddr + 4) = *(int **)(unaff_retaddr + 4) + 1;
    (**(code **)(*param_1 + 0xe0))(s_Amount_005e1da0,iVar1 + 1);
  }
  return;
}


