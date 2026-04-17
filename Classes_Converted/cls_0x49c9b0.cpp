#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x49c9b0



// Function at 0049c9b0

void cls_0x49c9b0::meth_0x49c9b0()

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  
  puVar1 = FUN_00482fb0(0x18);
  this->mbr_0x0 = (dword)puVar1;
  iVar3 = 0;
  do {
    puVar1 = FUN_00482fb0(0x7c);
    *(undefined4 **)(iVar3 + this->mbr_0x0) = puVar1;
    iVar3 = iVar3 + 4;
    for (iVar2 = 0x1f; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar1 = 0xffe17b80;
      puVar1 = puVar1 + 1;
    }
  } while (iVar3 < 0x18);
  return;
}



