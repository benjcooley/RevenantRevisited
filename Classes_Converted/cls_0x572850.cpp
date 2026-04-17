#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x572850



// Function at 00572850

void cls_0x572850::meth_0x572850()

{
  byte bVar1;
  byte bVar2;
  byte *pbVar3;
  int in_stack_00000004;
  
  if ((-1 < in_stack_00000004) && (in_stack_00000004 < (int)this->mbr_0x1c)) {
    pbVar3 = (byte *)(this->mbr_0x38 +
                     ((int)(in_stack_00000004 + (in_stack_00000004 >> 0x1f & 7U)) >> 3));
    bVar2 = '\x01' << ((byte)in_stack_00000004 & 7);
    bVar1 = *pbVar3;
    if ((bVar2 & bVar1) != 0) {
      this->mbr_0x20 = this->mbr_0x20 - 1;
    }
    *pbVar3 = ~bVar2 & bVar1;
  }
  return;
}



