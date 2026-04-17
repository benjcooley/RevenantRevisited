// Decompiled methods and structure for class: cls_0x5ab930

/*
/OOAnalyzer/cls_0x5ab930
pack(disabled)
Structure cls_0x5ab930 {
   0   cls_0x5ab930::vftable_5ab930 *   4   vftptr_0x0   "pointer to cls_0x5ab930::vftable_5ab930"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   252   dword   4   mbr_0xfc   "Unsigned Double-Word (ddw, 4-bytes)"
   256   dword   4   mbr_0x100   "Unsigned Double-Word (ddw, 4-bytes)"
   260   dword   4   mbr_0x104   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 264 Alignment: 1

*/

// Function at 004f8350

cls_0x5ab930 * __thiscall OOAnalyzer::cls_0x5ab930::virt_meth_0x4f8350(cls_0x5ab930 *this)

{
  byte in_stack_00000004;
  
  cls_0x5a50e8::~cls_0x5a50e8((cls_0x5a50e8 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8380

cls_0x5a47f0 * __thiscall OOAnalyzer::cls_0x5ab930::virt_meth_0x4f8380(cls_0x5ab930 *this)

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f97e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x108);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ab930__vftable_5ab930_005ab930;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f8420

cls_0x5ab930 * __thiscall OOAnalyzer::cls_0x5ab930::virt_meth_0x4f8420(cls_0x5ab930 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5ab930(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8440

cls_0x5ab930 * __thiscall OOAnalyzer::cls_0x5ab930::~cls_0x5ab930(cls_0x5ab930 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f998;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ab930__vftable_5ab930_005ab930;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ab930 *)pcVar1;
}



