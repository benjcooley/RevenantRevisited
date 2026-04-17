#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a96d8



// Function at 004f6e50

cls_0x5a96d8 * __thiscall cls_0x5a96d8::~cls_0x5a96d8(cls_0x5a96d8 *this)

{
  cls_0x5a96d8 *pcVar1;
  
  this->vftptr_0x0 = &cls_0x5a96d8__vftable_5a96d8_005a96d8;
  pcVar1 = (cls_0x5a96d8 *)FUN_004830f0((LPCVOID)this->mbr_0x4);
  return pcVar1;
}



// Function at 004f6e70

cls_0x5a96d8 * __thiscall cls_0x5a96d8::~cls_0x5a96d8(cls_0x5a96d8 *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0((LPCVOID)this->mbr_0x4);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



