// Decompiled methods and structure for class: cls_0x5a4f74

/*
/OOAnalyzer/cls_0x5a4f74
pack(disabled)
Structure cls_0x5a4f74 {
   0   cls_0x5a4f74::vftable_5a4f74 *   4   vftptr_0x0   "pointer to cls_0x5a4f74::vftable_5a4f74"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   380   dword   4   mbr_0x17c   "Unsigned Double-Word (ddw, 4-bytes)"
   411   byte   1   mbr_0x19b   "Unsigned Byte (db)"
   412   dword   4   mbr_0x19c   "Unsigned Double-Word (ddw, 4-bytes)"
   475   byte   1   mbr_0x1db   "Unsigned Byte (db)"
   476   dword   4   mbr_0x1dc   "Unsigned Double-Word (ddw, 4-bytes)"
   603   byte   1   mbr_0x25b   "Unsigned Byte (db)"
   604   dword   4   mbr_0x25c   "Unsigned Double-Word (ddw, 4-bytes)"
   608   dword   4   mbr_0x260   "Unsigned Double-Word (ddw, 4-bytes)"
   612   dword   4   mbr_0x264   "Unsigned Double-Word (ddw, 4-bytes)"
   616   dword   4   mbr_0x268   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 620 Alignment: 1

*/

// Function at 0046d4f0

cls_0x5a4f74 * __thiscall OOAnalyzer::cls_0x5a4f74::cls_0x5a4f74(cls_0x5a4f74 *this)

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->vftptr_0x0 = (cls_0x5a4f74__vftable_5a4f74 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a4f74__vftable_5a4f74_005a4f74;
  return this;
}



// Function at 0046d550

cls_0x5a4f74 * __thiscall OOAnalyzer::cls_0x5a4f74::virt_meth_0x46d550(cls_0x5a4f74 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a4f74(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d570

cls_0x5a4f74 * __thiscall OOAnalyzer::cls_0x5a4f74::~cls_0x5a4f74(cls_0x5a4f74 *this)

{
  cls_0x5a4f74 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d21e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4f74__vftable_5a4f74 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4f74 *)FUN_004830f0(*(undefined4 *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



