// Decompiled methods and structure for class: cls_0x5a85ac

/*
/OOAnalyzer/cls_0x5a85ac
pack(disabled)
Structure cls_0x5a85ac {
   0   TScreen   276   TScreen   "Component (member) class."
   288   dword   4   mbr_0x120   "Unsigned Double-Word (ddw, 4-bytes)"
   304   dword   4   mbr_0x130   "Unsigned Double-Word (ddw, 4-bytes)"
   332   dword   4   mbr_0x14c   "Unsigned Double-Word (ddw, 4-bytes)"
   348   byte   1   mbr_0x15c   "Unsigned Byte (db)"
}
Length: 349 Alignment: 1

*/

// Function at 004de770

cls_0x5a85ac * __thiscall OOAnalyzer::cls_0x5a85ac::cls_0x5a85ac(cls_0x5a85ac *this,int param_1)

{
  uint uVar1;
  
  TScreen::TScreen(&this->TScreen);
  uVar1 = *(uint *)(param_1 + 4);
  (this->TScreen).vftptr_0x0 =
       (TScreen__vftable_5a50e8 *)&cls_0x5a85ac__vftable_5a85ac_005a85ac;
  (this->TScreen).mbr_0x8 = (this->TScreen).mbr_0x8 | uVar1 | 0x48001;
  *(undefined4 *)&(this->TScreen).field_0xe4 = 0;
  this->mbr_0x14c = 0;
  this->mbr_0x130 = 0;
  this->mbr_0x120 = 0;
  this->mbr_0x15c = 0;
  return this;
}



// Function at 004f4ae0

cls_0x5a85ac * __thiscall OOAnalyzer::cls_0x5a85ac::virt_meth_0x4f4ae0(cls_0x5a85ac *this)

{
  byte in_stack_00000004;
  
  TScreen::~TScreen(&this->TScreen);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



