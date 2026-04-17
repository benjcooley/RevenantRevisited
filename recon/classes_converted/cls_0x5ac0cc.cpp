#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5ac0cc



// Function at 004f0130

void cls_0x5ac0cc::virt_meth_0x4f0130()

{
  this->virt_meth_0x40dd60();
  this->mbr_0x100 = 0x3ecccccd;
  this->mbr_0xfc = 0;
  this->mbr_0x108 = 0;
  this->mbr_0x104 = 0x41f00000;
  return;
}



// Function at 004f0160

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x5ac0cc::virt_meth_0x4f0160()

{
  int *piVar1;
  float fVar2;
  float fVar3;
  dword dVar4;
  int *piVar5;
  int iVar6;
  
  this->virt_meth_0x40e2e0();
  (**(code **)(*(int *)this->mbr_0x4 + 0x158))(0);
  fVar2 = (float)this->mbr_0x108 + _DAT_005a356c;
  dVar4 = this->mbr_0xfc + 1;
  fVar3 = (float)_DAT_005a3a68;
  this->mbr_0x108 = (dword)fVar2;
  this->mbr_0xfc = dVar4;
  if (fVar3 < fVar2) {
    this->mbr_0x108 = (dword)(fVar2 - _DAT_005a36c0);
  }
  this->mbr_0x100 = (dword)((float)this->mbr_0x100 + _DAT_005a8a14);
  this->mbr_0x104 = (dword)((float)this->mbr_0x104 - _DAT_005a8a14);
  if (0x4b < (int)dVar4) {
    piVar1 = (int *)this->mbr_0x4;
    iVar6 = piVar1[0x36];
    if ((((iVar6 != 0) && (*(int *)(iVar6 + 0x11c) != 0)) &&
        (*(int *)(*(int *)(iVar6 + 0x11c) + 200) != 0)) &&
       ((0 < *(int *)(iVar6 + 0xc) && (*(int *)(iVar6 + 0x10) != 0)))) {
      if (*(int *)(iVar6 + 0xc) < 1) {
        piVar5 = (int *)0x0;
      }
      else {
        piVar5 = *(int **)(iVar6 + 0x10);
      }
      (**(code **)(*piVar5 + 0x1b4))(0);
    }
    (**(code **)(*piVar1 + 0x40))(piVar1[2] | 0x1000);
    (**(code **)(*piVar1 + 0x40))(piVar1[2] | 0x8000);
    if ((piVar1[0x36] != 0) && (iVar6 = (**(code **)(*piVar1 + 0x3c))(), iVar6 != 0)) {
      (**(code **)(*(int *)piVar1[0x36] + 0xc))();
    }
  }
  return;
}



// Function at 004f8980

cls_0x5ac0cc * cls_0x5ac0cc::virt_meth_0x4f8980()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f89b0

cls_0x5a47f0 * cls_0x5ac0cc::virt_meth_0x4f89b0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059face;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x10c);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5ac0cc__vftable_5ac0cc_005ac0cc;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f8a50

cls_0x5ac0cc * cls_0x5ac0cc::virt_meth_0x4f8a50()

{
  byte in_stack_00000004;
  
  ~cls_0x5ac0cc(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f8a70

cls_0x5ac0cc * __thiscall cls_0x5ac0cc::~cls_0x5ac0cc(cls_0x5ac0cc *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fae8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ac0cc__vftable_5ac0cc_005ac0cc;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ac0cc *)pcVar1;
}



// Function at 004f8ac0

TScreen * cls_0x5ac0cc::virt_meth_0x4f8ac0(int param_1)

{
  uint uVar1;
  uint uVar2;
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fb0b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x18c);
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
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4f8bb0_005ac130;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



