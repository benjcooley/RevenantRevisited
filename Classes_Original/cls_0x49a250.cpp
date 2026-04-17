// Decompiled methods and structure for class: cls_0x49a250

/*
/OOAnalyzer/cls_0x49a250
pack(disabled)
Structure cls_0x49a250 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 0049a250

undefined4 __thiscall OOAnalyzer::cls_0x49a250::meth_0x49a250(cls_0x49a250 *this)

{
  uint in_stack_00000004;
  
  if (in_stack_00000004 < this->mbr_0xc) {
    this->mbr_0x8 = this->mbr_0x4 + in_stack_00000004;
    return 1;
  }
  return 0;
}



