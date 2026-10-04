// FUN_00427c80 @ 00427c80 size=70

undefined4 FUN_00427c80(undefined4 param_1,int param_2)

{
  int iVar1;
  
  if (*(int *)(param_2 + 0x10) == 8) {
    iVar1 = FUN_004d2a60(*(undefined4 *)(param_2 + 0x14));
    if (iVar1 != 0) {
      FUN_00479580();
      return 0;
    }
    FUN_0041ee50(s_Invalid_Attack__d_005cca20,*(undefined4 *)(param_2 + 0x14));
    FUN_00479580();
  }
  return 4;
}


