#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x51fec0



// Function at 0051fec0

void cls_0x51fec0::meth_0x51fec0(undefined4 param_1)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)this->mbr_0x0) {
    do {
      piVar1 = *(int **)(this->mbr_0x10 + iVar2 * 4);
      if (piVar1 != (int *)0x0) {
        (**(code **)(*piVar1 + 0x10))(param_1);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x0);
  }
  return;
}



