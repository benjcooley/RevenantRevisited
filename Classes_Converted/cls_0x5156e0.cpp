#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5156e0



// Function at 005156e0

cls_0x5156e0 * __thiscall cls_0x5156e0::~cls_0x5156e0(cls_0x5156e0 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0fc4;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x5156c0_005b4a18;
  local_4 = 2;
  this->virt_meth_0x40de10();
  local_4._0_1_ = 1;
  this->mbr_0x2d8 = (dword)&cls_0x5b42f0__vftable_5b42f0_005b42f0;
  FUN_004830f0((LPCVOID)this->mbr_0x344);
  local_4 = (uint)local_4._1_3_ << 8;
  this->mbr_0x248 = (dword)&cls_0x5b4308__vftable_5b4308_005b4308;
  FUN_004830f0((LPCVOID)this->mbr_0x2c8);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5156e0 *)pcVar1;
}



