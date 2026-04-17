#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x439530



// Function at 00439530

undefined4 cls_0x439530::meth_0x439530(int param_1)

{
  dword dVar1;
  uint uVar2;
  uint in_stack_00000008;
  
  if (this->mbr_0x0 != 0) {
    uVar2 = 0;
    if (0 < (int)this->mbr_0x4) {
      do {
        if (((this->mbr_0x14 != 0) && (uVar2 < this->mbr_0x4)) &&
           (*(int *)(this->mbr_0x14 + uVar2 * 4) != 0)) {
          dVar1 = *(dword *)(this->mbr_0x14 + uVar2 * 4);
          if (dVar1 == 0) {
            dVar1 = this->mbr_0x18;
          }
          if ((((*(int *)(dVar1 + 0x3c) == param_1) && (*(int *)(dVar1 + 0x48) != 0)) &&
              ((*(uint *)(dVar1 + 8) & in_stack_00000008) != 0)) ||
             (*(int *)(dVar1 + 0x40) == param_1)) {
            return 1;
          }
        }
        uVar2 = uVar2 + 1;
      } while ((int)uVar2 < (int)this->mbr_0x4);
    }
  }
  return 0;
}



