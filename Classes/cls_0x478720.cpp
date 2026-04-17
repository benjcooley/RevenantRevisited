// Decompiled methods and structure for class: cls_0x478720

/*
/OOAnalyzer/cls_0x478720
pack(disabled)
Structure cls_0x478720 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 00478720

cls_0x478720 * __thiscall OOAnalyzer::cls_0x478720::cls_0x478720(cls_0x478720 *this)

{
  this->mbr_0x4 = 0;
  this->mbr_0x0 = 0;
  return this;
}



// Function at 00478730

void __thiscall OOAnalyzer::cls_0x478720::meth_0x478730(cls_0x478720 *this)

{
  if (this->mbr_0x4 != 0) {
    FUN_004830f0(this->mbr_0x4);
  }
  return;
}



