#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x50ad90



// Function at 0050ad90

int cls_0x50ad90::meth_0x50ad90()

{
  int iVar1;
  int *piVar2;
  dword dVar3;
  
  dVar3 = this->mbr_0x84;
  iVar1 = 0;
  if (0 < (int)dVar3) {
    piVar2 = (int *)this->mbr_0x80;
    do {
      if (*piVar2 != 0) {
        iVar1 = iVar1 + 1;
      }
      piVar2 = piVar2 + 0x12;
      dVar3 = dVar3 - 1;
    } while (dVar3 != 0);
  }
  return iVar1;
}



