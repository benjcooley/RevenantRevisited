#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x573e20



// Function at 00573e20

void cls_0x573e20::meth_0x573e20()

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



