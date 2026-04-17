#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a4be4



// Function at 0046cef0

cls_0x5a4be4::cls_0x5a4be4()

{
  TObjectInstance::TObjectInstance(&this->TObjectInstance);
  this->mbr_0x70 = (dword)&cls_0x5a4c30__vftable_5a4c30_005a4c30;
  this->mbr_0x78 = 0;
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  this->mbr_0x74 = 0;
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&cls_0x5a4be4__vftable_5a4be4_005a4be4;
  return this;
}



// Function at 0046cff0

cls_0x5a4be4 * cls_0x5a4be4::virt_meth_0x46cff0()

{
  byte in_stack_00000004;
  
  TObjectInstance::~TObjectInstance(&this->TObjectInstance);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



