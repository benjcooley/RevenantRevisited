#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4878f0



// Function at 004878f0

cls_0x4878f0::cls_0x4878f0()

{
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d4e8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x40);
  local_4 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x41c7f0).mbr_0x14,0x40);
  ExceptionList = local_c;
  return this;
}



