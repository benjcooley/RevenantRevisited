#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x46ec50



// Function at 0046ec50

void cls_0x46ec50::meth_0x46ec50()

{
  dword dVar1;
  dword dVar2;
  uint uVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  dword *in_stack_00000004;
  
  if (((this->mbr_0xc0 == this->mbr_0x10) && (this->mbr_0xc4 == this->mbr_0x14)) &&
     (this->mbr_0xc8 == this->mbr_0x18)) {
    dVar1 = this->mbr_0xd0;
    *in_stack_00000004 = this->mbr_0xcc;
    dVar2 = this->mbr_0xd4;
    in_stack_00000004[1] = dVar1;
    in_stack_00000004[2] = dVar2;
    return;
  }
  lVar4 = __ftol();
  this->mbr_0xcc = (dword)lVar4;
  lVar5 = __ftol();
  this->mbr_0xd0 = (dword)lVar5;
  lVar6 = __ftol();
  uVar3 = (uint)lVar6 & 0xffff;
  this->mbr_0xc0 = this->mbr_0x10;
  this->mbr_0xd4 = uVar3;
  *in_stack_00000004 = (dword)lVar4;
  in_stack_00000004[1] = (dword)lVar5;
  in_stack_00000004[2] = uVar3;
  this->mbr_0xc4 = this->mbr_0x14;
  this->mbr_0xc8 = this->mbr_0x18;
  return;
}



