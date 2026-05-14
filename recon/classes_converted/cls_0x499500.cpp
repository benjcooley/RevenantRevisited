#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x499500



// Function at 00499500

void cls_0x499500::meth_0x499500_TSector_InitLights()

{
  int *piVar1;
  int iVar2;
  
  if ((this->mbr_0x98 == 0) && (this->mbr_0xac == 0)) {
    for (iVar2 = 0; iVar2 < (int)this->mbr_0xfc; iVar2 = iVar2 + 1) {
      piVar1 = *(int **)(this->mbr_0x10c + iVar2 * 4);
      if (piVar1 == (int *)0x0) {
        piVar1 = (int *)this->mbr_0x110;
      }
      if (*(int *)(this->mbr_0xc8 + *piVar1 * 4) != 0) {
        FUN_00471b40();
      }
    }
    this->mbr_0xac = 1;
  }
  return;
}



