#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4bc850



// Function at 004bc850

void cls_0x4bc850::meth_0x4bc850()

{
  dword dVar1;
  int iVar2;
  dword dVar3;
  int *in_stack_00000004;
  
  this->mbr_0x64 = (dword)in_stack_00000004;
  if (in_stack_00000004 != (int *)0x0) {
    dVar1 = in_stack_00000004[1];
    iVar2 = *in_stack_00000004;
    this->mbr_0x8 = in_stack_00000004[2];
    this->mbr_0x4 = dVar1;
    dVar3 = (**(code **)(iVar2 + 0x18))();
    dVar1 = in_stack_00000004[0xe];
    this->mbr_0xc = dVar3;
    dVar3 = in_stack_00000004[4];
    this->mbr_0x38 = dVar1;
    this->mbr_0x10 = dVar3;
    (**(code **)(this->mbr_0x0 + 0x24))();
    return;
  }
  this->mbr_0x4 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0xc = 0;
  this->mbr_0x10 = 0;
  this->mbr_0x38 = 0;
  return;
}



