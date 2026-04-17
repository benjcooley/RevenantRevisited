#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x411e40



// Function at 00411e40

undefined4 cls_0x411e40::meth_0x411e40()

{
  if (this->mbr_0x54 != 0) {
    if (DAT_005d7a28 != 0) {
      FUN_0056d120((int)&this->mbr_0x4);
      this->mbr_0x54 = 0;
      return 1;
    }
    (**(code **)(*DAT_006699d0 + 0x38))(DAT_006699d0,this->mbr_0x0);
    this->mbr_0x54 = 0;
  }
  return 1;
}



