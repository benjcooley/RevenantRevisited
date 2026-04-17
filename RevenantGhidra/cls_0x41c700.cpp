// Decompiled methods and structure for class: cls_0x41c700

/*
/OOAnalyzer/cls_0x41c700
pack(disabled)
Structure cls_0x41c700 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 0041c700

void __thiscall OOAnalyzer::cls_0x41c700::meth_0x41c700(cls_0x41c700 *this)

{
  dword dVar1;
  dword *in_stack_00000004;
  
  this->mbr_0x0 = *in_stack_00000004;
  this->mbr_0x4 = in_stack_00000004[1];
  dVar1 = in_stack_00000004[3];
  this->mbr_0x8 = in_stack_00000004[2];
  this->mbr_0xc = dVar1;
  return;
}



