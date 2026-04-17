// Decompiled methods and structure for class: cls_0x49a0a0

/*
/OOAnalyzer/cls_0x49a0a0
pack(disabled)
Structure cls_0x49a0a0 {
   0   cls_0x41c7f0   24   cls_0x41c7f0   "Component (member) class."
}
Length: 24 Alignment: 1

*/

// Function at 0049a0a0

cls_0x49a0a0 * __thiscall OOAnalyzer::cls_0x49a0a0::cls_0x49a0a0(cls_0x49a0a0 *this)

{
  undefined4 *puVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059db88;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0);
  local_4 = 0;
  puVar1 = FUN_00482fb0(4);
  (this->cls_0x41c7f0).mbr_0x14 = (dword)puVar1;
  *puVar1 = 0;
  ExceptionList = local_c;
  return this;
}



