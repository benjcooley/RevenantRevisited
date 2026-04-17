// Decompiled methods and structure for class: cls_0x4a3060

/*
/OOAnalyzer/cls_0x4a3060
pack(disabled)
Structure cls_0x4a3060 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 28 Alignment: 1

*/

// Function at 004a3060

bool __thiscall OOAnalyzer::cls_0x4a3060::meth_0x4a3060(cls_0x4a3060 *this,int param_1)

{
  dword dVar1;
  int in_stack_00000008;
  
  if ((((param_1 < 0) || (in_stack_00000008 < 0)) || (dVar1 = this->mbr_0x0, (int)dVar1 <= param_1))
     || ((int)this->mbr_0x4 <= in_stack_00000008)) {
    return false;
  }
  if ((this->mbr_0x10 & 6) != 0) {
    return (uint)*(ushort *)((int)&this[2].mbr_0x10 + (dVar1 * in_stack_00000008 + param_1) * 2) !=
           this->mbr_0x18;
  }
  if ((this->mbr_0x10 & 1) != 0) {
    return (uint)*(byte *)((int)&this[2].mbr_0x10 + dVar1 * in_stack_00000008 + param_1) !=
           this->mbr_0x18;
  }
  return true;
}



