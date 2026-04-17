#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x48c730



// Function at 0048c730

undefined4 cls_0x48c730::meth_0x48c730()

{
  uint uVar1;
  cls_0x48c730 *pcVar2;
  cls_0x48c730 *extraout_ECX;
  int iVar3;
  byte *in_stack_00000004;
  
  iVar3 = 0;
  pcVar2 = this;
  if (0 < (int)this->mbr_0x4) {
    do {
      uVar1 = FUN_0059a530((uint)pcVar2,*(byte **)(this->mbr_0x14 + iVar3 * 4),in_stack_00000004);
      if (uVar1 == 0) {
        return *(undefined4 *)(this->mbr_0x14 + iVar3 * 4);
      }
      iVar3 = iVar3 + 1;
      pcVar2 = extraout_ECX;
    } while (iVar3 < (int)this->mbr_0x4);
  }
  return 0;
}



