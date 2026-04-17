#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4f76e0



// Function at 004f76e0

cls_0x4f76e0 * __thiscall cls_0x4f76e0::~cls_0x4f76e0(cls_0x4f76e0 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f6c8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&cls_0x5a9d88__vftable_5a9d88_005a9d88;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x4f76e0 *)pcVar1;
}



