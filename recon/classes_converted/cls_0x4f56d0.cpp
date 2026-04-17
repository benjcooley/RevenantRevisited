#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4f56d0



// Function at 004f56d0

void cls_0x4f56d0::meth_0x4f56d0(uint param_1)

{
  int in_stack_00000008;
  
  if (in_stack_00000008 != 0) {
    (**(code **)(this->mbr_0x0 + 0x40))(this->mbr_0x8 | param_1);
    return;
  }
  (**(code **)(this->mbr_0x0 + 0x40))(this->mbr_0x8 & ~param_1);
  return;
}



