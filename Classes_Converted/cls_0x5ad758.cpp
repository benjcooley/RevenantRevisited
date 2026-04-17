#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5ad758



// Function at 004f3970

void cls_0x5ad758::virt_meth_0x4f3970()

{
  this->virt_meth_0x40de10();
  if ((LPCVOID)this->mbr_0xfc != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xfc);
  }
  this->mbr_0xfc = 0;
  return;
}



// Function at 004f9df0

cls_0x5ad758 * cls_0x5ad758::virt_meth_0x4f9df0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9e20

cls_0x5a47f0 * cls_0x5ad758::virt_meth_0x4f9e20()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ff0e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x110);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ad758__vftable_5ad758_005ad758;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f9ec0

cls_0x5ad758 * cls_0x5ad758::virt_meth_0x4f9ec0()

{
  byte in_stack_00000004;
  
  ~cls_0x5ad758(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9ee0

cls_0x5ad758 * __thiscall cls_0x5ad758::~cls_0x5ad758(cls_0x5ad758 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ff28;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ad758__vftable_5ad758_005ad758;
  local_4 = 0;
  this->virt_meth_0x40de10();
  if ((LPCVOID)this->mbr_0xfc != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0xfc);
  }
  this->mbr_0xfc = 0;
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ad758 *)pcVar1;
}



