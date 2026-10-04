// FUN_00427cd0 @ 00427cd0 size=85

undefined4 FUN_00427cd0(undefined4 param_1,int param_2)

{
  int iVar1;
  
  iVar1 = FUN_00451d70(*(undefined4 *)(param_2 + 0x28),1,2);
  if (iVar1 != 0) {
    iVar1 = FUN_004d3b90(iVar1,3);
    if (iVar1 != 0) {
      FUN_00479580();
      return 0;
    }
    FUN_0041ee50(s_Invalid_Target__s_005cca34,*(undefined4 *)(param_2 + 0x28));
    FUN_00479580();
  }
  return 4;
}


