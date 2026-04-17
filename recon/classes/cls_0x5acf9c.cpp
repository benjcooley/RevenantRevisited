// Decompiled methods and structure for class: cls_0x5acf9c

/*
/OOAnalyzer/cls_0x5acf9c
pack(disabled)
Structure cls_0x5acf9c {
   0   cls_0x5acf9c::vftable_5acf9c *   4   vftptr_0x0   "pointer to cls_0x5acf9c::vftable_5acf9c"
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
}
Length: 260 Alignment: 1

*/

// Function at 004f9710

cls_0x5acf9c * __thiscall OOAnalyzer::cls_0x5acf9c::virt_meth_0x4f9710(cls_0x5acf9c *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9740

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5acf9c::virt_meth_0x4f9740(cls_0x5acf9c *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fd6e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x104);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5acf9c__vftable_5acf9c_005acf9c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f97e0

cls_0x5acf9c * __thiscall OOAnalyzer::cls_0x5acf9c::virt_meth_0x4f97e0(cls_0x5acf9c *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5acf9c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9800

cls_0x5acf9c * __thiscall OOAnalyzer::cls_0x5acf9c::~cls_0x5acf9c(cls_0x5acf9c *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fd88;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5acf9c__vftable_5acf9c_005acf9c;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5acf9c *)pcVar1;
}



// Function at 004f9850

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5acf9c::virt_meth_0x4f9850(cls_0x5acf9c *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fdbe;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x4f8);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ad004__vftable_5ad004_005ad004;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



