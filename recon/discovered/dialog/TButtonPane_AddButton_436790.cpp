// FUN_00436790 @ 00436790 size=53

int __thiscall FUN_00436790(undefined4 param_1,int *param_2)

{
  int iVar1;
  
  (**(code **)(*param_2 + 4))(param_1);
  iVar1 = FUN_0041c840(param_2);
  param_2[3] = iVar1;
  (**(code **)(*param_2 + 0x1c))(param_2[5] | 0x20);
  return iVar1;
}


