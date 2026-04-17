// Decompiled methods and structure for class: cls_0x5b1ab8

/*
/OOAnalyzer/cls_0x5b1ab8
pack(disabled)
Structure cls_0x5b1ab8 {
   0   cls_0x5b1ab8::vftable_5b1ab8 *   4   vftptr_0x0   "pointer to cls_0x5b1ab8::vftable_5b1ab8"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   508   dword   4   mbr_0x1fc   "Unsigned Double-Word (ddw, 4-bytes)"
   512   dword   4   mbr_0x200   "Unsigned Double-Word (ddw, 4-bytes)"
   516   dword   4   mbr_0x204   "Unsigned Double-Word (ddw, 4-bytes)"
   636   dword   4   mbr_0x27c   "Unsigned Double-Word (ddw, 4-bytes)"
   640   dword   4   mbr_0x280   "Unsigned Double-Word (ddw, 4-bytes)"
   644   dword   4   mbr_0x284   "Unsigned Double-Word (ddw, 4-bytes)"
   764   dword   4   mbr_0x2fc   "Unsigned Double-Word (ddw, 4-bytes)"
   768   dword   4   mbr_0x300   "Unsigned Double-Word (ddw, 4-bytes)"
   772   dword   4   mbr_0x304   "Unsigned Double-Word (ddw, 4-bytes)"
   892   dword   4   mbr_0x37c   "Unsigned Double-Word (ddw, 4-bytes)"
   908   dword   4   mbr_0x38c   "Unsigned Double-Word (ddw, 4-bytes)"
   924   byte   1   mbr_0x39c   "Unsigned Byte (db)"
   928   dword   4   mbr_0x3a0   "Unsigned Double-Word (ddw, 4-bytes)"
   944   dword   4   mbr_0x3b0   "Unsigned Double-Word (ddw, 4-bytes)"
   948   dword   4   mbr_0x3b4   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 952 Alignment: 1

*/

// Function at 00509e80

cls_0x5b1ab8 * __thiscall OOAnalyzer::cls_0x5b1ab8::virt_meth_0x509e80(cls_0x5b1ab8 *this)

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509eb0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5b1ab8::virt_meth_0x509eb0(cls_0x5b1ab8 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a087e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x3c4);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b1ab8__vftable_5b1ab8_005b1ab8;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00509f50

cls_0x5b1ab8 * __thiscall OOAnalyzer::cls_0x5b1ab8::virt_meth_0x509f50(cls_0x5b1ab8 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5b1ab8(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509f70

cls_0x5b1ab8 * __thiscall OOAnalyzer::cls_0x5b1ab8::~cls_0x5b1ab8(cls_0x5b1ab8 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0898;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b1ab8__vftable_5b1ab8_005b1ab8;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b1ab8 *)pcVar1;
}



