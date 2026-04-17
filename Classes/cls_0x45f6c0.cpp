// Decompiled methods and structure for class: cls_0x45f6c0

/*
/OOAnalyzer/cls_0x45f6c0
pack(disabled)
Structure cls_0x45f6c0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 12 Alignment: 1

*/

// Function at 0045f6c0

cls_0x45f6c0 * __thiscall OOAnalyzer::cls_0x45f6c0::cls_0x45f6c0(cls_0x45f6c0 *this)

{
  dword dVar1;
  dword *in_stack_00000004;
  
  this->mbr_0x0 = *in_stack_00000004;
  dVar1 = in_stack_00000004[2];
  this->mbr_0x4 = in_stack_00000004[1];
  this->mbr_0x8 = dVar1;
  return this;
}



