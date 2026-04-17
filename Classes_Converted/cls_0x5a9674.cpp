#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a9674



// Function at 004e3000

void cls_0x5a9674::virt_meth_0x4e3000()

{
  this->mbr_0x184 = 0;
  TPlayScreen::meth_0x471b60((TPlayScreen *)this);
  this->mbr_0x188 = 0;
  return;
}



// Function at 004e30e0

void cls_0x5a9674::virt_meth_0x4e30e0(int param_1)

{
  dword dVar1;
  dword in_stack_00000008;
  
  this->virt_meth_0x4df0f0(param_1);
  if (param_1 == 1) {
    if (this->mbr_0x184 != in_stack_00000008) {
      return;
    }
  }
  else {
    if (param_1 != 2) {
      return;
    }
    dVar1 = this->mbr_0x184;
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
  (*this->vftptr_0x0->virt_meth_0x410bd0_64)(this);
  dVar1 = this->mbr_0x184;
  this->mbr_0x184 = 0;
  *(undefined4 *)(dVar1 + 0x1b8) = 0;
  *(undefined4 *)(dVar1 + 0x1bc) = 0;
  return;
}



// Function at 004e3790

undefined4 cls_0x5a9674::virt_meth_0x4e3790()

{
  cls_0x411eb0::meth_0x4178e0((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  cls_0x411eb0::meth_0x417d60((cls_0x411eb0 *)((int)&DAT_0065a578 + 4),8);
  (**(code **)(this->mbr_0xfc + 0xc))(0,1);
  (**(code **)(this->mbr_0x124 + 0xc))(0,1);
  cls_0x411eb0::meth_0x417b00((cls_0x411eb0 *)((int)&DAT_0065a578 + 4));
  return 1;
}



// Function at 004f55b0

cls_0x5a47f0 * cls_0x5a9674::virt_meth_0x4f55b0()

{
  cls_0x5a47f0 *this_00;
  undefined4 *puVar1;
  int iVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ef84;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x158);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4._0_1_ = 2;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x410ca0_005a370c;
    local_4._0_1_ = 3;
    this_00[4].mbr_0xc = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
    puVar1 = FUN_00482fb0(0x1810);
    this_00[4].mbr_0x10 = (dword)puVar1;
    puVar1 = puVar1 + 0x15;
    iVar2 = 0x46;
    do {
      *puVar1 = 0;
      puVar1 = puVar1 + 0x16;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    this_00[4].mbr_0x28 = 0x46;
    local_4 = CONCAT31(local_4._1_3_,4);
    *(cls_0x5a96d8__vftable_5a96d8 **)&this_00[4].mbr_0x34 = &cls_0x5a96d8__vftable_5a96d8_005a96d8;
    puVar1 = FUN_00482fb0(0x1810);
    this_00[4].mbr_0x38 = (dword)puVar1;
    puVar1 = puVar1 + 0x15;
    iVar2 = 0x46;
    do {
      *puVar1 = 0;
      puVar1 = puVar1 + 0x16;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    this_00[5].mbr_0x14 = 0x46;
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&cls_0x5a9674__vftable_5a9674_005a9674;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 004f7ae0

void cls_0x5a9674::virt_meth_0x4f7ae0()

{
  dword dVar1;
  undefined uVar2;
  int *piVar3;
  uint uVar4;
  undefined3 extraout_var;
  uint uVar5;
  
  dVar1 = this->mbr_0xd8;
  if ((((dVar1 != 0) && (*(int *)(dVar1 + 0x11c) != 0)) &&
      (*(int *)(*(int *)(dVar1 + 0x11c) + 200) != 0)) &&
     ((0 < *(int *)(dVar1 + 0xc) && (*(int *)(dVar1 + 0x10) != 0)))) {
    if (*(int *)(dVar1 + 0xc) < 1) {
      piVar3 = (int *)0x0;
    }
    else {
      piVar3 = *(int **)(dVar1 + 0x10);
    }
    (**(code **)(*piVar3 + 0x1b4))(0);
  }
  uVar5 = this->mbr_0x8 | 0x1000;
  (*this->vftptr_0x0->virt_meth_0x410bd0_64)(this);
  uVar4 = this->mbr_0x8 | 0x8000;
  (*this->vftptr_0x0->virt_meth_0x410bd0_64)(this);
  if (this->mbr_0xd8 != 0) {
    uVar2 = (*this->vftptr_0x0->virt_meth_0x4e37f0_60)(this);
    if (CONCAT31(extraout_var,uVar2) != 0) {
      (**(code **)(*(int *)this->mbr_0xd8 + 0xc))(uVar4,uVar5);
    }
  }
  return;
}



// Function at 004f7b70

cls_0x5a9674 * cls_0x5a9674::virt_meth_0x4f7b70()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7bb0

cls_0x5a9674 * cls_0x5a9674::virt_meth_0x4f7bb0()

{
  byte in_stack_00000004;
  
  ~cls_0x5a9674(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f7bd0

cls_0x5a9674 * __thiscall cls_0x5a9674::~cls_0x5a9674(cls_0x5a9674 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f7e4;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a9674__vftable_5a9674_005a9674;
  local_4 = 2;
  this->virt_meth_0x40de10();
  local_4._0_1_ = 1;
  this->mbr_0x124 = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0((LPCVOID)this->mbr_0x128);
  local_4 = (uint)local_4._1_3_ << 8;
  this->mbr_0xfc = (dword)&cls_0x5a96d8__vftable_5a96d8_005a96d8;
  FUN_004830f0((LPCVOID)this->mbr_0x100);
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5a9674 *)pcVar1;
}



