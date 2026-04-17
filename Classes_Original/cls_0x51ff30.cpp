// Decompiled methods and structure for class: cls_0x51ff30

/*
/OOAnalyzer/cls_0x51ff30
pack(disabled)
Structure cls_0x51ff30 {
   0   word   2   mbr_0x0   "Unsigned Word (dw, 2-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 0051ff30

int __thiscall OOAnalyzer::cls_0x51ff30::meth_0x51ff30(cls_0x51ff30 *this)

{
  uint in_stack_00000004;
  
  if (in_stack_00000004 < (uint)(int)(short)this->mbr_0x0) {
    return in_stack_00000004 * 0x50 + this->mbr_0x4;
  }
  return 0x6554b4;
}



