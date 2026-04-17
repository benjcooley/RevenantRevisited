#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x587fb0



// Function at 00587fb0

void cls_0x587fb0::meth_0x587fb0()

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  
  piVar1 = (int *)this->mbr_0x0;
  iVar3 = *piVar1;
  while (iVar3 != 0) {
    iVar3 = *piVar1;
    if (iVar3 != 0) {
      piVar2 = (int *)piVar1[1];
      *piVar1 = 0;
      *(int **)(iVar3 + 4) = piVar2;
      *piVar2 = iVar3;
    }
    iVar3 = *piVar1;
  }
  return;
}



