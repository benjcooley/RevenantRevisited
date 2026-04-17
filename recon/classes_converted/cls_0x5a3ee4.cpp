#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a3ee4



// Function at 00438d30

void cls_0x5a3ee4::virt_meth_0x438d30()

{
  (this->cls_0x5a68d8).mbr_0x10 = (this->cls_0x5a68d8).mbr_0x4;
  (this->cls_0x5a68d8).mbr_0x34 = (this->cls_0x5a68d8).mbr_0x64 + 0x48;
  return;
}



// Function at 00438d40

undefined4 cls_0x5a3ee4::virt_meth_0x438d40()

{
  (this->cls_0x5a68d8).mbr_0x34 = 0;
  return 1;
}



// Function at 00438d60

cls_0x5a3ee4 * cls_0x5a3ee4::virt_meth_0x438d60()

{
  byte in_stack_00000004;
  
  ~cls_0x5a3ee4(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004a3910

cls_0x5a3ee4::cls_0x5a3ee4()

{
  cls_0x5a68d8::cls_0x5a68d8(&this->cls_0x5a68d8);
  (this->cls_0x5a68d8).vftptr_0x0 =
       (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
  return this;
}



// Function at 004a3930

cls_0x5a3ee4 * __thiscall cls_0x5a3ee4::~cls_0x5a3ee4(cls_0x5a3ee4 *this)

{
  LPCVOID pvVar1;
  cls_0x5a3ee4 *pcVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059dde8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  (this->cls_0x5a68d8).vftptr_0x0 =
       (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
  local_4 = 0;
  if (((this->cls_0x5a68d8).mbr_0x68 != 0) &&
     (pvVar1 = (LPCVOID)(this->cls_0x5a68d8).mbr_0x64, pvVar1 != (LPCVOID)0x0)) {
    FUN_004830f0(pvVar1);
  }
  local_4 = 0xffffffff;
  pcVar2 = (cls_0x5a3ee4 *)cls_0x5a6858::meth_0x4bcb50((cls_0x5a6858 *)this);
  ExceptionList = local_c;
  return pcVar2;
}



