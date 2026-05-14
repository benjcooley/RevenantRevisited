#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x496e20



// Function at 00496e20

void cls_0x496e20::meth_0x496e20()

{
  cls_0x4922c0 *this_00;
  uint uVar1;
  
  uVar1 = 0;
  if (0 < (int)this->mbr_0x8018) {
    do {
      if (((this->mbr_0x8028 != 0) && (uVar1 < this->mbr_0x8018)) &&
         (*(int *)(this->mbr_0x8028 + uVar1 * 4) != 0)) {
        this_00 = *(cls_0x4922c0 **)(this->mbr_0x8028 + uVar1 * 4);
        if (this_00->mbr_0x48 != 0) {
          cls_0x4922c0::meth_0x493e40_TScript_End(this_00);
        }
        this_00->mbr_0x48 = 0;
        this_00->mbr_0x8 = this_00->mbr_0x4;
        this_00->mbr_0x4c = 0;
        this_00->mbr_0x50 = 0;
        this_00->mbr_0x10 = 0xffffffff;
        this_00->mbr_0xb4 = 0;
        this_00->mbr_0xb5 = 0;
        this_00->mbr_0xc0 = 0;
        if ((LPCVOID)this_00->mbr_0xb8 != (LPCVOID)0x0) {
          FUN_004830f0((LPCVOID)this_00->mbr_0xb8);
          this_00->mbr_0xb8 = 0;
        }
        this_00->mbr_0xa4 = 0;
      }
      uVar1 = uVar1 + 1;
    } while ((int)uVar1 < (int)this->mbr_0x8018);
  }
  return;
}



