#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a7dd8



// Function at 004da9c0

cls_0x5a7dd8 * cls_0x5a7dd8::virt_meth_0x4da9c0()

{
  byte in_stack_00000004;
  
  ~cls_0x5a7dd8(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004da9e0

cls_0x5a7dd8 * __thiscall cls_0x5a7dd8::~cls_0x5a7dd8(cls_0x5a7dd8 *this)

{
  cls_0x5a7e38 *pcVar1;
  
  this->vftptr_0x0 = &cls_0x5a7dd8__vftable_5a7dd8_005a7dd8;
  pcVar1 = cls_0x5a7e38::~cls_0x5a7e38((cls_0x5a7e38 *)this);
  return (cls_0x5a7dd8 *)pcVar1;
}



