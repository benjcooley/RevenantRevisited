// Decompiled methods and structure for class: cls_0x45f6f0

/*
/OOAnalyzer/cls_0x45f6f0
pack(disabled)
Structure cls_0x45f6f0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 0045f6f0

undefined4 __thiscall OOAnalyzer::cls_0x45f6f0::meth_0x45f6f0(cls_0x45f6f0 *this)

{
  int *in_stack_00000004;
  
  if ((((*in_stack_00000004 <= (int)this->mbr_0x0) && (in_stack_00000004[1] <= (int)this->mbr_0x4))
      && ((int)this->mbr_0x8 <= in_stack_00000004[2])) &&
     ((int)this->mbr_0xc <= in_stack_00000004[3])) {
    return 1;
  }
  return 0;
}



