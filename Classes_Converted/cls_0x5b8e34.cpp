#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b8e34



// Function at 00528a20

cls_0x5b8e34 * cls_0x5b8e34::virt_meth_0x528a20()

{
  byte in_stack_00000004;
  
  ~cls_0x5b8e34(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00528a40

cls_0x5b8e34 * __thiscall cls_0x5b8e34::~cls_0x5b8e34(cls_0x5b8e34 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1758;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b8e34__vftable_5b8e34_005b8e34;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b8e34 *)pcVar1;
}



