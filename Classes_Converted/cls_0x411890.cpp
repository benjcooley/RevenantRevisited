#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x411890



// Function at 00411890

void cls_0x411890::meth_0x411890()

{
  int *piVar1;
  
  if (this->mbr_0x54 != 0) {
    if (DAT_005d7a28 == 0) {
      (**(code **)(*DAT_006699d0 + 0x38))(DAT_006699d0,this->mbr_0x0);
    }
    else {
      FUN_0056d120((int)&this->mbr_0x4);
    }
    this->mbr_0x54 = 0;
  }
  piVar1 = (int *)this->mbr_0x0;
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 8))(piVar1);
    this->mbr_0x0 = 0;
  }
  return;
}



