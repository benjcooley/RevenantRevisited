#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x51e5c0



// Function at 0051e5c0

int cls_0x51e5c0::meth_0x51e5c0()

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = 0;
  iVar3 = 0;
  do {
    iVar1 = (**(code **)(this->mbr_0x0 + 0xdc))(iVar3 + 0x28);
    iVar2 = iVar2 + iVar1;
    iVar3 = iVar3 + 1;
  } while (iVar3 < 0xb);
  return iVar2;
}



