// Decompiled methods and structure for class: cls_0x49a210

/*
/OOAnalyzer/cls_0x49a210
pack(disabled)
Structure cls_0x49a210 {
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 16 Alignment: 1

*/

// Function at 0049a1d0

cls_0x49a210 * __thiscall OOAnalyzer::cls_0x49a210::meth_0x49a1d0(cls_0x49a210 *this)

{
  undefined4 *puVar1;
  undefined4 in_stack_00000004;
  
  if ((int)((this->mbr_0xc + this->mbr_0x4) - this->mbr_0x8) < 4) {
    cls_0x5a5ff0::meth_0x49cc70((cls_0x5a5ff0 *)this);
  }
  puVar1 = (undefined4 *)this->mbr_0x8;
  *puVar1 = in_stack_00000004;
  this->mbr_0x8 = (dword)(puVar1 + 1);
  return this;
}



// Function at 0049a210

cls_0x49a210 * __thiscall OOAnalyzer::cls_0x49a210::cls_0x49a210(cls_0x49a210 *this)

{
  undefined4 *puVar1;
  undefined4 in_stack_00000004;
  
  if ((int)((this->mbr_0xc + this->mbr_0x4) - this->mbr_0x8) < 4) {
    cls_0x5a5ff0::meth_0x49cc70((cls_0x5a5ff0 *)this);
  }
  puVar1 = (undefined4 *)this->mbr_0x8;
  *puVar1 = in_stack_00000004;
  this->mbr_0x8 = (dword)(puVar1 + 1);
  return this;
}



