// FUN_00428b20 @ 00428b20 size=100

undefined4 FUN_00428b20(undefined4 param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  if ((*(int *)(param_2 + 0x10) != 9) && (*(int *)(param_2 + 0x10) != 10)) {
    iVar1 = FUN_00479700(&DAT_005ccbcc,0);
    if (iVar1 == 0) {
      iVar2 = FUN_00479700(&DAT_005ccbd4,0);
      iVar1 = DAT_0065abcc;
      if (iVar2 == 0) {
        return 4;
      }
    }
    else {
      iVar1 = DAT_0065abcc / 2;
    }
    FUN_0049a610(iVar1);
    FUN_00479580();
    return 0;
  }
  return 4;
}


