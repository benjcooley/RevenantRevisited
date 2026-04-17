#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x405aa0



// Function at 00405aa0

cls_0x405aa0 * __thiscall cls_0x405aa0::~cls_0x405aa0(cls_0x405aa0 *this)

{
  cls_0x405aa0 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  uint local_4;
  
  puStack_8 = &LAB_0059c26e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x40d3e0_005a3480;
  local_4 = 1;
  this->virt_meth_0x401900();
  local_4 = local_4 & 0xffffff00;
  FUN_004830f0((LPCVOID)this->mbr_0x48);
  this->mbr_0x0 = (dword)&cls_0x5a34a0__vftable_5a34a0_005a34a0;
  local_4 = 2;
  this->virt_meth_0x40db50();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x405aa0 *)FUN_004830f0((LPCVOID)this->mbr_0x28);
  ExceptionList = local_c;
  return pcVar1;
}



