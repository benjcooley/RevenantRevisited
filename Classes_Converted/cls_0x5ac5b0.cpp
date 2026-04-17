#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5ac5b0



// Function at 004f0780

void cls_0x5ac5b0::virt_meth_0x4f0780()

{
  int iVar1;
  dword dVar2;
  undefined4 uVar3;
  
  this->virt_meth_0x40dd60();
  this->mbr_0x104 = 0;
  this->mbr_0x100 = 0;
  iVar1 = *(int *)this->mbr_0x4;
  this->mbr_0x10c = 0x3f000000;
  dVar2 = (**(code **)(iVar1 + 0x200))();
  this->mbr_0xfc = dVar2;
  uVar3 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,uVar3);
  this->mbr_0x110 = 0;
  this->mbr_0x108 = 0;
  return;
}



// Function at 004f8dc0

cls_0x5ac5b0 * cls_0x5ac5b0::virt_meth_0x4f8dc0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8df0

cls_0x5a47f0 * cls_0x5ac5b0::virt_meth_0x4f8df0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fbae;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x118);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ac5b0__vftable_5ac5b0_005ac5b0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f8e90

cls_0x5ac5b0 * cls_0x5ac5b0::virt_meth_0x4f8e90()

{
  byte in_stack_00000004;
  
  ~cls_0x5ac5b0(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8eb0

cls_0x5ac5b0 * __thiscall cls_0x5ac5b0::~cls_0x5ac5b0(cls_0x5ac5b0 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fbc8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ac5b0__vftable_5ac5b0_005ac5b0;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ac5b0 *)pcVar1;
}



