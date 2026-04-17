#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x40b770



// Function at 0040b770

dword cls_0x40b770::meth_0x40b770(uint param_1)

{
  dword dVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *in_stack_00000008;
  
  if (((((int)param_1 < 0) || ((int)this->mbr_0x34 <= (int)param_1)) ||
      (dVar1 = this->mbr_0x44, dVar1 == 0)) ||
     ((this->mbr_0x34 <= param_1 || (*(int *)(dVar1 + param_1 * 4) == 0)))) {
    return 0;
  }
  if (in_stack_00000008 != (undefined4 *)0x0) {
    puVar3 = *(undefined4 **)(dVar1 + param_1 * 4);
    if (puVar3 == (undefined4 *)0x0) {
      puVar3 = (undefined4 *)this->mbr_0x48;
    }
    for (iVar2 = 0x17; iVar2 != 0; iVar2 = iVar2 + -1) {
      *in_stack_00000008 = *puVar3;
      puVar3 = puVar3 + 1;
      in_stack_00000008 = in_stack_00000008 + 1;
    }
  }
  dVar1 = *(dword *)(this->mbr_0x44 + param_1 * 4);
  if (dVar1 != 0) {
    return dVar1;
  }
  return this->mbr_0x48;
}



// Function at 0040b830

void cls_0x40b770::meth_0x40b830(uint param_1)

{
  int *piVar1;
  int iVar2;
  dword dVar3;
  undefined4 *puVar4;
  undefined4 *in_stack_00000008;
  
  if ((((-1 < (int)param_1) && ((int)param_1 < (int)this->mbr_0x34)) &&
      (dVar3 = this->mbr_0x44, dVar3 != 0)) &&
     ((param_1 < this->mbr_0x34 && (*(int *)(dVar3 + param_1 * 4) != 0)))) {
    puVar4 = *(undefined4 **)(dVar3 + param_1 * 4);
    if (puVar4 == (undefined4 *)0x0) {
      puVar4 = (undefined4 *)this->mbr_0x48;
    }
    for (iVar2 = 0x14; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *in_stack_00000008;
      in_stack_00000008 = in_stack_00000008 + 1;
      puVar4 = puVar4 + 1;
    }
    dVar3 = *(dword *)(this->mbr_0x44 + param_1 * 4);
    if (dVar3 == 0) {
      dVar3 = this->mbr_0x48;
    }
    *(undefined4 *)(dVar3 + 0x48) = 0;
    dVar3 = *(dword *)(this->mbr_0x44 + param_1 * 4);
    if (dVar3 == 0) {
      dVar3 = this->mbr_0x48;
    }
    iVar2 = *(int *)(this->mbr_0x44 + param_1 * 4);
    if (iVar2 != 0) {
      piVar1 = *(int **)(iVar2 + 0x50);
      (**(code **)(*piVar1 + 0xc))(piVar1,dVar3);
      return;
    }
    piVar1 = *(int **)(this->mbr_0x48 + 0x50);
    (**(code **)(*piVar1 + 0xc))(piVar1,dVar3);
  }
  return;
}



