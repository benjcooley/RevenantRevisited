// Decompiled methods and structure for class: cls_0x429970

/*
/OOAnalyzer/cls_0x429970
pack(disabled)
Structure cls_0x429970 {
   0   word   2   mbr_0x0   "Unsigned Word (dw, 2-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 00429970

undefined4 __thiscall OOAnalyzer::cls_0x429970::meth_0x429970(cls_0x429970 *this)

{
  uint in_stack_00000004;
  
  if (in_stack_00000004 < (uint)(int)(short)this->mbr_0x0) {
    return *(undefined4 *)(in_stack_00000004 * 0x50 + 0x40 + this->mbr_0x4);
  }
  return 0;
}



