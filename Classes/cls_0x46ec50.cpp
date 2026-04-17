// Decompiled methods and structure for class: cls_0x46ec50

/*
/OOAnalyzer/cls_0x46ec50
pack(disabled)
Structure cls_0x46ec50 {
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   192   dword   4   mbr_0xc0   "Unsigned Double-Word (ddw, 4-bytes)"
   196   dword   4   mbr_0xc4   "Unsigned Double-Word (ddw, 4-bytes)"
   200   dword   4   mbr_0xc8   "Unsigned Double-Word (ddw, 4-bytes)"
   204   dword   4   mbr_0xcc   "Unsigned Double-Word (ddw, 4-bytes)"
   208   dword   4   mbr_0xd0   "Unsigned Double-Word (ddw, 4-bytes)"
   212   dword   4   mbr_0xd4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 216 Alignment: 1

*/

// Function at 0046ec50

void __thiscall OOAnalyzer::cls_0x46ec50::meth_0x46ec50(cls_0x46ec50 *this)

{
  dword dVar1;
  dword dVar2;
  uint uVar3;
  dword *in_stack_00000004;
  
  if (((this->mbr_0xc0 == this->mbr_0x10) && (this->mbr_0xc4 == this->mbr_0x14)) &&
     (this->mbr_0xc8 == this->mbr_0x18)) {
    dVar1 = this->mbr_0xd0;
    *in_stack_00000004 = this->mbr_0xcc;
    dVar2 = this->mbr_0xd4;
    in_stack_00000004[1] = dVar1;
    in_stack_00000004[2] = dVar2;
    return;
  }
  dVar1 = __ftol();
  this->mbr_0xcc = dVar1;
  dVar2 = __ftol();
  this->mbr_0xd0 = dVar2;
  uVar3 = __ftol();
  this->mbr_0xc0 = this->mbr_0x10;
  this->mbr_0xd4 = uVar3 & 0xffff;
  *in_stack_00000004 = dVar1;
  in_stack_00000004[1] = dVar2;
  in_stack_00000004[2] = uVar3 & 0xffff;
  this->mbr_0xc4 = this->mbr_0x14;
  this->mbr_0xc8 = this->mbr_0x18;
  return;
}



