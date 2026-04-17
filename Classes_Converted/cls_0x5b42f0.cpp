#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b42f0



// Function at 00515290

cls_0x5b42f0 * __thiscall cls_0x5b42f0::~cls_0x5b42f0(cls_0x5b42f0 *this)

{
  cls_0x5b42f0 *pcVar1;
  
  this->vftptr_0x0 = &cls_0x5b42f0__vftable_5b42f0_005b42f0;
  pcVar1 = (cls_0x5b42f0 *)FUN_004830f0((LPCVOID)this->mbr_0x6c);
  return pcVar1;
}



// Function at 005152b0

cls_0x5b42f0 * __thiscall cls_0x5b42f0::~cls_0x5b42f0(cls_0x5b42f0 *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5b42f0__vftable_5b42f0_005b42f0;
  FUN_004830f0((LPCVOID)this->mbr_0x6c);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



