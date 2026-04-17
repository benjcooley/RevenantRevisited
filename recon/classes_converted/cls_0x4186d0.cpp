#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4186d0



// Function at 004186d0

int cls_0x4186d0::meth_0x4186d0()

{
  int *piVar1;
  int iVar2;
  int in_stack_00000004;
  
  if (in_stack_00000004 < (int)this->mbr_0xc) {
    piVar1 = (int *)((int)this + in_stack_00000004 * 0x28 + 0x38);
    iVar2 = *piVar1;
    if (iVar2 != 0) {
      return (int)piVar1 + iVar2;
    }
  }
  return 0;
}



