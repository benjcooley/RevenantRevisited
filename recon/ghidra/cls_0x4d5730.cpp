// Decompiled methods and structure for class: cls_0x4d5730

/*
/OOAnalyzer/cls_0x4d5730
pack(disabled)
Structure cls_0x4d5730 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   404   dword   4   mbr_0x194   "Unsigned Double-Word (ddw, 4-bytes)"
   408   dword   4   mbr_0x198   "Unsigned Double-Word (ddw, 4-bytes)"
   412   dword   4   mbr_0x19c   "Unsigned Double-Word (ddw, 4-bytes)"
   416   dword   4   mbr_0x1a0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 420 Alignment: 1

*/

// Function at 004d5730

void __thiscall OOAnalyzer::cls_0x4d5730::meth_0x4d5730(cls_0x4d5730 *this)

{
  int iVar1;
  dword in_stack_00000004;
  dword in_stack_00000008;
  dword in_stack_0000000c;
  
  iVar1 = (**(code **)(this->mbr_0x0 + 0x1c0))();
  if ((0 < iVar1) || (-1 < (int)in_stack_00000008)) {
    if (-1 < (int)in_stack_00000004) {
      this->mbr_0x194 = in_stack_00000004;
    }
    this->mbr_0x198 = in_stack_00000008;
    this->mbr_0x19c = in_stack_0000000c;
    if (0 < (int)in_stack_00000008) {
      this->mbr_0x1a0 = 0xffffffff;
      return;
    }
    this->mbr_0x1a0 = 1;
  }
  return;
}



