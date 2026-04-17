#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x40c6d0



// Function at 0040c6d0

undefined4 cls_0x40c6d0::meth_0x40c6d0()

{
  dword dVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = 0;
  if (0 < (int)this->mbr_0x4c) {
    do {
      if (((this->mbr_0x5c != 0) && (uVar3 < this->mbr_0x4c)) &&
         (*(int *)(this->mbr_0x5c + uVar3 * 4) != 0)) {
        dVar1 = *(dword *)(this->mbr_0x5c + uVar3 * 4);
        if (dVar1 == 0) {
          dVar1 = this->mbr_0x60;
        }
        if (*(int *)(dVar1 + 0x90) != 0) {
          dVar1 = *(dword *)(this->mbr_0x5c + uVar3 * 4);
          if (dVar1 == 0) {
            dVar1 = this->mbr_0x60;
          }
          iVar2 = (**(code **)(**(int **)(dVar1 + 0x90) + 0x60))(*(int **)(dVar1 + 0x90));
          if (iVar2 == -0x7789fe3e) {
            return 1;
          }
        }
      }
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < (int)this->mbr_0x4c);
  }
  return 0;
}



