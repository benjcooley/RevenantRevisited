// Decompiled methods and structure for class: cls_0x488c10

/*
/OOAnalyzer/cls_0x488c10
pack(disabled)
Structure cls_0x488c10 {
   8   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
}
Length: 52 Alignment: 1

*/

// Function at 00488c10

cls_0x488c10 * __thiscall OOAnalyzer::cls_0x488c10::cls_0x488c10(cls_0x488c10 *this)

{
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d73b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,8);
  local_4 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x41c7f0).mbr_0x14,0x40);
  ExceptionList = local_c;
  return this;
}



