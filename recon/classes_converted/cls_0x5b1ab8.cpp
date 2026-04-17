#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b1ab8



// Function at 00509e80

cls_0x5b1ab8 * cls_0x5b1ab8::virt_meth_0x509e80()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509eb0

cls_0x5a47f0 * cls_0x5b1ab8::virt_meth_0x509eb0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a087e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x3c4);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b1ab8__vftable_5b1ab8_005b1ab8;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00509f50

cls_0x5b1ab8 * cls_0x5b1ab8::virt_meth_0x509f50()

{
  byte in_stack_00000004;
  
  ~cls_0x5b1ab8(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509f70

cls_0x5b1ab8 * __thiscall cls_0x5b1ab8::~cls_0x5b1ab8(cls_0x5b1ab8 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0898;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b1ab8__vftable_5b1ab8_005b1ab8;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b1ab8 *)pcVar1;
}



