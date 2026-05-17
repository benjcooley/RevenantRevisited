// FUN_0052c780_TBottomBarPane_Initialize @ 0052c780 size=59

undefined4 __fastcall FUN_0052c780_TBottomBarPane_Initialize(int param_1)

{
  undefined4 uVar1;
  
  FUN_00434e40();
  uVar1 = FUN_0046d710(s_UtilityBar_005e37a0);
  *(undefined4 *)(param_1 + 0x180) = uVar1;
  uVar1 = FUN_0046d710(s_BarEndCap_005e37ac);
  *(undefined4 *)(param_1 + 0x184) = uVar1;
  return 1;
}


