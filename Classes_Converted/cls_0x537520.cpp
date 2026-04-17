#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x537520



// Function at 00537520

void cls_0x537520::meth_0x537520()

{
  uint uVar1;
  int *piVar2;
  int iVar3;
  dword dVar4;
  int in_stack_00000004;
  
  dVar4 = in_stack_00000004 * 3 + this->mbr_0x188;
  if ((int)dVar4 < 1) {
    dVar4 = 0;
    if (this->mbr_0x88 != 0) {
      uVar1 = (*(int **)this->mbr_0x98)[5] | 4;
      iVar3 = **(int **)this->mbr_0x98;
      goto LAB_005375c5;
    }
  }
  else {
    if ((int)dVar4 < 0xf3) {
      if (this->mbr_0x88 == 0) {
        piVar2 = (int *)0x0;
      }
      else {
        piVar2 = *(int **)this->mbr_0x98;
      }
      (**(code **)(*piVar2 + 0x1c))(piVar2[5] & 0xfffffffb);
      if (this->mbr_0x88 < 2) {
        piVar2 = (int *)0x0;
      }
      else {
        piVar2 = *(int **)(this->mbr_0x98 + 4);
      }
      iVar3 = *piVar2;
      uVar1 = piVar2[5] & 0xfffffffb;
      goto LAB_005375c5;
    }
    dVar4 = 0xf3;
    if (1 < this->mbr_0x88) {
      piVar2 = *(int **)(this->mbr_0x98 + 4);
      iVar3 = *piVar2;
      uVar1 = piVar2[5] | 4;
      goto LAB_005375c5;
    }
  }
  uVar1 = uRam00000014 | 4;
  iVar3 = iRam00000000;
LAB_005375c5:
  (**(code **)(iVar3 + 0x1c))(uVar1);
  if (dVar4 != this->mbr_0x188) {
    this->mbr_0x188 = dVar4;
    (**(code **)(this->mbr_0x0 + 0x28))();
  }
  return;
}



