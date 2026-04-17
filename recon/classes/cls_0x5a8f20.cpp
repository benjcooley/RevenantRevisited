// Decompiled methods and structure for class: cls_0x5a8f20

/*
/OOAnalyzer/cls_0x5a8f20
pack(disabled)
Structure cls_0x5a8f20 {
   0   cls_0x5a8f20::vftable_5a8f20 *   4   vftptr_0x0   "pointer to cls_0x5a8f20::vftable_5a8f20"
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
   264   dword   4   mbr_0x108   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   276   dword   4   mbr_0x114   "Unsigned Double-Word (ddw, 4-bytes)"
   292   dword   4   mbr_0x124   "Unsigned Double-Word (ddw, 4-bytes)"
   296   dword   4   mbr_0x128   "Unsigned Double-Word (ddw, 4-bytes)"
   300   dword   4   mbr_0x12c   "Unsigned Double-Word (ddw, 4-bytes)"
   304   dword   4   mbr_0x130   "Unsigned Double-Word (ddw, 4-bytes)"
   308   dword   4   mbr_0x134   "Unsigned Double-Word (ddw, 4-bytes)"
   312   dword   4   mbr_0x138   "Unsigned Double-Word (ddw, 4-bytes)"
   328   dword   4   mbr_0x148   "Unsigned Double-Word (ddw, 4-bytes)"
   332   dword   4   mbr_0x14c   "Unsigned Double-Word (ddw, 4-bytes)"
   388   dword   4   mbr_0x184   "Unsigned Double-Word (ddw, 4-bytes)"
   6976   dword   4   mbr_0x1b40   "Unsigned Double-Word (ddw, 4-bytes)"
   6984   dword   4   mbr_0x1b48   "Unsigned Double-Word (ddw, 4-bytes)"
   6988   dword   4   mbr_0x1b4c   "Unsigned Double-Word (ddw, 4-bytes)"
   6992   dword   4   mbr_0x1b50   "Unsigned Double-Word (ddw, 4-bytes)"
   7012   dword   4   mbr_0x1b64   "Unsigned Double-Word (ddw, 4-bytes)"
   7016   dword   4   mbr_0x1b68   "Unsigned Double-Word (ddw, 4-bytes)"
   7020   dword   4   mbr_0x1b6c   "Unsigned Double-Word (ddw, 4-bytes)"
   7024   dword   4   mbr_0x1b70   "Unsigned Double-Word (ddw, 4-bytes)"
   7028   dword   4   mbr_0x1b74   "Unsigned Double-Word (ddw, 4-bytes)"
   7032   dword   4   mbr_0x1b78   "Unsigned Double-Word (ddw, 4-bytes)"
   7036   dword   4   mbr_0x1b7c   "Unsigned Double-Word (ddw, 4-bytes)"
   7040   dword   4   mbr_0x1b80   "Unsigned Double-Word (ddw, 4-bytes)"
   7044   dword   4   mbr_0x1b84   "Unsigned Double-Word (ddw, 4-bytes)"
   7052   dword   4   mbr_0x1b8c   "Unsigned Double-Word (ddw, 4-bytes)"
   7056   dword   4   mbr_0x1b90   "Unsigned Double-Word (ddw, 4-bytes)"
   7396   dword   4   mbr_0x1ce4   "Unsigned Double-Word (ddw, 4-bytes)"
   7400   dword   4   mbr_0x1ce8   "Unsigned Double-Word (ddw, 4-bytes)"
   7408   dword   4   mbr_0x1cf0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 7412 Alignment: 1

*/

// Function at 004f4fb0

cls_0x5a8f20 * __thiscall OOAnalyzer::cls_0x5a8f20::virt_meth_0x4f4fb0(cls_0x5a8f20 *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f4fe0

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5a8f20::virt_meth_0x4f4fe0(cls_0x5a8f20 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ee1e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x1cf4);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a8f20__vftable_5a8f20_005a8f20;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f5080

cls_0x5a8f20 * __thiscall OOAnalyzer::cls_0x5a8f20::virt_meth_0x4f5080(cls_0x5a8f20 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a8f20(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f50a0

cls_0x5a8f20 * __thiscall OOAnalyzer::cls_0x5a8f20::~cls_0x5a8f20(cls_0x5a8f20 *this)

{
  cls_0x5a8f20 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ee38;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a8f20__vftable_5a8f20_005a8f20;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a8f20 *)cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return pcVar1;
}



