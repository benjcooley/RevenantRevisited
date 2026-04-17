// Decompiled methods and structure for class: cls_0x4782a0

/*
/OOAnalyzer/cls_0x4782a0
pack(disabled)
Structure cls_0x4782a0 {
   0   word   2   mbr_0x0   "Unsigned Word (dw, 2-bytes)"
   2   word   2   mbr_0x2   "Unsigned Word (dw, 2-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 004782a0

void __thiscall OOAnalyzer::cls_0x4782a0::meth_0x4782a0(cls_0x4782a0 *this)

{
  if (this->mbr_0x4 != 0) {
    FUN_004830f0(this->mbr_0x4);
  }
  this->mbr_0x4 = 0;
  this->mbr_0x0 = 0;
  this->mbr_0x2 = 0;
  return;
}



