#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b0a28



// Function at 00502b70

void cls_0x5b0a28::virt_meth_0x502b70()

{
  dword dVar1;
  
  this->virt_meth_0x4de800();
  if (this->mbr_0x188 == 0) {
    dVar1 = this->mbr_0x184;
    this->mbr_0x184 = dVar1 + 1;
    if (dVar1 == 0) {
      this->mbr_0x188 = 0x3c;
    }
    else if (dVar1 == 1) {
      dVar1 = this->mbr_0xd8;
      if (dVar1 != 0) {
        FUN_004de250(*(TPlayScreen **)(dVar1 + 4),(int *)&this->mbr_0x10,300,
                     *(int *)(*(int *)(dVar1 + 0x120) + 0x54),
                     *(int *)(*(int *)(dVar1 + 0x120) + 0x58));
        FUN_004de610(*(TPlayScreen **)(this->mbr_0xd8 + 4),(int *)&this->mbr_0x10,300);
      }
      this->mbr_0x188 = 0x32;
    }
    else if (dVar1 == 2) {
      this->mbr_0x188 = 0x19;
    }
    if (3 < (int)this->mbr_0x184) {
      SoftwareRenderer::meth_0x4defe0((SoftwareRenderer *)this);
    }
  }
  this->mbr_0x188 = this->mbr_0x188 - 1;
  return;
}



// Function at 00509360

cls_0x5b0a28 * cls_0x5b0a28::virt_meth_0x509360()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509390

cls_0x5a47f0 * cls_0x5b0a28::virt_meth_0x509390()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a063e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x578);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5b0a28__vftable_5b0a28_005b0a28;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00509430

cls_0x5b0a28 * cls_0x5b0a28::virt_meth_0x509430()

{
  byte in_stack_00000004;
  
  ~cls_0x5b0a28(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00509450

cls_0x5b0a28 * __thiscall cls_0x5b0a28::~cls_0x5b0a28(cls_0x5b0a28 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0658;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b0a28__vftable_5b0a28_005b0a28;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b0a28 *)pcVar1;
}



