// Decompiled methods and structure for class: cls_0x41c7b0

/*
/OOAnalyzer/cls_0x41c7b0
pack(disabled)
Structure cls_0x41c7b0 {
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 0041c7b0

void __thiscall OOAnalyzer::cls_0x41c7b0::meth_0x41c7b0(cls_0x41c7b0 *this)

{
  dword in_stack_00000004;
  
  if (0 < (int)in_stack_00000004) {
    in_stack_00000004 = -in_stack_00000004;
  }
  if (this->mbr_0x10 == 0) {
    this->mbr_0x10 = in_stack_00000004;
    return;
  }
  this->mbr_0x14 = in_stack_00000004;
  return;
}



