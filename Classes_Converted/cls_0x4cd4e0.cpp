#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4cd4e0



// Function at 004cd4e0

undefined4 cls_0x4cd4e0::meth_0x4cd4e0()

{
  int iVar1;
  int iVar2;
  int *in_stack_00000004;
  
  if ((in_stack_00000004 == (int *)0x0) || ((*(byte *)(in_stack_00000004 + 2) & 0x80) == 0)) {
    iVar1 = (**(code **)(this->mbr_0x0 + 4))();
    iVar2 = (**(code **)(*in_stack_00000004 + 0x2fc))();
    if (iVar1 <= *(int *)(this->mbr_0xfc + 0x1b8)) {
      iVar1 = (**(code **)(this->mbr_0x0 + 0x2e0))(iVar1);
      if (100 - iVar1 < iVar2) {
        return 1;
      }
    }
  }
  return 0;
}



