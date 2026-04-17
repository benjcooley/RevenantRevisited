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
  cls_0x5b98b8 **ppcVar1;
  cls_0x5b98b8 *extraout_ECX;
  cls_0x5b98b8 *extraout_ECX_00;
  cls_0x419dd0 cVar2;
  cls_0x419dd0 cVar3;
  cls_0x5b98b8 *pcVar4;
  dword *pdVar5;
  dword *pdVar6;
  cls_0x5b98b8 *pcVar7;
  cls_0x429950 local_18;
  undefined *local_14;
  cls_0x429950 local_10;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1a9a;
  local_c = ExceptionList;
  if ((0 < (int)this->mbr_0x1d8) && (DAT_00667fcc != (uint **)0x0)) {
    ExceptionList = &local_c;
    this->mbr_0x1ec = DAT_0065d0d0;
    TCharacter::meth_0x47c580((TCharacter *)&DAT_0065caf0);
    ppcVar1 = (cls_0x5b98b8 **)FUN_00482fb0(0x160);
    local_4 = 0;
    if (ppcVar1 == (cls_0x5b98b8 **)0x0) {
      ppcVar1 = (cls_0x5b98b8 **)0x0;
    }
    else {
      pcVar4 = (cls_0x5b98b8 *)this->mbr_0x1d8;
      pdVar6 = &this->mbr_0x1b8;
      pcVar7 = (cls_0x5b98b8 *)0xffffffff;
      pdVar5 = &this->mbr_0x198;
      cls_0x429950::cls_0x429950(&local_18,0xff,0xff);
      local_14 = &stack0xffffffc8;
      cVar3.mbr_0x0 = (dword)extraout_ECX;
      cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffffc8);
      cls_0x429950::cls_0x429950(&local_10,0x3c,0xaf);
      local_14 = &stack0xffffffc4;
      cVar2.mbr_0x0 = (dword)extraout_ECX_00;
      cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffffc4);
      ppcVar1 = FUN_00533f10(ppcVar1,(cls_0x5b98b8 *)this,DAT_00667fcc,(cls_0x5b98b8 *)0x3,
                             (cls_0x5b98b8 *)0xffffd8f0,(cls_0x5b98b8 *)0xffffd8f0,
                             (cls_0x5b98b8 *)0xffffd8f0,(cls_0x5b98b8 *)0xffffd8f0,
                             (cls_0x5b98b8 *)cVar2.mbr_0x0,(cls_0x5b98b8 *)cVar3.mbr_0x0,pcVar4,
                             (uint **)pdVar5,pdVar6,pcVar7);
    }
    local_4 = 0xffffffff;
    this->mbr_0x190 = (dword)ppcVar1;
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x17c);
    ExceptionList = local_c;
    return 1;
  }
  return 0;
}



