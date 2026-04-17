// Decompiled methods and structure for class: cls_0x56fb40

/*
/OOAnalyzer/cls_0x56fb40
pack(disabled)
Structure cls_0x56fb40 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 12 Alignment: 1

*/

// Function at 0056fb40

int __thiscall OOAnalyzer::cls_0x56fb40::meth_0x56fb40(cls_0x56fb40 *this)

{
  if (this->mbr_0x4 == 0) {
    return 0;
  }
  return (int)(this->mbr_0x8 - this->mbr_0x4) >> 2;
}



