#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x40e110



// Function at 0040e110

void cls_0x40e110::meth_0x40e110(int *param_1, uint param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int in_stack_0000000c;
  
  if (param_1 != (int *)0x0) {
    if ((int)param_2 < 0) {
      uVar3 = 0;
      iVar2 = 0;
      while( true ) {
        if ((int *)param_1[0x15] == (int *)0x0) {
          iVar1 = 1;
        }
        else {
          iVar1 = (**(code **)(*(int *)param_1[0x15] + 0x3c))();
        }
        if (iVar1 <= (int)uVar3) break;
        iVar1 = *(int *)(*(int *)(this->mbr_0x8 + 4) + 0x54);
        if (uVar3 < *(uint *)(iVar1 + 4)) {
          iVar1 = iVar1 + 8 + iVar2;
        }
        else {
          iVar1 = 0;
        }
        uVar3 = uVar3 + 1;
        *(undefined2 *)(iVar1 + 0x32) = 0;
        *(undefined2 *)(iVar1 + 0x30) = 0;
        *(undefined2 *)(iVar1 + 0x2e) = 0;
        *(undefined2 *)(iVar1 + 0x2c) = 0;
        iVar2 = iVar2 + 0x4c;
      }
      this->mbr_0x3c = this->mbr_0x3c | 4;
      param_2 = 0;
    }
    else {
      iVar2 = *(int *)(*(int *)(this->mbr_0x8 + 4) + 0x54);
      if (param_2 < *(uint *)(iVar2 + 4)) {
        iVar2 = iVar2 + 8 + param_2 * 0x4c;
      }
      else {
        iVar2 = 0;
      }
      *(undefined2 *)(iVar2 + 0x32) = 0;
      *(undefined2 *)(iVar2 + 0x30) = 0;
      *(undefined2 *)(iVar2 + 0x2e) = 0;
      *(undefined2 *)(iVar2 + 0x2c) = 0;
    }
    uVar3 = this->mbr_0x3c;
    this->mbr_0x3c = uVar3 | 2;
    if (in_stack_0000000c == 0) {
      *(undefined *)((int)param_1 + 0x36) = 0x20;
      param_1[0x2c] = 0x20;
    }
    else {
      *(undefined *)((int)param_1 + 0x36) = 0;
      param_1[0x2c] = 0;
      this->mbr_0x3c = uVar3 | 10;
    }
    (**(code **)(*param_1 + 0x18))(param_2);
    DAT_006680b0 = 1;
    DAT_006682a8 = 1;
    DAT_006682c4 = 1;
  }
  return;
}



