// FUN_00437000_DefWidget_ParsePanel @ 00437000 size=123

undefined4 __thiscall FUN_00437000_DefWidget_ParsePanel(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_0047a410(param_2,s_BEGIN_005cda78);
  if (iVar1 == 0) {
    uVar2 = (**(code **)(*param_1 + 0x98))(s_Panel_layout_BEGIN_expected_005cda80,0);
    return uVar2;
  }
  iVar1 = FUN_00479700(&DAT_005cda9c,0);
  while( true ) {
    if (iVar1 != 0) {
      FUN_004795a0();
      return 1;
    }
    iVar1 = (**(code **)(*param_1 + 0xac))(param_2);
    if (iVar1 == 0) break;
    iVar1 = FUN_00479700(&DAT_005cda9c,0);
  }
  return 0;
}


