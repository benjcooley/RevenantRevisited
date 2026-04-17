#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x56fc00



// Function at 0056fc00

cls_0x56fc00::cls_0x56fc00()

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  int iVar5;
  bool bVar6;
  
  bVar6 = DAT_005e5980 != '\0';
  this->mbr_0x0 = 0xffffffff;
  if (bVar6) {
    uVar4 = 0;
    uVar2 = 0;
    do {
      uVar4 = uVar4 | 1 << (0x1f - (&DAT_005b9cc8)[uVar2] & 0x1f);
      uVar2 = uVar2 + 1;
    } while (uVar2 < 0xe);
    uVar2 = 0;
    puVar3 = &DAT_00676264;
    do {
      iVar5 = 8;
      uVar1 = uVar2;
      do {
        if ((uVar1 & 1) == 0) {
          uVar1 = uVar1 >> 1;
        }
        else {
          uVar1 = uVar1 >> 1 ^ uVar4;
        }
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      *puVar3 = uVar1;
      puVar3 = puVar3 + 1;
      uVar2 = uVar2 + 1;
    } while ((int)puVar3 < 0x676664);
    DAT_005e5980 = '\0';
  }
  return this;
}



