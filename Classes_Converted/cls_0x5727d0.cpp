#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5727d0



// Function at 005727d0

uint cls_0x5727d0::meth_0x5727d0()

{
  dword dVar1;
  dword dVar2;
  byte *pbVar3;
  int iVar4;
  byte bVar5;
  bool bVar6;
  int *in_stack_00000004;
  
  if ((*in_stack_00000004 < 0) || ((int)this->mbr_0x1c <= *in_stack_00000004)) {
    *in_stack_00000004 = 0;
  }
  dVar1 = this->mbr_0x38;
  dVar2 = this->mbr_0x1c;
  do {
    iVar4 = *in_stack_00000004;
    bVar6 = iVar4 != 0;
    pbVar3 = (byte *)(((int)(iVar4 + (iVar4 >> 0x1f & 7U)) >> 3) + dVar1);
    bVar5 = '\x01' << ((byte)iVar4 & 7);
    while (iVar4 < (int)dVar2) {
      if ((*pbVar3 & bVar5) != 0) {
        return CONCAT31((int3)((uint)pbVar3 >> 8),1);
      }
      iVar4 = *in_stack_00000004 + 1;
      bVar5 = bVar5 << 1;
      *in_stack_00000004 = iVar4;
      if (bVar5 == 0) {
        pbVar3 = pbVar3 + 1;
        bVar5 = 1;
      }
    }
    *in_stack_00000004 = 0;
  } while (bVar6);
  return (uint)pbVar3 & 0xffffff00;
}



