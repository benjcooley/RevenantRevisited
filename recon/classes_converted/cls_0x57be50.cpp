#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x57be50



// Function at 0057be50

void cls_0x57be50::meth_0x57be50()

{
  undefined4 *puVar1;
  dword dVar2;
  undefined4 *puVar3;
  dword dVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  dword in_stack_00000004;
  
  puVar3 = FUN_00482fb0(in_stack_00000004 * 4);
  if ((int)this->mbr_0xc <= (int)in_stack_00000004) {
    in_stack_00000004 = this->mbr_0xc;
  }
  puVar1 = (undefined4 *)this->mbr_0x0;
  puVar5 = puVar1;
  puVar6 = puVar3;
  for (; in_stack_00000004 != 0; in_stack_00000004 = in_stack_00000004 - 1) {
    *puVar6 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar6 = puVar6 + 1;
  }
  FUN_004830f0(puVar1);
  dVar2 = this->mbr_0x0;
  dVar4 = this->mbr_0xc + 8;
  this->mbr_0xc = dVar4;
  this->mbr_0x0 = (dword)puVar3;
  this->mbr_0x4 = (dword)(puVar3 + ((int)(this->mbr_0x4 - dVar2) >> 2));
  this->mbr_0x8 = (dword)(puVar3 + dVar4);
  return;
}



