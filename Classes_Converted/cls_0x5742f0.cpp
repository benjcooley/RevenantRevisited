#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5742f0



// Function at 005742f0

void cls_0x5742f0::meth_0x5742f0()

{
  if ((HGLOBAL)this->mbr_0x38 != (HGLOBAL)0x0) {
    GlobalFree((HGLOBAL)this->mbr_0x38);
    this->mbr_0x38 = 0;
  }
  return;
}



