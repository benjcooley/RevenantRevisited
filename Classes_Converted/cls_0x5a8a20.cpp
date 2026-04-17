#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a8a20



// Function at 004f7190

cls_0x5a8a20 * cls_0x5a8a20::virt_meth_0x4f7190()

{
  byte in_stack_00000004;
  
  ~cls_0x5a8a20(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f71b0

cls_0x5a8a20 * __thiscall cls_0x5a8a20::~cls_0x5a8a20(cls_0x5a8a20 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f598;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a8a20__vftable_5a8a20_005a8a20;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5a8a20 *)pcVar1;
}



