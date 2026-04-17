// Decompiled methods and structure for class: cls_0x4453b0

/*
/OOAnalyzer/cls_0x4453b0
pack(disabled)
Structure cls_0x4453b0 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 156 Alignment: 1

*/

// Function at 004453b0

cls_0x4453b0 * __thiscall OOAnalyzer::cls_0x4453b0::~cls_0x4453b0(cls_0x4453b0 *this)

{
  cls_0x4453b0 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ce96;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 0;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  this->mbr_0x0 = (dword)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  cls_0x5b98b8::virt_meth_0x434f30((cls_0x5b98b8 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x4453b0 *)FUN_004830f0(this->mbr_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



