// FUN_0046fd30 @ 0046fd30 size=174

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __thiscall FUN_0046fd30(int *param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  int iVar2;
  undefined4 unaff_EBP;
  int iVar3;
  undefined4 unaff_retaddr;
  
  iVar3 = 0;
  if (param_4 < 1) {
    return 0;
  }
  iVar1 = (**(code **)(*param_1 + 0xa8))(param_3);
  if (iVar1 != 0) {
    while( true ) {
      iVar2 = (**(code **)(*param_1 + 0x6c))(unaff_retaddr,iVar1,param_4);
      param_4 = param_4 - iVar2;
      iVar3 = iVar3 + iVar2;
      if (param_4 < 1) break;
      iVar1 = (**(code **)(*param_1 + 0xa8))(unaff_EBP);
      if (iVar1 == 0) {
        return iVar3;
      }
    }
    if (iVar1 != 0) {
      if (*(int *)(iVar1 + 100) == DAT_0065d674) {
        _DAT_0065d548 = 1;
        (**(code **)(DAT_0065d4f8 + 0x90))();
      }
      if (*(int *)(iVar1 + 100) == DAT_0065b088) {
        _DAT_0065b078 = 1;
      }
    }
  }
  return iVar3;
}


