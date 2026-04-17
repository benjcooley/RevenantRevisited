#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5ad26c



// Function at 004f2e60

void cls_0x5ad26c::virt_meth_0x4f2e60()

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  undefined4 uVar4;
  undefined4 *puVar5;
  int iVar6;
  
  this->virt_meth_0x40dd60();
  uVar4 = cls_0x5a7e38::meth_0x40eef0((cls_0x5a7e38 *)this);
  cls_0x5a486c::meth_0x40a0c0((cls_0x5a486c *)this->mbr_0x8,uVar4);
  dVar1 = this->mbr_0x4;
  this->mbr_0x100 = 100;
  dVar2 = *(dword *)(dVar1 + 0x14);
  dVar3 = *(dword *)(dVar1 + 0x18);
  this->mbr_0x104 = *(dword *)(dVar1 + 0x10);
  this->mbr_0x108 = dVar2;
  this->mbr_0x10c = dVar3;
  puVar5 = FUN_00482fb0(4000);
  this->mbr_0xfc = (dword)puVar5;
  iVar6 = 0;
  if (0 < (int)this->mbr_0x100) {
    do {
      (*this->vftptr_0x0->virt_meth_0x4f2cb0_96)(this);
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)this->mbr_0x100);
  }
  iVar6 = 0;
  if (0 < (int)this->mbr_0x100) {
    do {
      (*this->vftptr_0x0->virt_meth_0x4f2d70_100)(this);
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)this->mbr_0x100);
  }
  return;
}



// Function at 004f2f00

void cls_0x5ad26c::virt_meth_0x4f2f00()

{
  this->virt_meth_0x40e2e0();
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  (*this->vftptr_0x0->virt_meth_0x4f2d70_100)(this);
  return;
}



// Function at 004f9a10

cls_0x5ad26c * cls_0x5ad26c::virt_meth_0x4f9a10()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9a40

cls_0x5a47f0 * cls_0x5ad26c::virt_meth_0x4f9a40()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fe2e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x110);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ad26c__vftable_5ad26c_005ad26c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f9ae0

cls_0x5ad26c * cls_0x5ad26c::virt_meth_0x4f9ae0()

{
  byte in_stack_00000004;
  
  ~cls_0x5ad26c(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9b00

cls_0x5ad26c * __thiscall cls_0x5ad26c::~cls_0x5ad26c(cls_0x5ad26c *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fe48;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ad26c__vftable_5ad26c_005ad26c;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ad26c *)pcVar1;
}



