#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x445240



// Function at 00445240

undefined4 cls_0x445240::meth_0x445240()

{
  dword dVar1;
  dword dVar2;
  
  dVar1 = this->mbr_0x584;
  dVar2 = dVar1 + 1;
  this->mbr_0x584 = dVar2;
  if ((int)dVar2 < (int)this->mbr_0x57c) {
    return *(undefined4 *)(&this->field_0x180 + dVar1 * 4);
  }
  return 0xffffffff;
}



