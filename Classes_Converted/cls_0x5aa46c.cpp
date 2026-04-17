#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5aa46c



// Function at 004e7530

void cls_0x5aa46c::virt_meth_0x4e7530()

{
  this->virt_meth_0x40dd60();
  (**(code **)(*(int *)this->mbr_0x4 + 0x40))(((int *)this->mbr_0x4)[2] | 0x100);
  return;
}



// Function at 004f64c0

cls_0x5aa46c * cls_0x5aa46c::virt_meth_0x4f64c0()

{
  byte in_stack_00000004;
  
  ~cls_0x5aa46c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f64e0

cls_0x5aa46c * __thiscall cls_0x5aa46c::~cls_0x5aa46c(cls_0x5aa46c *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f2f8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5aa46c__vftable_5aa46c_005aa46c;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5aa46c *)pcVar1;
}



// Function at 004f6530

cls_0x5a47f0 * cls_0x5aa46c::virt_meth_0x4f6530()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f32e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x4dc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5aa46c__vftable_5aa46c_005aa46c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



