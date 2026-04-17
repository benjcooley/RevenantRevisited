#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4e2fa0



// Function at 004e2fa0

void cls_0x4e2fa0::meth_0x4e2fa0()

{
  dword in_stack_00000004;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  this->mbr_0x184 = in_stack_00000004;
  if (in_stack_00000004 != 0) {
    local_c = *(undefined4 *)(in_stack_00000004 + 0x10);
    local_8 = *(undefined4 *)(in_stack_00000004 + 0x14);
    local_4 = *(undefined4 *)(in_stack_00000004 + 0x18);
    (**(code **)(this->mbr_0x0 + 8))(&local_c,0xffffffff,0);
  }
  return;
}



