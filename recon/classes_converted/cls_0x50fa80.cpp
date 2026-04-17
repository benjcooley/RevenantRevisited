#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x50fa80



// Function at 0050fa80

cls_0x50fa80 * __thiscall cls_0x50fa80::~cls_0x50fa80(cls_0x50fa80 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0c18;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&cls_0x5b30fc__vftable_5b30fc_005b30fc;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x50fa80 *)pcVar1;
}



