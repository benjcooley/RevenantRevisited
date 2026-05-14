#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4959f0



// Function at 004959f0

void cls_0x4959f0::meth_0x4959f0(byte *param_1)

{
  int *piVar1;
  uint uVar2;
  cls_0x4959f0 *pcVar3;
  cls_0x4959f0 *extraout_ECX;
  int iVar4;
  undefined4 in_stack_00000008;
  
  iVar4 = 0;
  pcVar3 = this;
  if (0 < (int)this->mbr_0x24) {
    while (uVar2 = FUN_0059a530_stricmp((uint)pcVar3,(byte *)(*(int *)(this->mbr_0x34 + iVar4 * 4) + 8),
                                param_1), uVar2 != 0) {
      iVar4 = iVar4 + 1;
      pcVar3 = extraout_ECX;
      if ((int)this->mbr_0x24 <= iVar4) {
        return;
      }
    }
    piVar1 = *(int **)(this->mbr_0x34 + iVar4 * 4);
    if (*piVar1 == 0) {
      *(undefined4 *)piVar1[1] = in_stack_00000008;
    }
  }
  return;
}



