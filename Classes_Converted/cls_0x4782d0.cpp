#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4782d0



// Function at 004782d0

undefined4 cls_0x4782d0::meth_0x4782d0()

{
  dword dVar1;
  
  this->mbr_0x4 = this->mbr_0x4 + 0x50;
  dVar1 = this->mbr_0x8 + 1;
  this->mbr_0x8 = dVar1;
  if (((short *)this->mbr_0x0 != (short *)0x0) && ((int)dVar1 < (int)*(short *)this->mbr_0x0)) {
    return 1;
  }
  return 0;
}



// Function at 00478300

undefined4 cls_0x4782d0::meth_0x478300()

{
  if (((short *)this->mbr_0x0 != (short *)0x0) &&
     (this->mbr_0x8 < (uint)(int)*(short *)this->mbr_0x0)) {
    return 1;
  }
  return 0;
}



// Function at 004785c0

void cls_0x4782d0::meth_0x4785c0()

{
  dword in_stack_00000004;
  
  this->mbr_0x8 = 0;
  this->mbr_0x0 = in_stack_00000004;
  this->mbr_0x4 = *(dword *)(in_stack_00000004 + 4);
  return;
}



