#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b2764



// Function at 0050d230

void cls_0x5b2764::virt_meth_0x50d230()

{
  word wVar1;
  int iVar2;
  
  wVar1 = this->mbr_0xc;
  if ((wVar1 == 1) || (wVar1 == 0)) {
    iVar2 = TPlayScreen::meth_0x50d2b0((TPlayScreen *)this);
    if (iVar2 != 0) {
      TPlayScreen::meth_0x50d530((TPlayScreen *)this);
      return;
    }
    TPlayScreen::meth_0x50d530((TPlayScreen *)this);
  }
  else if ((wVar1 == 3) || (wVar1 == 2)) {
    iVar2 = TPlayScreen::meth_0x50d2b0((TPlayScreen *)this);
    if (iVar2 != 0) {
      TPlayScreen::meth_0x50d530((TPlayScreen *)this);
      return;
    }
    TPlayScreen::meth_0x50d530((TPlayScreen *)this);
    return;
  }
  return;
}



// Function at 0050e030

cls_0x5b2764 * cls_0x5b2764::virt_meth_0x50e030()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050e060

cls_0x5a47f0 * cls_0x5b2764::virt_meth_0x50e060()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0a6e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0xfc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b2764__vftable_5b2764_005b2764;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 0050e100

cls_0x5b2764 * cls_0x5b2764::virt_meth_0x50e100()

{
  byte in_stack_00000004;
  
  ~cls_0x5b2764(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0050e120

cls_0x5b2764 * __thiscall cls_0x5b2764::~cls_0x5b2764(cls_0x5b2764 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0a88;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b2764__vftable_5b2764_005b2764;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b2764 *)pcVar1;
}



// Function at 0050e430

TScreen * cls_0x5b2764::virt_meth_0x50e430(undefined4 param_1)

{
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0acb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x108);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    *(undefined4 *)&this_00->field_0xdc = 0;
    this_00->mbr_0xe0 = 0;
    this_00->mbr_0x8 = this_00->mbr_0x8 | 0x8001;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    *(undefined4 *)&this_00->field_0xe8 = 0;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x50e4e0_005b2a4c;
    this_00->mbr_0x104 = 3;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



