#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a4510



// Function at 00445300

cls_0x5a4510 * __thiscall cls_0x5a4510::~cls_0x5a4510(cls_0x5a4510 *this)

{
  cls_0x5a4510 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ce6e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4510 *)FUN_004830f0((LPCVOID)this->mbr_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00445360

cls_0x5a4510 * cls_0x5a4510::virt_meth_0x445360()

{
  byte in_stack_00000004;
  
  ~cls_0x5a4510(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



