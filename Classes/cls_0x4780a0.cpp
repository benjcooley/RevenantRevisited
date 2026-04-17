// Decompiled methods and structure for class: cls_0x4780a0

/*
/OOAnalyzer/cls_0x4780a0
pack(disabled)
Structure cls_0x4780a0 {
   0   word   2   mbr_0x0   "Unsigned Word (dw, 2-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 004780a0

int __thiscall OOAnalyzer::cls_0x4780a0::meth_0x4780a0(cls_0x4780a0 *this)

{
  uint in_stack_00000004;
  
  if (in_stack_00000004 < (uint)(int)(short)this->mbr_0x0) {
    return in_stack_00000004 * 0x50 + this->mbr_0x4;
  }
  return 0;
}



