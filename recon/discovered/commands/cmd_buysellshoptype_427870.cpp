// FUN_00427870 @ 00427870 size=371

undefined4 FUN_00427870(void)

{
  int iVar1;
  
  iVar1 = FUN_00479700(&DAT_005cc8b0,0);
  if (iVar1 != 0) {
    FUN_00478a10();
    FUN_00479580();
    iVar1 = FUN_00479700(s_WEAPON_005cc8b4,0);
    if (iVar1 != 0) {
      DAT_0065a534 = 9;
      FUN_00478a10();
      return 0;
    }
    iVar1 = FUN_00479700(s_ARMOR_005cc8bc,0);
    if (iVar1 != 0) {
      DAT_0065a534 = 5;
      FUN_00478a10();
      return 0;
    }
    iVar1 = FUN_00479700(&DAT_005cc8c4,0);
    if (iVar1 != 0) {
      DAT_0065a534 = 0x11;
      FUN_00478a10();
      return 0;
    }
    FUN_0041ee50(s_Missing_Shop_Type_005cc8cc);
    return 4;
  }
  iVar1 = FUN_00479700(&DAT_005cc8e0,0);
  if (iVar1 == 0) {
    FUN_0041ee50(s_Buy_Sell_Option_005cc914);
    return 4;
  }
  FUN_00478a10();
  FUN_00479580();
  iVar1 = FUN_00479700(s_WEAPON_005cc8e8,0);
  if (iVar1 != 0) {
    DAT_0065a534 = 10;
    FUN_00478a10();
    return 0;
  }
  iVar1 = FUN_00479700(s_ARMOR_005cc8f0,0);
  if (iVar1 != 0) {
    DAT_0065a534 = 6;
    FUN_00478a10();
    return 0;
  }
  iVar1 = FUN_00479700(&DAT_005cc8f8,0);
  if (iVar1 != 0) {
    DAT_0065a534 = 0x12;
    FUN_00478a10();
    return 0;
  }
  FUN_0041ee50(s_Missing_Shop_Type_005cc900);
  return 4;
}


