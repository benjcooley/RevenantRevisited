// Decompiled methods and structure for class: cls_0x406070

/*
/OOAnalyzer/cls_0x406070
pack(disabled)
Structure cls_0x406070 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 44 Alignment: 1

*/

// Function at 00406070

cls_0x406070 * __thiscall OOAnalyzer::cls_0x406070::~cls_0x406070(cls_0x406070 *this)

{
  cls_0x406070 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c51b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&cls_0x5a34a0__vftable_5a34a0_005a34a0;
  local_4 = 0;
  cls_0x5a3544::virt_meth_0x40db50((cls_0x5a3544 *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x406070 *)FUN_004830f0(this->mbr_0x28);
  ExceptionList = local_c;
  return pcVar1;
}



