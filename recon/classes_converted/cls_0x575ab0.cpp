#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x575ab0



// Function at 00575ab0

void cls_0x575ab0::meth_0x575ab0()

{
  undefined4 *puVar1;
  int *piVar2;
  
  puVar1 = (undefined4 *)this->mbr_0x20;
  this->mbr_0xf0 = 0;
  this->mbr_0x28 = 0xffffffff;
  if (puVar1 != (undefined4 *)0x0) {
    puVar1[1] = *puVar1;
  }
  piVar2 = (int *)this->mbr_0x0;
  if (piVar2 != (int *)0x0) {
    (**(code **)(*piVar2 + 8))(piVar2);
    this->mbr_0x0 = 0;
  }
  return;
}



