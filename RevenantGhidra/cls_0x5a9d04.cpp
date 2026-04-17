// Decompiled methods and structure for class: cls_0x5a9d04

/*
/OOAnalyzer/cls_0x5a9d04
pack(disabled)
Structure cls_0x5a9d04 {
   0   cls_0x5a9d04::vftable_5a9d04 *   4   vftptr_0x0   "pointer to cls_0x5a9d04::vftable_5a9d04"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   216   dword   4   mbr_0xd8   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   392   dword   4   mbr_0x188   "Unsigned Double-Word (ddw, 4-bytes)"
   396   dword   4   mbr_0x18c   "Unsigned Double-Word (ddw, 4-bytes)"
   400   dword   4   mbr_0x190   "Unsigned Double-Word (ddw, 4-bytes)"
   620   dword   4   mbr_0x26c   "Unsigned Double-Word (ddw, 4-bytes)"
   972   dword   4   mbr_0x3cc   "Unsigned Double-Word (ddw, 4-bytes)"
   1092   dword   4   mbr_0x444   "Unsigned Double-Word (ddw, 4-bytes)"
   1212   dword   4   mbr_0x4bc   "Unsigned Double-Word (ddw, 4-bytes)"
   1216   dword   4   mbr_0x4c0   "Unsigned Double-Word (ddw, 4-bytes)"
   1220   dword   4   mbr_0x4c4   "Unsigned Double-Word (ddw, 4-bytes)"
   1224   dword   4   mbr_0x4c8   "Unsigned Double-Word (ddw, 4-bytes)"
   1236   dword   4   mbr_0x4d4   "Unsigned Double-Word (ddw, 4-bytes)"
   1240   dword   4   mbr_0x4d8   "Unsigned Double-Word (ddw, 4-bytes)"
   1244   dword   4   mbr_0x4dc   "Unsigned Double-Word (ddw, 4-bytes)"
   1248   dword   4   mbr_0x4e0   "Unsigned Double-Word (ddw, 4-bytes)"
   1252   dword   4   mbr_0x4e4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1256 Alignment: 1

*/

// Function at 004f5c00

cls_0x5a9d04 * __thiscall OOAnalyzer::cls_0x5a9d04::virt_meth_0x4f5c00(cls_0x5a9d04 *this)

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f5c30

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a9d04::virt_meth_0x4f5c30(cls_0x5a9d04 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f0de;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x4e8);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a9d04__vftable_5a9d04_005a9d04;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f7650

cls_0x5a9d04 * __thiscall OOAnalyzer::cls_0x5a9d04::virt_meth_0x4f7650(cls_0x5a9d04 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a9d04(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7670

cls_0x5a9d04 * __thiscall OOAnalyzer::cls_0x5a9d04::~cls_0x5a9d04(cls_0x5a9d04 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f6a8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a9d04__vftable_5a9d04_005a9d04;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5a9d04 *)pcVar1;
}



