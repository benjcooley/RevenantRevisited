// Decompiled methods and structure for class: cls_0x5aa820

/*
/OOAnalyzer/cls_0x5aa820
pack(disabled)
Structure cls_0x5aa820 {
   0   cls_0x5aa820::vftable_5aa820 *   4   vftptr_0x0   "pointer to cls_0x5aa820::vftable_5aa820"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   60   byte   1   mbr_0x3c   "Unsigned Byte (db)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   172   dword   4   mbr_0xac   "Unsigned Double-Word (ddw, 4-bytes)"
   236   dword   4   mbr_0xec   "Unsigned Double-Word (ddw, 4-bytes)"
   240   dword   4   mbr_0xf0   "Unsigned Double-Word (ddw, 4-bytes)"
   244   dword   4   mbr_0xf4   "Unsigned Double-Word (ddw, 4-bytes)"
   248   dword   4   mbr_0xf8   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   540   dword   4   mbr_0x21c   "Unsigned Double-Word (ddw, 4-bytes)"
   544   dword   4   mbr_0x220   "Unsigned Double-Word (ddw, 4-bytes)"
   548   dword   4   mbr_0x224   "Unsigned Double-Word (ddw, 4-bytes)"
   972   dword   4   mbr_0x3cc   "Unsigned Double-Word (ddw, 4-bytes)"
   976   dword   4   mbr_0x3d0   "Unsigned Double-Word (ddw, 4-bytes)"
   980   dword   4   mbr_0x3d4   "Unsigned Double-Word (ddw, 4-bytes)"
   1404   dword   4   mbr_0x57c   "Unsigned Double-Word (ddw, 4-bytes)"
   1408   dword   4   mbr_0x580   "Unsigned Double-Word (ddw, 4-bytes)"
   1412   dword   4   mbr_0x584   "Unsigned Double-Word (ddw, 4-bytes)"
   1836   dword   4   mbr_0x72c   "Unsigned Double-Word (ddw, 4-bytes)"
   1980   dword   4   mbr_0x7bc   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 1984 Alignment: 1

*/

// Function at 004f6910

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5aa820::virt_meth_0x4f6910(cls_0x5aa820 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f42e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0xfc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5aa820__vftable_5aa820_005aa820;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f7940

cls_0x5aa820 * __thiscall OOAnalyzer::cls_0x5aa820::virt_meth_0x4f7940(cls_0x5aa820 *this)

{
  byte in_stack_00000004;
  
  cls_0x4f7960::~cls_0x4f7960((cls_0x4f7960 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7a70

cls_0x5aa820 * __thiscall OOAnalyzer::cls_0x5aa820::virt_meth_0x4f7a70(cls_0x5aa820 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5aa820(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7a90

cls_0x5aa820 * __thiscall OOAnalyzer::cls_0x5aa820::~cls_0x5aa820(cls_0x5aa820 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f7a8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5aa820__vftable_5aa820_005aa820;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5aa820 *)pcVar1;
}



