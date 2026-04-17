#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a495c



// Function at 0044b660

cls_0x5a495c * __thiscall cls_0x5a495c::~cls_0x5a495c(cls_0x5a495c *this)

{
  byte in_stack_00000004;
  
  this->vftptr_0x0 = &cls_0x5a495c__vftable_5a495c_005a495c;
  if (this->mbr_0x4 != 0) {
    this->mbr_0x4 = 0;
  }
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



