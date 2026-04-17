#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x51da00



// Function at 0051da00

int cls_0x51da00::meth_0x51da00()

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = 0;
  iVar3 = 0;
  do {
    iVar1 = (**(code **)(this->mbr_0x0 + 0xdc))(iVar3 + 0x22);
    iVar2 = iVar2 + iVar1;
    iVar3 = iVar3 + 1;
  } while (iVar3 < 6);
  return iVar2;
}



