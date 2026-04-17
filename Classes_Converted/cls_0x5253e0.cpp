#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5253e0



// Function at 005253e0

void cls_0x5253e0::meth_0x5253e0()

{
  uint uVar1;
  
  uVar1 = FUN_0059a530((uint)this,(byte *)this->mbr_0x38,(byte *)s_DunFireWallS_005e2fc4);
  if (uVar1 == 0) {
    this->mbr_0xfc = 0x80;
  }
  else {
    this->mbr_0xfc = 0x40;
  }
  this->mbr_0xdc = 0xffffffff;
  this->mbr_0xd8 = 0;
  this->mbr_0xe0 = 0;
  this->mbr_0x8 = this->mbr_0x8 | 0x40000;
  return;
}



