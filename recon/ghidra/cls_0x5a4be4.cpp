// Decompiled methods and structure for class: cls_0x5a4be4

/*
/OOAnalyzer/cls_0x5a4be4
pack(disabled)
Structure cls_0x5a4be4 {
   0   TObjectInstance   112   TObjectInstance   "Component (member) class."
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 132 Alignment: 1

*/

// Function at 0046cef0

cls_0x5a4be4 * __thiscall OOAnalyzer::cls_0x5a4be4::cls_0x5a4be4(cls_0x5a4be4 *this)

{
  TObjectInstance::TObjectInstance(&this->TObjectInstance);
  this->mbr_0x70 = (dword)&cls_0x5a4c30__vftable_5a4c30_005a4c30;
  this->mbr_0x78 = 0;
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  this->mbr_0x74 = 0;
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&cls_0x5a4be4__vftable_5a4be4_005a4be4;
  return this;
}



// Function at 0046cff0

cls_0x5a4be4 * __thiscall OOAnalyzer::cls_0x5a4be4::virt_meth_0x46cff0(cls_0x5a4be4 *this)

{
  byte in_stack_00000004;
  
  TObjectInstance::~TObjectInstance(&this->TObjectInstance);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



