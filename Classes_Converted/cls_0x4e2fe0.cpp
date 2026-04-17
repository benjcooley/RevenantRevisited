#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4e2fe0



// Function at 004e2fe0

void cls_0x4e2fe0::meth_0x4e2fe0()

{
  int *piVar1;
  
  piVar1 = (int *)(**(code **)(this->mbr_0x0 + 0x24))();
  if (piVar1 != (int *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x004e2fed. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*piVar1 + 0x60))();
    return;
  }
  return;
}



