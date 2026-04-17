// Decompiled methods and structure for class: cls_0x41c720

/*
/OOAnalyzer/cls_0x41c720
pack(disabled)
Structure cls_0x41c720 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 0041c720

undefined4 __thiscall OOAnalyzer::cls_0x41c720::meth_0x41c720(cls_0x41c720 *this,int param_1)

{
  int in_stack_00000008;
  
  if (((((int)this->mbr_0x0 <= param_1) && ((int)this->mbr_0x4 <= in_stack_00000008)) &&
      (param_1 <= (int)this->mbr_0x8)) && (in_stack_00000008 <= (int)this->mbr_0xc)) {
    return 1;
  }
  return 0;
}



