#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4a1120



// Function at 004a1120

int cls_0x4a1120::meth_0x4a1120()

{
  byte *pbVar1;
  uint uVar2;
  int iVar3;
  byte *in_stack_00000004;
  
  iVar3 = 0;
  if (0 < (int)this->mbr_0x0) {
    do {
      pbVar1 = *(byte **)(this->mbr_0x10 + iVar3 * 4);
      if ((((pbVar1 != (byte *)0x0) && (*(uint *)(pbVar1 + 400) != 0)) && ((*pbVar1 & 1) != 0)) &&
         (uVar2 = FUN_0059a530_stricmp(*(uint *)(pbVar1 + 400),pbVar1 + 0x110,in_stack_00000004), uVar2 == 0
         )) {
        return iVar3;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)this->mbr_0x0);
  }
  return -1;
}



