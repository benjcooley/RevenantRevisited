#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x48c780



// Function at 0048c780

dword cls_0x48c780::meth_0x48c780(int param_1)

{
  int iVar1;
  int *piVar2;
  int in_stack_00000008;
  
  if (this->mbr_0x0 == 0) {
    return 0;
  }
  iVar1 = 0;
  if (0 < (int)this->mbr_0x18) {
    piVar2 = (int *)this->mbr_0x28;
    do {
      if ((*(int *)(*piVar2 + 0xc0) == param_1) && (*(int *)(*piVar2 + 0xc4) == in_stack_00000008))
      {
        return ((int *)this->mbr_0x28)[iVar1];
      }
      iVar1 = iVar1 + 1;
      piVar2 = piVar2 + 1;
    } while (iVar1 < (int)this->mbr_0x18);
  }
  return this->mbr_0x54;
}



