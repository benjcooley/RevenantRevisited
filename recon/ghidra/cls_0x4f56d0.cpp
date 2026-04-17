// Decompiled methods and structure for class: cls_0x4f56d0

/*
/OOAnalyzer/cls_0x4f56d0
pack(disabled)
Structure cls_0x4f56d0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 12 Alignment: 1

*/

// Function at 004f56d0

void __thiscall OOAnalyzer::cls_0x4f56d0::meth_0x4f56d0(cls_0x4f56d0 *this,uint param_1)

{
  int in_stack_00000008;
  
  if (in_stack_00000008 != 0) {
    (**(code **)(this->mbr_0x0 + 0x40))(this->mbr_0x8 | param_1);
    return;
  }
  (**(code **)(this->mbr_0x0 + 0x40))(this->mbr_0x8 & ~param_1);
  return;
}



