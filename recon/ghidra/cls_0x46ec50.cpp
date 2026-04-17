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
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
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
  lVar4 = __ftol();
  this->mbr_0xcc = (dword)lVar4;
  lVar5 = __ftol();
  this->mbr_0xd0 = (dword)lVar5;
  lVar6 = __ftol();
  uVar3 = (uint)lVar6 & 0xffff;
  this->mbr_0xc0 = this->mbr_0x10;
  this->mbr_0xd4 = uVar3;
  *in_stack_00000004 = (dword)lVar4;
  in_stack_00000004[1] = (dword)lVar5;
  in_stack_00000004[2] = uVar3;
  this->mbr_0xc4 = this->mbr_0x14;
  this->mbr_0xc8 = this->mbr_0x18;
  return;
}



