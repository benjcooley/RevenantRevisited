// Decompiled methods and structure for class: cls_0x588020

/*
/OOAnalyzer/cls_0x588020
pack(disabled)
Structure cls_0x588020 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 00588020

void __thiscall OOAnalyzer::cls_0x588020::meth_0x588020(cls_0x588020 *this)

{
  FUN_004830f0(this->mbr_0x0);
  return;
}



// Function at 00588030

int __thiscall OOAnalyzer::cls_0x588020::meth_0x588030(cls_0x588020 *this)

{
  return (int)(this->mbr_0x4 - this->mbr_0x0) >> 3;
}



// Function at 00588040

int __thiscall OOAnalyzer::cls_0x588020::meth_0x588040(cls_0x588020 *this)

{
  int in_stack_00000004;
  
  return this->mbr_0x0 + in_stack_00000004 * 8;
}



