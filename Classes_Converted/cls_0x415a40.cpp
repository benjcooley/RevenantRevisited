#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x415a40



// Function at 00415a40

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 cls_0x415a40::meth_0x415a40(int param_1)

{
  int **ppiVar1;
  undefined in_stack_00000008;
  undefined in_stack_00000009;
  ushort in_stack_0000000a;
  
  if ((((this->mbr_0x0 != 0) && (-1 < param_1)) && (param_1 < DAT_005e8860)) &&
     (ppiVar1 = *(int ***)(DAT_005e8870 + param_1 * 4), ppiVar1 != (int **)0x0)) {
    FUN_00411a60(ppiVar1,(int *)((float)(in_stack_0000000a & 0xff) * _DAT_005a3534),
                 (int *)((float)(CONCAT21(in_stack_0000000a,in_stack_00000009) & 0xff) *
                        _DAT_005a3534),
                 (int *)((float)(CONCAT11(in_stack_00000009,in_stack_00000008) & 0xff) *
                        _DAT_005a3534));
    return 1;
  }
  return 0;
}



