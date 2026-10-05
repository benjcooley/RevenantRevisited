// FUN_004d09b0 @ 004d09b0 size=104

int __thiscall
FUN_004d09b0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_0049d780(param_2);
  iVar2 = FUN_0049d7c0(param_2);
  if ((iVar1 != 0) && (iVar2 != 0)) {
    iVar1 = FUN_004d0610(iVar1,param_3,param_4,iVar2);
    if (iVar1 != 0) {
      FUN_00586f00(param_1,iVar2,param_3,param_4);
    }
    return iVar1;
  }
  return 0;
}


