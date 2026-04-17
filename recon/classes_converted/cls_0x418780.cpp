#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x418780



// Function at 00418780

cls_0x418780 * __thiscall cls_0x418780::~cls_0x418780(cls_0x418780 *this)

{
  cls_0x418780 *pcVar1;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c818;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x419ff0_005a384c;
  local_4 = 0;
  cls_0x5a35ac::meth_0x4191b0((cls_0x5a35ac *)this);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x418780 *)cls_0x5a35ac::meth_0x446260((cls_0x5a35ac *)this);
  ExceptionList = pvStack_c;
  return pcVar1;
}



