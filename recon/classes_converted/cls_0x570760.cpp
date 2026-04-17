#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x570760



// Function at 00570760

int cls_0x570760::meth_0x570760()

{
  int in_stack_00000004;
  
  if (((int)this->mbr_0x44 <= in_stack_00000004) && (in_stack_00000004 <= (int)this->mbr_0x48)) {
    return this->mbr_0x4c + (in_stack_00000004 % 0x32) * 0x68;
  }
  return 0;
}



