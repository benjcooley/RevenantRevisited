// Decompiled methods and structure for class: cls_0x5a4d20

/*
/OOAnalyzer/cls_0x5a4d20
pack(disabled)
Structure cls_0x5a4d20 {
   0   TObjectInstance   112   TObjectInstance   "Component (member) class."
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 132 Alignment: 1

*/

// Function at 0046d100

cls_0x5a4d20 * __thiscall OOAnalyzer::cls_0x5a4d20::cls_0x5a4d20(cls_0x5a4d20 *this)

{
  TObjectInstance::TObjectInstance(&this->TObjectInstance);
  this->mbr_0x70 = (dword)&cls_0x5a4c30__vftable_5a4c30_005a4c30;
  this->mbr_0x78 = 0;
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  this->mbr_0x74 = 0;
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&cls_0x5a4d20__vftable_5a4d20_005a4d20;
  return this;
}



// Function at 0046d130

cls_0x5a4d20 * __thiscall OOAnalyzer::cls_0x5a4d20::virt_meth_0x46d130(cls_0x5a4d20 *this)

{
  byte in_stack_00000004;
  
  TObjectInstance::~TObjectInstance(&this->TObjectInstance);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



