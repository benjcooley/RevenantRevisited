#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x44ed90



// Function at 0044ed90

void cls_0x44ed90::meth_0x44ed90()

{
  int local_18;
  undefined4 local_14;
  undefined4 local_10;
  int local_c [3];
  
  local_18 = *(int *)(DAT_00667fcc + 0x10);
  local_14 = *(undefined4 *)(DAT_00667fcc + 0x14);
  local_10 = *(undefined4 *)(DAT_00667fcc + 0x18);
  FUN_0046dad0((this->mbr_0x78 - this->mbr_0x4) + DAT_00668510,
               (this->mbr_0x7c - this->mbr_0x8) + DAT_00668514,local_c,
               *(int *)(DAT_00667fcc + 0x18) + 0x32);
  FUN_0046dc60_AngleToPP(&local_18,local_c);
  return;
}



