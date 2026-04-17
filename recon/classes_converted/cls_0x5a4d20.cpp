#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a4d20



// Function at 0046d100

cls_0x5a4d20::cls_0x5a4d20()

{
  TObjectInstance::TObjectInstance(&this->TObjectInstance);
  this->mbr_0x70 = (dword)&cls_0x5a4c30__vftable_5a4c30_005a4c30;
  this->mbr_0x78 = 0;
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  this->mbr_0x74 = 0;
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&cls_0x5a4d20__vftable_5a4d20_005a4d20;
  return this;
}



// Function at 0046d130

cls_0x5a4d20 * cls_0x5a4d20::virt_meth_0x46d130()

{
  byte in_stack_00000004;
  
  TObjectInstance::~TObjectInstance(&this->TObjectInstance);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



