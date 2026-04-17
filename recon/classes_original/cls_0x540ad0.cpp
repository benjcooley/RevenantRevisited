// Decompiled methods and structure for class: cls_0x540ad0

/*
/OOAnalyzer/cls_0x540ad0
pack(disabled)
Structure cls_0x540ad0 {
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 00540ad0

dword __thiscall OOAnalyzer::cls_0x540ad0::meth_0x540ad0(cls_0x540ad0 *this)

{
  dword dVar1;
  int in_stack_00000004;
  
  dVar1 = *(dword *)(this->mbr_0x10 + in_stack_00000004 * 4);
  if (dVar1 == 0) {
    dVar1 = this->mbr_0x14;
  }
  return dVar1;
}



