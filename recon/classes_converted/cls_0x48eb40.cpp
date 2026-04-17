#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x48eb40



// Function at 0048eb40

undefined4 cls_0x48eb40::meth_0x48eb40()

{
  int iVar1;
  
  if ((int *)this->mbr_0x40 != (int *)0x0) {
    iVar1 = (**(code **)(*(int *)this->mbr_0x40 + 0x20))();
    if (iVar1 == 0) {
      return *(undefined4 *)(this->mbr_0x40 + 4);
    }
  }
  if ((int *)this->mbr_0x44 != (int *)0x0) {
    iVar1 = (**(code **)(*(int *)this->mbr_0x44 + 0x24))();
    if (iVar1 == 0) {
      return *(undefined4 *)(this->mbr_0x44 + 4);
    }
  }
  return 0;
}



