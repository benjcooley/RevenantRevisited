#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4390a0



// Function at 004390a0

undefined4 cls_0x4390a0::meth_0x4390a0(byte *param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *in_stack_00000008;
  
  if ((this->mbr_0x0 != 0) && (iVar2 = 0, 0 < (int)this->mbr_0x4)) {
    do {
      puVar3 = *(undefined4 **)(this->mbr_0x14 + iVar2 * 4);
      if (puVar3 == (undefined4 *)0x0) {
        puVar3 = (undefined4 *)this->mbr_0x18;
      }
      if ((puVar3 != (undefined4 *)0x0) &&
         (uVar1 = FUN_0059a530_stricmp(puVar3[1],(byte *)puVar3[1],param_1), uVar1 == 0)) {
        for (iVar2 = 0x13; iVar2 != 0; iVar2 = iVar2 + -1) {
          *in_stack_00000008 = *puVar3;
          puVar3 = puVar3 + 1;
          in_stack_00000008 = in_stack_00000008 + 1;
        }
        return 1;
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)this->mbr_0x4);
  }
  return 0;
}



