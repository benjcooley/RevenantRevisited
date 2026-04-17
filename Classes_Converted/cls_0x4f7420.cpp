#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4f7420



// Function at 004f7420

cls_0x4f7420 * __thiscall cls_0x4f7420::~cls_0x4f7420(cls_0x4f7420 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f618;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x4f7360_005ab18c;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x4f7420 *)pcVar1;
}



