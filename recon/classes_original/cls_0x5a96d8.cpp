// Decompiled methods and structure for class: cls_0x5a96d8

/*
/OOAnalyzer/cls_0x5a96d8
pack(disabled)
Structure cls_0x5a96d8 {
   0   cls_0x5a96d8::vftable_5a96d8 *   4   vftptr_0x0   "pointer to cls_0x5a96d8::vftable_5a96d8"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 40 Alignment: 1

*/

// Function at 004f6e50

cls_0x5a96d8 * __thiscall OOAnalyzer::cls_0x5a96d8::~cls_0x5a96d8(cls_0x5a96d8 *this)

{
  cls_0x5a96d8 *pcVar1;
  
  this->vftptr_0x0 = &cls_0x5a96d8__vftable_5a96d8_005a96d8;
  pcVar1 = (cls_0x5a96d8 *)FUN_004830f0(this->mbr_0x4);
  return pcVar1;
}



// Function at 004f6e70

cls_0x5a96d8 * __thiscall OOAnalyzer::cls_0x5a96d8::~cls_0x5a96d8(cls_0x5a96d8 *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0(this->mbr_0x4);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



