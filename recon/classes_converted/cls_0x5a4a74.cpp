#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a4a74



// Function at 00469a60

bool cls_0x5a4a74::virt_meth_0x469a60()

{
  int iVar1;
  
  iVar1 = this->virt_meth_0x435150(s_hostgame_005d3cc8,0x659cbc,0,0,0,0x280,0x1e0,0x1c2,0xa0,
                     s_widgets_005d3cc0);
  return iVar1 != 0;
}



// Function at 0046d390

cls_0x5a4a74 * cls_0x5a4a74::virt_meth_0x46d390()

{
  byte in_stack_00000004;
  
  ~cls_0x5a4a74(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0046d3b0

cls_0x5a4a74 * __thiscall cls_0x5a4a74::~cls_0x5a4a74(cls_0x5a4a74 *this)

{
  cls_0x5a4a74 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d1de;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a4a74__vftable_5a4a74 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 0;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5a4a74 *)FUN_004830f0((LPCVOID)this->mbr_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



