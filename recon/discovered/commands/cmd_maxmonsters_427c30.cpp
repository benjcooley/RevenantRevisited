// FUN_00427c30 @ 00427c30 size=71

undefined4 FUN_00427c30(int param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = FUN_0047a410(param_2,&DAT_005cca18,&param_2);
  if (iVar1 == 0) {
    FUN_0041ee50(&DAT_005cca1c,*(undefined4 *)(param_1 + 0xec));
    return 0;
  }
  *(undefined4 *)(param_1 + 0xec) = param_2;
  return 0;
}


