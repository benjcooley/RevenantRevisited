// Decompiled methods and structure for class: cls_0x4c00b0

/*
/OOAnalyzer/cls_0x4c00b0
pack(disabled)
Structure cls_0x4c00b0 {
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 220 Alignment: 1

*/

// Function at 004c00b0

void __thiscall OOAnalyzer::cls_0x4c00b0::meth_0x4c00b0(cls_0x4c00b0 *this)

{
  int in_stack_00000004;
  
  if (in_stack_00000004 != 0) {
    this->mbr_0xd8 = *(dword *)(in_stack_00000004 + 0x40);
    return;
  }
  this->mbr_0xd8 = 0xffffffff;
  return;
}



