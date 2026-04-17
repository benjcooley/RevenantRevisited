#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b9538



// Function at 00539360

void cls_0x5b9538::virt_meth_0x539360()

{
  cls_0x5a4f28::meth_0x48ee10((cls_0x5a4f28 *)this);
  (**(code **)(*(int *)this->mbr_0x88 + 4))();
  return;
}



// Function at 00539f60

cls_0x5b9538::cls_0x5b9538()

{
  TObjectInstance::TObjectInstance(&this->TObjectInstance);
  this->mbr_0x70 = (dword)&cls_0x5a4c30__vftable_5a4c30_005a4c30;
  this->mbr_0x78 = 0;
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  this->mbr_0x74 = 0;
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&cls_0x5b9538__vftable_5b9538_005b9538;
  return this;
}



// Function at 00539f90

cls_0x5b9538 * __thiscall cls_0x5b9538::~cls_0x5b9538(cls_0x5b9538 *this)

{
  cls_0x5b9538 *pcVar1;
  
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&cls_0x5b9538__vftable_5b9538_005b9538;
  pcVar1 = (cls_0x5b9538 *)TObjectInstance::~TObjectInstance(&this->TObjectInstance);
  return pcVar1;
}



// Function at 00539fa0

cls_0x5b9538 * cls_0x5b9538::virt_meth_0x539fa0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b9538(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



