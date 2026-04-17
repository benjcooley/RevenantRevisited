#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a36e0



// Function at 0040d320

cls_0x5a36e0::cls_0x5a36e0()

{
  int iVar1;
  dword in_stack_00000004;
  
  iVar1 = DAT_005e8728;
  this->vftptr_0x0 = &cls_0x5a36e0__vftable_5a36e0_005a36e0;
  if (iVar1 < 0x80) {
    (&DAT_005e8304)[iVar1] = this;
    DAT_005e8728 = iVar1 + 1;
    this->mbr_0x4 = in_stack_00000004;
    return this;
  }
  this->mbr_0x4 = in_stack_00000004;
  return this;
}



// Function at 004113f0

uint cls_0x5a36e0::virt_meth_0x4113f0()

{
  uint uVar1;
  
  uVar1 = FUN_004a18a0((undefined4 *)this->mbr_0x4);
  if (uVar1 == 0xffffffff) {
    return 0xffffffff;
  }
  if ((this->mbr_0xc & 2) != 0) {
    return uVar1 ^ 0xcc;
  }
  if ((this->mbr_0xc & 1) == 0) {
    if ((uVar1 & 0x80) != 0) {
      this->mbr_0xc = 3;
      return uVar1 ^ 0xcc;
    }
    this->mbr_0xc = 1;
  }
  return uVar1;
}



// Function at 00411440

void cls_0x5a36e0::virt_meth_0x411440()

{
  FUN_004a16c0((undefined4 *)this->mbr_0x4,0,0);
  this->mbr_0xc = 0;
  return;
}



