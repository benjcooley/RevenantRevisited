#include "SDrawParam.h"

// Decompiled methods and structure for class: SDrawParam



// Function at 004ec890

void SDrawParam::virt_meth_0x4ec890()

{
  dword dVar1;
  int iVar2;
  
  this->virt_meth_0x4de800();
  this->sx = this->sx + 1;
  if ((TPlayScreen *)this->dheight != (TPlayScreen *)0x0) {
    TPlayScreen::meth_0x4cee70_Stop((TPlayScreen *)this->dheight);
    dVar1 = this->sx;
    if ((int)dVar1 < 0xf0) {
      if (((((int *)this->dheight)[2] & 0x2000000U) == 0) ||
         (iVar2 = (**(code **)(*(int *)this->dheight + 0x1c0))(), iVar2 < 1)) {
        this->sx = 0xf0;
      }
      dVar1 = this->sx;
    }
    if ((dVar1 == 0xf0) && ((DAT_0066829c == 0 || (DAT_0067682c != 0)))) {
      (**(code **)(*(int *)this->dheight + 0x40))(((int *)this->dheight)[2] & 0xff7fffff);
      (**(code **)(*(int *)this->dheight + 0x40))(((int *)this->dheight)[2] & 0xfdffffff);
    }
    return;
  }
  return;
}



// Function at 004ec9d0

void SDrawParam::virt_meth_0x4ec9d0(int param_1)

{
  dword dVar1;
  dword in_stack_00000008;
  
  this->virt_meth_0x4df0f0(param_1);
  if (param_1 == 1) {
    if (this->dheight != in_stack_00000008) {
      return;
    }
  }
  else {
    if (param_1 != 2) {
      return;
    }
    dVar1 = this->dheight;
    if (dVar1 == 0) {
      return;
    }
    if (*(dword *)(dVar1 + 0x44) != in_stack_00000008) {
      return;
    }
    if ((*(uint *)(dVar1 + 8) & 0x80000) != 0) {
      return;
    }
  }
  this->dheight = 0;
  (*this->vftptr_0x0->virt_meth_0x410bd0_64)(this);
  return;
}



// Function at 004f7e80

SDrawParam * SDrawParam::virt_meth_0x4f7e80()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7eb0

cls_0x5a47f0 * SDrawParam::virt_meth_0x4f7eb0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f86e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x884);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ab460__vftable_5ab460_005ab460;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f7f50

SDrawParam * SDrawParam::virt_meth_0x4f7f50()

{
  byte in_stack_00000004;
  
  ~SDrawParam(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7f70

SDrawParam * __thiscall SDrawParam::~SDrawParam(SDrawParam *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f888;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ab460__vftable_5ab460_005ab460;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (SDrawParam *)pcVar1;
}



// Function at 004f7fc0

TScreen * SDrawParam::virt_meth_0x4f7fc0(int param_1)

{
  uint uVar1;
  uint uVar2;
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059f8ab;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x234);
  local_4 = 0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    uVar1 = *(uint *)(param_1 + 4);
    uVar2 = this_00->mbr_0x8;
    *(undefined4 *)&this_00->field_0xe4 = 0;
    this_00[1].mbr_0x38 = 0;
    this_00->mbr_0x8 = uVar2 | uVar1 | 0x48001;
    this_00[1].mbr_0x1c = 0;
    this_00[1].mbr_0xc = 0;
    this_00[1].mbr_0xe = 0;
    *(undefined *)&this_00[1].mbr_0x48 = 0;
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4f8080_005ab4c4;
    *(undefined4 *)&this_00[1].field_0x70 = 1;
    *(undefined4 *)&this_00[1].field_0xa4 = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



