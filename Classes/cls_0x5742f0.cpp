// Decompiled methods and structure for class: cls_0x5742f0

/*
/OOAnalyzer/cls_0x5742f0
pack(disabled)
Structure cls_0x5742f0 {
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 60 Alignment: 1

*/

// Function at 005742f0

void __thiscall OOAnalyzer::cls_0x5742f0::meth_0x5742f0(cls_0x5742f0 *this)

{
  if ((HGLOBAL)this->mbr_0x38 != (HGLOBAL)0x0) {
    GlobalFree((HGLOBAL)this->mbr_0x38);
    this->mbr_0x38 = 0;
  }
  return;
}



