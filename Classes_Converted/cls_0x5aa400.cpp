#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5aa400



// Function at 004e7160

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5aa400::virt_meth_0x4e7160()

{
  dword dVar1;
  float fVar2;
  dword *pdVar3;
  dword *pdVar4;
  int iVar5;
  
  this->virt_meth_0x40dd60();
  dVar1 = this->mbr_0x4;
  pdVar4 = &this->mbr_0x144;
  iVar5 = 5;
  *(undefined *)(dVar1 + 0x36) = 0xe0;
  *(undefined4 *)(dVar1 + 0xb0) = 0xffffffe0;
  pdVar3 = &this->mbr_0x108;
  do {
    pdVar3[-1] = 0x41a00000;
    *pdVar3 = 0x41a00000;
    *pdVar4 = 0;
    pdVar4 = pdVar4 + 1;
    pdVar3 = pdVar3 + 3;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  fVar2 = *(float *)&this->field_0x12c + _DAT_005a8d0c;
  this->mbr_0x10c = 0xc0a00000;
  this->mbr_0x118 = 0x40a00000;
  this->mbr_0x13c = 0;
  this->mbr_0x140 = 0;
  this->mbr_0x158 = 0x3e99999a;
  this->mbr_0x15c = 0x3dcccccd;
  *(float *)&this->field_0x12c = fVar2;
  *(float *)&this->field_0x138 = *(float *)&this->field_0x138 - _DAT_005a8d0c;
  return;
}



// Function at 004e7210

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5aa400::virt_meth_0x4e7210()

{
  dword dVar1;
  float fVar2;
  
  this->virt_meth_0x40e2e0();
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  dVar1 = this->mbr_0x13c;
  *(float *)&this->field_0xfc = *(float *)&this->field_0xfc + _DAT_005a37e8;
  if ((int)dVar1 < 0x18) {
    if (0xb < (int)dVar1) {
      this->mbr_0x10c = (dword)((float)this->mbr_0x10c + _DAT_005a34e4);
      this->mbr_0x118 = (dword)((float)this->mbr_0x118 - _DAT_005a34e4);
    }
    fVar2 = *(float *)&this->field_0x12c - _DAT_005a36d8;
    this->mbr_0x13c = dVar1 + 1;
    *(float *)&this->field_0x12c = fVar2;
    *(float *)&this->field_0x138 = *(float *)&this->field_0x138 + _DAT_005a36d8;
  }
  this->mbr_0x140 = this->mbr_0x140 + 1;
  return;
}



// Function at 004f62e0

cls_0x5aa400 * cls_0x5aa400::virt_meth_0x4f62e0()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f6310

cls_0x5a47f0 * cls_0x5aa400::virt_meth_0x4f6310()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f28e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x160);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5aa400__vftable_5aa400_005aa400;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f63b0

cls_0x5aa400 * cls_0x5aa400::virt_meth_0x4f63b0()

{
  byte in_stack_00000004;
  
  ~cls_0x5aa400(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f63d0

cls_0x5aa400 * __thiscall cls_0x5aa400::~cls_0x5aa400(cls_0x5aa400 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f2a8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5aa400__vftable_5aa400_005aa400;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5aa400 *)pcVar1;
}



