#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x40c310



// Function at 0040c310

dword cls_0x40c310::meth_0x40c310(uint param_1)

{
  dword dVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *in_stack_00000008;
  
  if (((((int)param_1 < 0) || ((int)this->mbr_0x4c <= (int)param_1)) ||
      (dVar1 = this->mbr_0x5c, dVar1 == 0)) ||
     ((this->mbr_0x4c <= param_1 || (*(int *)(dVar1 + param_1 * 4) == 0)))) {
    return 0;
  }
  if (in_stack_00000008 != (undefined4 *)0x0) {
    puVar3 = *(undefined4 **)(dVar1 + param_1 * 4);
    if (puVar3 == (undefined4 *)0x0) {
      puVar3 = (undefined4 *)this->mbr_0x60;
    }
    for (iVar2 = 0x26; iVar2 != 0; iVar2 = iVar2 + -1) {
      *in_stack_00000008 = *puVar3;
      puVar3 = puVar3 + 1;
      in_stack_00000008 = in_stack_00000008 + 1;
    }
  }
  dVar1 = *(dword *)(this->mbr_0x5c + param_1 * 4);
  if (dVar1 != 0) {
    return dVar1;
  }
  return this->mbr_0x60;
}



