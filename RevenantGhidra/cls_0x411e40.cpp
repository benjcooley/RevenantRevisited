// Decompiled methods and structure for class: cls_0x411e40

/*
/OOAnalyzer/cls_0x411e40
pack(disabled)
Structure cls_0x411e40 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 88 Alignment: 1

*/

// Function at 00411e40

undefined4 __thiscall OOAnalyzer::cls_0x411e40::meth_0x411e40(cls_0x411e40 *this)

{
  if (this->mbr_0x54 != 0) {
    if (DAT_005d7a28 != 0) {
      FUN_0056d120((int)&this->mbr_0x4);
      this->mbr_0x54 = 0;
      return 1;
    }
    (**(code **)(*DAT_006699d0 + 0x38))(DAT_006699d0,this->mbr_0x0);
    this->mbr_0x54 = 0;
  }
  return 1;
}



