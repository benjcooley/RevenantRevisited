#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x523960



// Function at 00523960

cls_0x523960 * __thiscall cls_0x523960::~cls_0x523960(cls_0x523960 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1438;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x523940_005b6124;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x523960 *)pcVar1;
}



