// Decompiled methods and structure for class: cls_0x57bc10

/*
/OOAnalyzer/cls_0x57bc10
pack(disabled)
Structure cls_0x57bc10 {
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 32 Alignment: 1

*/

// Function at 0057bc10

undefined4 __thiscall OOAnalyzer::cls_0x57bc10::meth_0x57bc10(cls_0x57bc10 *this)

{
  if ((int)this->mbr_0x1c < 0) {
    return 0;
  }
  return *(undefined4 *)(this->mbr_0x14 + this->mbr_0x1c * 4);
}



