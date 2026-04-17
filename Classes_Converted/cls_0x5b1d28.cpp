#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b1d28



// Function at 0050a020

cls_0x5b1d28 * cls_0x5b1d28::virt_meth_0x50a020()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050a050

cls_0x5a47f0 * cls_0x5b1d28::virt_meth_0x50a050()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a08ee;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x9130);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b1d28__vftable_5b1d28_005b1d28;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 0050a0f0

cls_0x5b1d28 * cls_0x5b1d28::virt_meth_0x50a0f0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b1d28(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050a110

cls_0x5b1d28 * __thiscall cls_0x5b1d28::~cls_0x5b1d28(cls_0x5b1d28 *this)

{
  cls_0x5b1d28 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0908;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b1d28__vftable_5b1d28_005b1d28;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5b1d28 *)cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 0050a160

cls_0x5a85ac * cls_0x5b1d28::virt_meth_0x50a160(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0933;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x1ac);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    local_4 = CONCAT31(local_4._1_3_,1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x50a1f0_005b1d8c;
    (*(code *)PTR_virt_meth_0x471b50_005b1ee4)(0);
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x30 + 3) = 1;
    ExceptionList = this_00;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



