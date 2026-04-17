// Decompiled methods and structure for class: cls_0x539230

/*
/OOAnalyzer/cls_0x539230
pack(disabled)
Structure cls_0x539230 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 00539230

void __thiscall OOAnalyzer::cls_0x539230::meth_0x539230(cls_0x539230 *this)

{
  int in_stack_00000004;
  
  if (in_stack_00000004 != 0) {
    (**(code **)(this->mbr_0x0 + 0x1c))(this->mbr_0x14 | 4);
    return;
  }
  (**(code **)(this->mbr_0x0 + 0x1c))(this->mbr_0x14 & 0xfffffffb);
  return;
}



