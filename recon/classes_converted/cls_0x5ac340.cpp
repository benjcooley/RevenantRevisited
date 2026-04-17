#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5ac340



// Function at 004f0410

void cls_0x5ac340::virt_meth_0x4f0410()

{
  this->virt_meth_0x40dd60();
  this->mbr_0xfc = 0;
  return;
}



// Function at 004f8bb0

cls_0x5ac340 * cls_0x5ac340::virt_meth_0x4f8bb0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8be0

cls_0x5a47f0 * cls_0x5ac340::virt_meth_0x4f8be0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fb3e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x104);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ac340__vftable_5ac340_005ac340;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f8c80

cls_0x5ac340 * cls_0x5ac340::virt_meth_0x4f8c80()

{
  byte in_stack_00000004;
  
  ~cls_0x5ac340(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8ca0

cls_0x5ac340 * __thiscall cls_0x5ac340::~cls_0x5ac340(cls_0x5ac340 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fb58;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ac340__vftable_5ac340_005ac340;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ac340 *)pcVar1;
}



// Function at 004f8cf0

TScreen * cls_0x5ac340::virt_meth_0x4f8cf0(int param_1)

{
  uint uVar1;
  uint uVar2;
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fb7b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x188);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    uVar1 = *(uint *)(param_1 + 4);
    uVar2 = this_00->mbr_0x8;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    this_00[1].mbr_0x38 = 0;
    this_00->mbr_0x8 = uVar2 | uVar1 | 0x48001;
    this_00[1].mbr_0x1c = 0;
    this_00[1].mbr_0xc = 0;
    this_00[1].mbr_0xe = 0;
    *(undefined *)&this_00[1].mbr_0x48 = 0;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4f8dc0_005ac3a4;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



