// Decompiled methods and structure for class: cls_0x587e30

/*
/OOAnalyzer/cls_0x587e30
pack(disabled)
Structure cls_0x587e30 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 12 Alignment: 1

*/

// Function at 00587e30

undefined4 __thiscall OOAnalyzer::cls_0x587e30::meth_0x587e30(cls_0x587e30 *this)

{
  uint in_stack_00000004;
  
  if (in_stack_00000004 <= this->mbr_0x8) {
    this->mbr_0x4 = this->mbr_0x0 + in_stack_00000004;
    return 1;
  }
  return 0;
}



