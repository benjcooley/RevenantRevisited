#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4453b0



// Function at 004453b0

cls_0x4453b0 * __thiscall cls_0x4453b0::~cls_0x4453b0(cls_0x4453b0 *this)

{
  cls_0x4453b0 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059ce96;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 0;
  this->virt_meth_0x434f30();
  this->mbr_0x0 = (dword)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x4453b0 *)FUN_004830f0((LPCVOID)this->mbr_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



