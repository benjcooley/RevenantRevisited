// Decompiled methods and structure for class: cls_0x5a495c

/*
/OOAnalyzer/cls_0x5a495c
pack(disabled)
Structure cls_0x5a495c {
   0   cls_0x5a495c::vftable_5a495c *   4   vftptr_0x0   "pointer to cls_0x5a495c::vftable_5a495c"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 8 Alignment: 1

*/

// Function at 0044b660

cls_0x5a495c * __thiscall OOAnalyzer::cls_0x5a495c::~cls_0x5a495c(cls_0x5a495c *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5a495c__vftable_5a495c_005a495c;
  if (this->mbr_0x4 != 0) {
    this->mbr_0x4 = 0;
  }
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



