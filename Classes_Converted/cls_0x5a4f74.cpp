#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a4f74



// Function at 0046d4f0

cls_0x5a4f74::cls_0x5a4f74()

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->vftptr_0x0 = (cls_0x5a4f74__vftable_5a4f74 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a4f74__vftable_5a4f74_005a4f74;
  return this;
}



// Function at 0046d550

cls_0x5a4f74 * cls_0x5a4f74::virt_meth_0x46d550()

{
  byte in_stack_00000004;
  
  ~cls_0x5a4f74(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d570

cls_0x5a4f74 * __thiscall cls_0x5a4f74::~cls_0x5a4f74(cls_0x5a4f74 *this)

{
  cls_0x5a4f74 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d21e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4f74__vftable_5a4f74 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4f74 *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



