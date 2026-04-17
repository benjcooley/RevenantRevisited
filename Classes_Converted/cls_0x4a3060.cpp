#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4a3060



// Function at 004a3060

bool cls_0x4a3060::meth_0x4a3060(int param_1)

{
  dword dVar1;
  int in_stack_00000008;
  
  if ((((param_1 < 0) || (in_stack_00000008 < 0)) || (dVar1 = this->mbr_0x0, (int)dVar1 <= param_1))
     || ((int)this->mbr_0x4 <= in_stack_00000008)) {
    return false;
  }
  if ((this->mbr_0x10 & 6) != 0) {
    return (uint)*(ushort *)((int)&this[2].mbr_0x10 + (dVar1 * in_stack_00000008 + param_1) * 2) !=
           this->mbr_0x18;
  }
  if ((this->mbr_0x10 & 1) != 0) {
    return (uint)*(byte *)((int)&this[2].mbr_0x10 + dVar1 * in_stack_00000008 + param_1) !=
           this->mbr_0x18;
  }
  return true;
}



