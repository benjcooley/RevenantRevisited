#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b4ecc



// Function at 005175d0

cls_0x5b4ecc * cls_0x5b4ecc::virt_meth_0x5175d0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b4ecc(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 005175f0

cls_0x5b4ecc * __thiscall cls_0x5b4ecc::~cls_0x5b4ecc(cls_0x5b4ecc *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1098;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b4ecc__vftable_5b4ecc_005b4ecc;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b4ecc *)pcVar1;
}



// Function at 00520930

cls_0x5b4f30 * cls_0x5b4ecc::virt_meth_0x520930(undefined4 param_1)

{
  cls_0x5b4f30 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a127b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  pcVar1 = (cls_0x5b4f30 *)FUN_00482fb0(0x674);
  local_4 = 0;
  if (pcVar1 != (cls_0x5b4f30 *)0x0) {
    pcVar1 = cls_0x5b4f30::cls_0x5b4f30(pcVar1,param_1);
    ExceptionList = local_c;
    return pcVar1;
  }
  ExceptionList = local_c;
  return (cls_0x5b4f30 *)0x0;
}



