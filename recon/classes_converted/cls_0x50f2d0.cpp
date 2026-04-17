#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x50f2d0



// Function at 0050f2d0

void cls_0x50f2d0::meth_0x50f2d0()

{
  dword dVar1;
  dword *in_stack_00000004;
  
  this->mbr_0xd8 = *in_stack_00000004;
  dVar1 = in_stack_00000004[2];
  this->mbr_0xdc = in_stack_00000004[1];
  this->mbr_0xe0 = dVar1;
  this->mbr_0xe4 = this->mbr_0xd8 - this->mbr_0x10;
  this->mbr_0xe8 = this->mbr_0xdc - this->mbr_0x14;
  this->mbr_0xec = this->mbr_0xe0 - this->mbr_0x18;
  cls_0x497c40::meth_0x497c40((cls_0x497c40 *)&DAT_0065def0);
  return;
}



