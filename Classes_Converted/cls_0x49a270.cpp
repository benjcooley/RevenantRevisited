#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x49a270



// Function at 0049a270

undefined4 cls_0x49a270::meth_0x49a270()

{
  dword dVar1;
  undefined4 uVar2;
  
  if (DAT_00668114 != 0) {
    return 1;
  }
  uVar2 = 0;
  if (this->mbr_0x0 == 0) {
    dVar1 = _AIL_redbook_open_4(0);
    this->mbr_0x0 = dVar1;
    if (dVar1 == 0) goto LAB_0049a2b1;
  }
  uVar2 = 1;
  dVar1 = _AIL_redbook_volume_4(this->mbr_0x0);
  this->mbr_0xc = dVar1;
  this->mbr_0x4 = 0x60;
LAB_0049a2b1:
  this->mbr_0x40 = 0;
  this->mbr_0x3c = 0xffffffff;
  this->mbr_0x24 = 0;
  this->mbr_0x8 = 0;
  return uVar2;
}



