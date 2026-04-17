#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x42e5b0



// Function at 0042e5b0

void cls_0x42e5b0::meth_0x42e5b0()

{
  int *in_stack_00000004;
  
  if ((((this->mbr_0x14 & 2) == 0) && (this->mbr_0x8 != 0)) && (this->mbr_0x10c != 0)) {
    (**(code **)(this->mbr_0x0 + 0x44))();
    (**(code **)(*in_stack_00000004 + 0x20))
              (this->mbr_0x158,this->mbr_0x15c,*(undefined4 *)this->mbr_0x10c,
               ((undefined4 *)this->mbr_0x10c)[1]);
  }
  return;
}



