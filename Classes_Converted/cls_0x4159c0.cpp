#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4159c0



// Function at 004159c0

undefined4 cls_0x4159c0::meth_0x4159c0(int param_1)

{
  int iVar1;
  int in_stack_00000008;
  
  if ((((this->mbr_0x0 != 0) && (-1 < param_1)) && (param_1 < DAT_005e8860)) &&
     (iVar1 = *(int *)(DAT_005e8870 + param_1 * 4), iVar1 != 0)) {
    *(float *)(iVar1 + 0x34) = (float)in_stack_00000008;
    *(int *)(iVar1 + 0x58) = in_stack_00000008;
    return 1;
  }
  return 0;
}



