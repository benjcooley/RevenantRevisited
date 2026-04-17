#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x579280



// Function at 00579280

void cls_0x579280::meth_0x579280()

{
  int in_stack_00000004;
  
  WaitForSingleObject((HANDLE)this->mbr_0x12c,0xffffffff);
  if (*(int *)(in_stack_00000004 + 0x18) == 1) {
    (**(code **)(*(int *)this->mbr_0x0 + 0xcc))
              ((int *)this->mbr_0x0,*(undefined4 *)(in_stack_00000004 + 0x14),0);
    *(undefined4 *)(in_stack_00000004 + 0x18) = 3;
  }
  ReleaseMutex((HANDLE)this->mbr_0x12c);
  return;
}



