// Decompiled methods and structure for class: cls_0x535e90

/*
/OOAnalyzer/cls_0x535e90
pack(disabled)
Structure cls_0x535e90 {
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   440   dword   4   mbr_0x1b8   "Unsigned Double-Word (ddw, 4-bytes)"
   472   dword   4   mbr_0x1d8   "Unsigned Double-Word (ddw, 4-bytes)"
   488   dword   4   mbr_0x1e8   "Unsigned Double-Word (ddw, 4-bytes)"
   492   dword   4   mbr_0x1ec   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 496 Alignment: 1

*/

// Function at 00535e90

undefined4 __thiscall OOAnalyzer::cls_0x535e90::meth_0x535e90(cls_0x535e90 *this)

{
  int iVar1;
  cls_0x419dd0 cStack_38;
  dword dStack_34;
  dword *pdStack_30;
  dword *pdStack_2c;
  dword dStack_28;
  cls_0x429950 local_18;
  cls_0x419dd0 *local_14;
  cls_0x429950 local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1a9a;
  local_c = ExceptionList;
  if ((0 < (int)this->mbr_0x1d8) && (DAT_00667fcc != 0)) {
    dStack_28 = this->mbr_0x1e8;
    ExceptionList = &local_c;
    this->mbr_0x1ec = DAT_0065d0d0;
    pdStack_2c = (dword *)0x535ef4;
    cls_0x5a5320_TPlayScreen::meth_0x47c580((cls_0x5a5320_TPlayScreen *)&DAT_0065caf0);
    dStack_28 = 0x160;
    pdStack_2c = (dword *)0x535efe;
    iVar1 = FUN_00482fb0();
    local_4 = 0;
    if (iVar1 == 0) {
      dStack_28 = 0;
    }
    else {
      dStack_34 = this->mbr_0x1d8;
      pdStack_2c = &this->mbr_0x1b8;
      dStack_28 = 0xffffffff;
      pdStack_30 = &this->mbr_0x198;
      cStack_38.mbr_0x0 = 0xff;
      cls_0x429950::cls_0x429950(&local_18,0xff,0xff);
      local_14 = &cStack_38;
      cls_0x419dd0::cls_0x419dd0(&cStack_38);
      cls_0x429950::cls_0x429950(&local_10,0x3c,0xaf);
      local_14 = (cls_0x419dd0 *)&stack0xffffffc4;
      cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffffc4);
      dStack_28 = FUN_00533f10(this,DAT_00667fcc,3,0xffffd8f0,0xffffd8f0,0xffffd8f0,0xffffd8f0);
    }
    local_4 = 0xffffffff;
    this->mbr_0x190 = dStack_28;
    pdStack_2c = (dword *)0x535fb8;
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x17c);
    ExceptionList = local_c;
    return 1;
  }
  return 0;
}



