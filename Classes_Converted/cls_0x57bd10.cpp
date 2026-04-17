#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x57bd10



// Function at 0057bd10

void cls_0x57bd10::meth_0x57bd10()

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  dword dVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 in_stack_00000004;
  
  if (this->mbr_0x4 == this->mbr_0x8) {
    dVar3 = this->mbr_0xc + 8;
    puVar2 = FUN_00482fb0(dVar3 * 4);
    if ((int)this->mbr_0xc <= (int)dVar3) {
      dVar3 = this->mbr_0xc;
    }
    puVar1 = (undefined4 *)this->mbr_0x0;
    puVar4 = puVar1;
    puVar5 = puVar2;
    for (; dVar3 != 0; dVar3 = dVar3 - 1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar5 = puVar5 + 1;
    }
    FUN_004830f0(puVar1);
    dVar3 = this->mbr_0x0;
    this->mbr_0x0 = (dword)puVar2;
    this->mbr_0x4 = (dword)(puVar2 + ((int)(this->mbr_0x4 - dVar3) >> 2));
    dVar3 = this->mbr_0xc + 8;
    this->mbr_0xc = dVar3;
    this->mbr_0x8 = (dword)(puVar2 + dVar3);
  }
  puVar2 = (undefined4 *)this->mbr_0x4;
  *puVar2 = in_stack_00000004;
  this->mbr_0x4 = (dword)(puVar2 + 1);
  return;
}



