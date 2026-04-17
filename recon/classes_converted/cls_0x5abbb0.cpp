#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5abbb0



// Function at 004eeae0

void cls_0x5abbb0::virt_meth_0x4eeae0()

{
  if ((LPCVOID)this->mbr_0x100 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x100);
    this->mbr_0x100 = 0;
  }
  if ((LPCVOID)this->mbr_0x104 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x104);
    this->mbr_0x104 = 0;
  }
  if ((LPCVOID)this->mbr_0x108 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x108);
    this->mbr_0x108 = 0;
  }
  this->virt_meth_0x40de10();
  return;
}



// Function at 004f8540

cls_0x5abbb0 * cls_0x5abbb0::virt_meth_0x4f8540()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8570

cls_0x5a47f0 * cls_0x5abbb0::virt_meth_0x4f8570()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f9ee;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x10c);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5abbb0__vftable_5abbb0_005abbb0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f8610

cls_0x5abbb0 * cls_0x5abbb0::virt_meth_0x4f8610()

{
  byte in_stack_00000004;
  
  ~cls_0x5abbb0(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8630

cls_0x5abbb0 * __thiscall cls_0x5abbb0::~cls_0x5abbb0(cls_0x5abbb0 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fa08;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5abbb0__vftable_5abbb0_005abbb0;
  local_4 = 0;
  if ((LPCVOID)this->mbr_0x100 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x100);
    this->mbr_0x100 = 0;
  }
  if ((LPCVOID)this->mbr_0x104 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x104);
    this->mbr_0x104 = 0;
  }
  if ((LPCVOID)this->mbr_0x108 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x108);
    this->mbr_0x108 = 0;
  }
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5abbb0 *)pcVar1;
}



