#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x51f330



// Function at 0051f330

int cls_0x51f330::meth_0x51f330()

{
  int *piVar1;
  int iVar2;
  int in_stack_00000004;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x0) {
    piVar1 = (int *)this->mbr_0x10;
    do {
      if ((*piVar1 != 0) && (*(int *)(*piVar1 + 0x40) == in_stack_00000004)) {
        return ((int *)this->mbr_0x10)[iVar2];
      }
      iVar2 = iVar2 + 1;
      piVar1 = piVar1 + 1;
    } while (iVar2 < (int)this->mbr_0x0);
  }
  return 0;
}



