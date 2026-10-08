// FUN_004d0a20 @ 004d0a20 size=124

int __thiscall FUN_004d0a20(undefined4 param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  if (param_2 == 0) {
    return 0;
  }
  iVar1 = FUN_0049d6d0(param_2);
  if (iVar1 < 0) {
    return 0;
  }
  iVar1 = FUN_0049d780(iVar1);
  if (iVar1 == 0) {
    return 0;
  }
  iVar1 = FUN_004d0610(iVar1,param_3,param_4,param_2);
  if (iVar1 != 0) {
    FUN_00586f00(param_1,param_2,param_3,param_4);
  }
  return iVar1;
}


