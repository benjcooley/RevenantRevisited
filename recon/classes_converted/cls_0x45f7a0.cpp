#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x45f7a0



// Function at 0045f7a0

undefined4 cls_0x45f7a0::meth_0x45f7a0()

{
  if (((uint *)this->mbr_0x0 != (uint *)0x0) && (this->mbr_0x8 < *(uint *)this->mbr_0x0)) {
    return 1;
  }
  return 0;
}



// Function at 0045f7e0

void cls_0x45f7a0::meth_0x45f7e0()

{
  dword in_stack_00000004;
  
  this->mbr_0x8 = 0;
  this->mbr_0x0 = in_stack_00000004;
  this->mbr_0x4 = *(dword *)(in_stack_00000004 + 0x10);
  return;
}



