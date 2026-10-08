// FUN_00422bf0 @ 00422bf0 size=49

undefined4 FUN_00422bf0(int param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = FUN_0047a410(param_2,&DAT_005cb5ac,&param_2);
  if (iVar1 == 0) {
    return 4;
  }
  *(undefined2 *)(param_1 + 0x5c) = (undefined2)param_2;
  return 0;
}


