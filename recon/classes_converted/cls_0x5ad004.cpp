#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5ad004



// Function at 004f98f0

cls_0x5ad004 * cls_0x5ad004::virt_meth_0x4f98f0()

{
  byte in_stack_00000004;
  
  ~cls_0x5ad004(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f9910

cls_0x5ad004 * __thiscall cls_0x5ad004::~cls_0x5ad004(cls_0x5ad004 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059fdd8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5ad004__vftable_5ad004_005ad004;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5ad004 *)pcVar1;
}



// Function at 004f9960

TScreen * cls_0x5ad004::virt_meth_0x4f9960(int param_1)

{
  uint uVar1;
  uint uVar2;
  TScreen *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059fdfb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0x184);
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
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x4f9a10_005ad068;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (TScreen *)0x0;
}



