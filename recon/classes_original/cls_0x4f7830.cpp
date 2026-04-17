// Decompiled methods and structure for class: cls_0x4f7830

/*
/OOAnalyzer/cls_0x4f7830
pack(disabled)
Structure cls_0x4f7830 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 004f7830

cls_0x4f7830 * __thiscall OOAnalyzer::cls_0x4f7830::~cls_0x4f7830(cls_0x4f7830 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f728;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x4f7810_005a9ec0;
  local_4 = 0;
  cls_0x5a9f2c::virt_meth_0x4e5400((cls_0x5a9f2c *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x4f7830 *)pcVar1;
}



