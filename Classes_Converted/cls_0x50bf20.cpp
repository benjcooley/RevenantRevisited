#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x50bf20



// Function at 0050bf20

void cls_0x50bf20::meth_0x50bf20()

{
  dword dVar1;
  int iVar2;
  dword dVar3;
  undefined4 *in_stack_00000004;
  
  if ((int)this->mbr_0x18 < (int)this->mbr_0x14) {
    *(undefined4 *)(this->mbr_0x1c + this->mbr_0x18 * 0xc) = *in_stack_00000004;
    *(undefined4 *)(this->mbr_0x1c + 4 + this->mbr_0x18 * 0xc) = in_stack_00000004[1];
    *(undefined4 *)(this->mbr_0x1c + 8 + this->mbr_0x18 * 0xc) = in_stack_00000004[2];
    *(float *)(*(int *)(this->mbr_0x0 + 0xa4) + 8 + this->mbr_0x18 * 0x40) =
         (float)((int)this->mbr_0xc / 2);
    *(undefined4 *)(*(int *)(this->mbr_0x0 + 0xa4) + 0x1c + this->mbr_0x18 * 0x40) = 0;
    *(undefined4 *)(*(int *)(this->mbr_0x0 + 0xa4) + 0x18 + this->mbr_0x18 * 0x40) = 0x3f800000;
    *(float *)(*(int *)(this->mbr_0x0 + 0xa4) + 0x28 + this->mbr_0x18 * 0x40) =
         (float)-((int)this->mbr_0xc / 2);
    *(undefined4 *)(*(int *)(this->mbr_0x0 + 0xa4) + 0x3c + this->mbr_0x18 * 0x40) = 0x3f800000;
    *(undefined4 *)(*(int *)(this->mbr_0x0 + 0xa4) + 0x38 + this->mbr_0x18 * 0x40) = 0x3f800000;
    dVar3 = this->mbr_0x18 + 1;
    this->mbr_0x18 = dVar3;
    if ((int)this->mbr_0x4 <= (int)this->mbr_0x8) {
      this->mbr_0x8 = this->mbr_0x8 + 1;
    }
    dVar1 = this->mbr_0x0;
    iVar2 = dVar3 * 2;
    *(int *)(dVar1 + 0xa0) = iVar2;
    iVar2 = iVar2 + -2;
    *(int *)(dVar1 + 0xa8) = iVar2;
    *(int *)(dVar1 + 0x138) = iVar2;
  }
  return;
}



// Function at 0050c070

void cls_0x50bf20::meth_0x50c070()

{
  dword dVar1;
  dword dVar2;
  undefined4 *puVar3;
  int iVar4;
  
  if (0 < (int)this->mbr_0x18) {
    dVar2 = this->mbr_0x18 - 1;
    this->mbr_0x18 = dVar2;
    if (this->mbr_0x4 == dVar2) {
      this->mbr_0x4 = this->mbr_0x4 - 1;
    }
    if (this->mbr_0x8 == dVar2) {
      this->mbr_0x8 = this->mbr_0x8 - 1;
    }
    dVar1 = this->mbr_0x0;
    *(dword *)(dVar1 + 0xa0) = dVar2 * 2;
    iVar4 = dVar2 * 2 + -2;
    *(int *)(dVar1 + 0xa8) = iVar4;
    *(int *)(dVar1 + 0x138) = iVar4;
    if (0 < (int)dVar2) {
      puVar3 = (undefined4 *)this->mbr_0x1c;
      do {
        dVar2 = dVar2 - 1;
        *puVar3 = puVar3[3];
        puVar3[1] = puVar3[4];
        puVar3[2] = puVar3[5];
        puVar3 = puVar3 + 3;
      } while (dVar2 != 0);
    }
  }
  return;
}



