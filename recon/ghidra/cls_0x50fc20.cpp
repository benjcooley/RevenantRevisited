// Decompiled methods and structure for class: cls_0x50fc20

/*
/OOAnalyzer/cls_0x50fc20
pack(disabled)
Structure cls_0x50fc20 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 4 Alignment: 1

*/

// Function at 0050fc20

cls_0x50fc20 * __thiscall OOAnalyzer::cls_0x50fc20::~cls_0x50fc20(cls_0x50fc20 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0c58;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&cls_0x5b30fc__vftable_5b30fc_005b30fc;
  local_4 = 0;
  cls_0x5a7e38::virt_meth_0x40de10((cls_0x5a7e38 *)this);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x50fc20 *)pcVar1;
}



