// Decompiled methods and structure for class: cls_0x51d3e0

/*
/OOAnalyzer/cls_0x51d3e0
pack(disabled)
Structure cls_0x51d3e0 {
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   844   word   2   mbr_0x34c   "Unsigned Word (dw, 2-bytes)"
   848   dword   4   mbr_0x350   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 852 Alignment: 1

*/

// Function at 0051d3e0

undefined4 __thiscall OOAnalyzer::cls_0x51d3e0::meth_0x51d3e0(cls_0x51d3e0 *this)

{
  int iVar1;
  
  iVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(this->mbr_0x48 + 0x1c));
  if (iVar1 < (short)this->mbr_0x34c) {
    return *(undefined4 *)(this->mbr_0x350 + 4 + iVar1 * 8);
  }
  return 0;
}



