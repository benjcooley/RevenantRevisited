// Decompiled methods and structure for class: cls_0x45f7a0

/*
/OOAnalyzer/cls_0x45f7a0
pack(disabled)
Structure cls_0x45f7a0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 12 Alignment: 1

*/

// Function at 0045f7a0

undefined4 __thiscall OOAnalyzer::cls_0x45f7a0::meth_0x45f7a0(cls_0x45f7a0 *this)

{
  if (((uint *)this->mbr_0x0 != (uint *)0x0) && (this->mbr_0x8 < *(uint *)this->mbr_0x0)) {
    return 1;
  }
  return 0;
}



// Function at 0045f7e0

void __thiscall OOAnalyzer::cls_0x45f7a0::meth_0x45f7e0(cls_0x45f7a0 *this)

{
  dword in_stack_00000004;
  
  this->mbr_0x8 = 0;
  this->mbr_0x0 = in_stack_00000004;
  this->mbr_0x4 = *(dword *)(in_stack_00000004 + 0x10);
  return;
}



