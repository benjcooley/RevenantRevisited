#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x487810



// Function at 00487810

cls_0x487810::cls_0x487810()

{
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d4c8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,4);
  local_4 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x41c7f0).mbr_0x14,8);
  ExceptionList = local_c;
  return this;
}



