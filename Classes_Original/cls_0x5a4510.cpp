// Decompiled methods and structure for class: cls_0x5a4510

/*
/OOAnalyzer/cls_0x5a4510
pack(disabled)
Structure cls_0x5a4510 {
   0   cls_0x5a4510::vftable_5a4510 *   4   vftptr_0x0   "pointer to cls_0x5a4510::vftable_5a4510"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 156 Alignment: 1

*/

// Function at 00445300

cls_0x5a4510 * __thiscall OOAnalyzer::cls_0x5a4510::~cls_0x5a4510(cls_0x5a4510 *this)

{
  cls_0x5a4510 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ce6e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4510 *)FUN_004830f0(this->mbr_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00445360

cls_0x5a4510 * __thiscall OOAnalyzer::cls_0x5a4510::virt_meth_0x445360(cls_0x5a4510 *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a4510(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



