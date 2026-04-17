#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5725a0



// Function at 005725a0

void cls_0x5725a0::meth_0x5725a0()

{
  dword dVar1;
  dword *pdVar2;
  
  dVar1 = this->mbr_0x0;
  if (dVar1 != 0) {
    pdVar2 = (dword *)this->mbr_0x4;
    this->mbr_0x0 = 0;
    *(dword **)(dVar1 + 4) = pdVar2;
    *pdVar2 = dVar1;
  }
  return;
}



