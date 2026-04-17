#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a89b4



// Function at 004df3a0

void cls_0x5a89b4::virt_meth_0x4df3a0()

{
  dword *pdVar1;
  int iVar2;
  
  this->virt_meth_0x40dd60();
  pdVar1 = &this->mbr_0x1b4;
  iVar2 = 0xf;
  do {
    pdVar1[1] = 0;
    *pdVar1 = 0;
    pdVar1[-1] = 0;
    pdVar1[-0x2c] = 0;
    pdVar1[-0x2d] = 0;
    pdVar1[-0x2e] = 0;
    pdVar1 = pdVar1 + 3;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  cls_0x40b770::meth_0x40b770((cls_0x40b770 *)this->mbr_0x8,0);
  this->mbr_0x26c = 0;
  this->mbr_0x278 = 0;
  this->mbr_0x27c = 0;
  this->mbr_0x280 = 0;
  this->mbr_0x284 = 0;
  this->mbr_0x268 = 0;
  this->mbr_0x270 = 0;
  this->mbr_0x274 = 0;
  this->mbr_0x288 = 0;
  this->mbr_0x28c = 0;
  this->mbr_0x290 = 0;
  this->mbr_0x294 = 0;
  this->mbr_0x298 = 0;
  this->mbr_0x29c = 0;
  this->mbr_0x2a0 = 0;
  this->mbr_0x2a4 = 0;
  this->mbr_0x2a8 = 0;
  this->mbr_0x2b0 = 0;
  cls_0x40b770::meth_0x40b830((cls_0x40b770 *)this->mbr_0x8,0);
  return;
}



// Function at 004f4bc0

cls_0x5a47f0 * cls_0x5a89b4::virt_meth_0x4f4bc0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ed2e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x7c8);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a89b4__vftable_5a89b4_005a89b4;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f70f0

cls_0x5a89b4 * cls_0x5a89b4::virt_meth_0x4f70f0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7120

cls_0x5a89b4 * cls_0x5a89b4::virt_meth_0x4f7120()

{
  byte in_stack_00000004;
  
  ~cls_0x5a89b4(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7140

cls_0x5a89b4 * __thiscall cls_0x5a89b4::~cls_0x5a89b4(cls_0x5a89b4 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f578;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a89b4__vftable_5a89b4_005a89b4;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5a89b4 *)pcVar1;
}



