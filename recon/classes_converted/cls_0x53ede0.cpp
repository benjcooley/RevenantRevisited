#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x53ede0



// Function at 0053ede0

undefined4 cls_0x53ede0::meth_0x53ede0()

{
  uint uVar1;
  int *piVar2;
  uint uVar3;
  cls_0x53ede0 *extraout_ECX;
  cls_0x53ede0 *pcVar4;
  int iVar5;
  int iVar6;
  byte *in_stack_00000004;
  
  iVar6 = 0;
  pcVar4 = this;
  if (0 < (int)this->mbr_0x4) {
    do {
      uVar1 = FUN_0059a530((uint)pcVar4,(byte *)(*(int *)(this->mbr_0x14 + iVar6 * 4) + 0x1c),
                           in_stack_00000004);
      if (uVar1 == 0) {
        return *(undefined4 *)(this->mbr_0x14 + iVar6 * 4);
      }
      pcVar4 = (cls_0x53ede0 *)this->mbr_0x14;
      iVar5 = 0;
      piVar2 = *(int **)(&pcVar4->field_0x0 + iVar6 * 4);
      if (0 < *piVar2) {
        do {
          uVar1 = *(uint *)(piVar2[4] + iVar5 * 4);
          uVar3 = uVar1;
          if (uVar1 == 0) {
            uVar3 = piVar2[5];
          }
          uVar1 = FUN_0059a530(uVar1,(byte *)(uVar3 + 4),in_stack_00000004);
          if (uVar1 == 0) {
            return *(undefined4 *)(this->mbr_0x14 + iVar6 * 4);
          }
          iVar5 = iVar5 + 1;
          piVar2 = *(int **)(this->mbr_0x14 + iVar6 * 4);
          pcVar4 = extraout_ECX;
        } while (iVar5 < *piVar2);
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)this->mbr_0x4);
  }
  return 0;
}



