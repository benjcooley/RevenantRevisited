// Decompiled methods and structure for class: cls_0x488160

/*
/OOAnalyzer/cls_0x488160
pack(disabled)
Structure cls_0x488160 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
   64   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
   88   cls_0x49c960   4   cls_0x49c960   "Component (member) class."
}
Length: 92 Alignment: 1

*/

// Function at 00488160

cls_0x488160 * __thiscall OOAnalyzer::cls_0x488160::cls_0x488160(cls_0x488160 *this)

{
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d5fc;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x10);
  local_4 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x41c7f0).mbr_0x14,0x10);
  local_4._0_1_ = 1;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x2c,0x10);
  local_4._0_1_ = 2;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x10);
  local_4 = CONCAT31(local_4._1_3_,3);
  cls_0x49c960::cls_0x49c960(&this->cls_0x49c960);
  this->mbr_0x0 = 0;
  ExceptionList = local_c;
  return this;
}



