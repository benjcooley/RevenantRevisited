#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4348b0



// Function at 004348b0

void cls_0x4348b0::meth_0x4348b0()

{
  int *piVar1;
  dword dVar2;
  uint uVar3;
  
  if (((((this->mbr_0x14 & 4) == 0) && (this->mbr_0x8 != 0)) && ((this->mbr_0x14 & 2) == 0)) &&
     (0 < (int)this->mbr_0xbc)) {
    (**(code **)(*(int *)this->mbr_0xc4 + 0x1c))(((int *)this->mbr_0xc4)[5] & 0xfffffffd);
    piVar1 = (int *)this->mbr_0xc4;
    dVar2 = this->mbr_0x8;
    uVar3 = piVar1[5];
    *(int **)(dVar2 + 0xa8) = piVar1;
    *(undefined4 *)(dVar2 + 0xac) = 1;
    (**(code **)(*piVar1 + 0x1c))(uVar3 | 0x20);
    this->mbr_0x94 = 1;
  }
  return;
}



