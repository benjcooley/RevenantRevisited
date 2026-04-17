// Decompiled methods and structure for class: cls_0x570760

/*
/OOAnalyzer/cls_0x570760
pack(disabled)
Structure cls_0x570760 {
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 80 Alignment: 1

*/

// Function at 00570760

int __thiscall OOAnalyzer::cls_0x570760::meth_0x570760(cls_0x570760 *this)

{
  int in_stack_00000004;
  
  if (((int)this->mbr_0x44 <= in_stack_00000004) && (in_stack_00000004 <= (int)this->mbr_0x48)) {
    return this->mbr_0x4c + (in_stack_00000004 % 0x32) * 0x68;
  }
  return 0;
}



