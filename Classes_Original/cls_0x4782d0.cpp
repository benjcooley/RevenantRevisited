// Decompiled methods and structure for class: cls_0x4782d0

/*
/OOAnalyzer/cls_0x4782d0
pack(disabled)
Structure cls_0x4782d0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 12 Alignment: 1

*/

// Function at 004782d0

undefined4 __thiscall OOAnalyzer::cls_0x4782d0::meth_0x4782d0(cls_0x4782d0 *this)

{
  dword dVar1;
  
  this->mbr_0x4 = this->mbr_0x4 + 0x50;
  dVar1 = this->mbr_0x8 + 1;
  this->mbr_0x8 = dVar1;
  if (((short *)this->mbr_0x0 != (short *)0x0) && ((int)dVar1 < (int)*(short *)this->mbr_0x0)) {
    return 1;
  }
  return 0;
}



// Function at 00478300

undefined4 __thiscall OOAnalyzer::cls_0x4782d0::meth_0x478300(cls_0x4782d0 *this)

{
  if (((short *)this->mbr_0x0 != (short *)0x0) &&
     (this->mbr_0x8 < (uint)(int)*(short *)this->mbr_0x0)) {
    return 1;
  }
  return 0;
}



// Function at 004785c0

void __thiscall OOAnalyzer::cls_0x4782d0::meth_0x4785c0(cls_0x4782d0 *this)

{
  dword in_stack_00000004;
  
  this->mbr_0x8 = 0;
  this->mbr_0x0 = in_stack_00000004;
  this->mbr_0x4 = *(dword *)(in_stack_00000004 + 4);
  return;
}



