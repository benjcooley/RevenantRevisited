#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x488b20



// Function at 00488b20

cls_0x488b20::cls_0x488b20()

{
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d724;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = 0;
  local_4 = 0;
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x40);
  local_4._0_1_ = 1;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x41c7f0).mbr_0x14,0x40);
  local_4 = CONCAT31(local_4._1_3_,2);
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x10);
  (this->cls_0x41c7f0).mbr_0x14 = 0;
  ExceptionList = local_c;
  return this;
}



