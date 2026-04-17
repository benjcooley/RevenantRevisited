#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x410e10



// Function at 00410e10

cls_0x410e10 * __thiscall cls_0x410e10::~cls_0x410e10(cls_0x410e10 *this)

{
  cls_0x410e10 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c716;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x410df0_005a3784;
  local_4 = 0;
  this->virt_meth_0x40de10();
  if (DAT_005e8730 < 2) {
    DAT_005e8730 = 0;
    if (DAT_005e8518 != (LPCVOID)0x0) {
      FUN_004830f0(DAT_005e8518);
    }
    DAT_005e8518 = (LPCVOID)0x0;
  }
  else {
    DAT_005e8730 = DAT_005e8730 + -1;
  }
  this->mbr_0x0 = (dword)&PTR_virt_meth_0x410ca0_005a370c;
  local_4 = 3;
  this->virt_meth_0x40de10();
  local_4._0_1_ = 2;
  FUN_004830f0((LPCVOID)this->mbr_0x68);
  local_4 = CONCAT31(local_4._1_3_,1);
  FUN_004830f0((LPCVOID)this->mbr_0x54);
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x410e10 *)cls_0x5a47f0::~cls_0x5a47f0((cls_0x5a47f0 *)this);
  ExceptionList = local_c;
  return pcVar1;
}



