#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4883c0



// Function at 004883c0

void cls_0x4883c0::meth_0x4883c0()

{
  LPCVOID pvVar1;
  SIZE_T SVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d66b;
  local_c = ExceptionList;
  local_4 = 0;
  ExceptionList = &local_c;
  meth_0x53ea70(this);
  pvVar1 = (LPCVOID)this->mbr_0x14;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  FUN_0058cfab(pvVar1);
  ExceptionList = local_c;
  return;
}



// Function at 0053ea70

void cls_0x4883c0::meth_0x53ea70()

{
  dword *this_00;
  cls_0x45f7c0 *this_01;
  int iVar1;
  
  this_00 = &this->mbr_0x4;
  iVar1 = 0;
  if ((int)this->mbr_0x4 < 1) {
    *this_00 = 0;
    this->mbr_0x8 = 0;
    return;
  }
  do {
    if ((-1 < iVar1) &&
       (this_01 = *(cls_0x45f7c0 **)(this->mbr_0x14 + iVar1 * 4), this_01 != (cls_0x45f7c0 *)0x0)) {
      cls_0x45f7c0::meth_0x53db70(this_01);
      FUN_004830f0(this_01);
    }
    cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
    iVar1 = iVar1 + 1;
  } while (iVar1 < (int)*this_00);
  *this_00 = 0;
  this->mbr_0x8 = 0;
  return;
}



