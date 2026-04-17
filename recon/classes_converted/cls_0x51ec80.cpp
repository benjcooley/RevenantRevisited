#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x51ec80



// Function at 0051ec80

undefined4 cls_0x51ec80::meth_0x51ec80()

{
  dword *this_00;
  LPCVOID pvVar1;
  uint uVar2;
  int iVar3;
  undefined4 *puVar4;
  
  if ((undefined4 *)this->mbr_0x10 != (undefined4 *)0x0) {
    puVar4 = (undefined4 *)this->mbr_0x10;
    for (uVar2 = this->mbr_0x8 & 0x3fffffff; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar4 = 0;
      puVar4 = puVar4 + 1;
    }
    for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
      *(undefined *)puVar4 = 0;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
    }
  }
  this_00 = &this->mbr_0x14;
  this->mbr_0x0 = 0;
  iVar3 = 0;
  this->mbr_0x4 = 0;
  if (0 < (int)*this_00) {
    do {
      if ((-1 < iVar3) &&
         (pvVar1 = *(LPCVOID *)(this->mbr_0x24 + iVar3 * 4), pvVar1 != (LPCVOID)0x0)) {
        FUN_004830f0(pvVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar3 = iVar3 + 1;
    } while (iVar3 < (int)*this_00);
  }
  *this_00 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x28 = 0xffffffff;
  DAT_00667fcc = 0;
  return 1;
}



