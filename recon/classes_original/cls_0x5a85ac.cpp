// Decompiled methods and structure for class: cls_0x5a85ac

/*
/OOAnalyzer/cls_0x5a85ac
pack(disabled)
Structure cls_0x5a85ac {
   0   cls_0x5a50e8   276   cls_0x5a50e8   "Component (member) class."
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
  
  cls_0x5a50e8::cls_0x5a50e8(&this->cls_0x5a50e8);
  uVar1 = *(uint *)(param_1 + 4);
  (this->cls_0x5a50e8).vftptr_0x0 =
       (cls_0x5a50e8__vftable_5a50e8 *)&cls_0x5a85ac__vftable_5a85ac_005a85ac;
  (this->cls_0x5a50e8).mbr_0x8 = (this->cls_0x5a50e8).mbr_0x8 | uVar1 | 0x48001;
  *(undefined4 *)&(this->cls_0x5a50e8).field_0xe4 = 0;
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
  
  cls_0x5a50e8::~cls_0x5a50e8(&this->cls_0x5a50e8);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



