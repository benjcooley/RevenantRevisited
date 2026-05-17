#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b5808_TBookPane



// Function at 00444e30

void cls_0x5b5808_TBookPane::meth_0x444e30(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8, undefined4 param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16)

{
  cls_0x5a3c68 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059ce2b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 0;
  if (this_00 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (this_00,param_1,param_2,param_3,param_4,param_5,param_6,param_7,param_8,param_9,
               param_10,param_11,param_12,param_13,param_14,param_15);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  ExceptionList = local_c;
  return;
}



// Function at 005212c0

void cls_0x5b5808_TBookPane::virt_meth_0x5212c0_TScrollPane_Close()

{
  this->virt_meth_0x434f30();
  if ((LPCVOID)this->mbr_0x180 != (LPCVOID)0x0) {
    FUN_00482f80((LPCVOID)this->mbr_0x180);
  }
  TCharacter::meth_0x48ed60((TCharacter *)&DAT_0065caf0);
  cls_0x5a4f28::meth_0x48ef30((cls_0x5a4f28 *)&DAT_0065caf0);
  cls_0x5a4f28::meth_0x48ee10((cls_0x5a4f28 *)&DAT_0065caf0);
  this->mbr_0x17c = 0;
  UNK_0065cb3c._4_4_ = 1;
  return;
}



// Function at 00521530

void cls_0x5b5808_TBookPane::virt_meth_0x521530(undefined4 param_1)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  int in_stack_00000008;
  
  this->virt_meth_0x4361f0(param_1);
  dVar3 = 0;
  if (in_stack_00000008 != 0) {
    switch(param_1) {
    case 0x23:
      dVar2 = this->mbr_0x188;
      dVar1 = dVar2 + 100000;
      this->mbr_0x188 = dVar1;
      if (((int)dVar1 < 0) || (dVar3 = this->mbr_0x18c, (int)dVar3 < (int)dVar1)) {
        this->mbr_0x188 = dVar3;
      }
      if (dVar2 != this->mbr_0x188) {
        (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
      }
      break;
    case 0x24:
      dVar2 = this->mbr_0x188;
      dVar1 = dVar2 - 100000;
      this->mbr_0x188 = dVar1;
      if (((int)dVar1 < 0) || (dVar3 = this->mbr_0x18c, (int)dVar3 < (int)dVar1)) {
        this->mbr_0x188 = dVar3;
      }
      if (dVar2 != this->mbr_0x188) {
        (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
        return;
      }
      break;
    case 0x26:
      dVar2 = this->mbr_0x188;
      dVar1 = dVar2 - 1;
      this->mbr_0x188 = dVar1;
      if (((int)dVar1 < 0) || (dVar3 = this->mbr_0x18c, (int)dVar3 < (int)dVar1)) {
        this->mbr_0x188 = dVar3;
      }
      if (dVar2 != this->mbr_0x188) {
        (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
        return;
      }
      break;
    case 0x28:
      dVar2 = this->mbr_0x188;
      dVar1 = dVar2 + 1;
      this->mbr_0x188 = dVar1;
      if (((int)dVar1 < 0) || (dVar3 = this->mbr_0x18c, (int)dVar3 < (int)dVar1)) {
        this->mbr_0x188 = dVar3;
      }
      if (dVar2 != this->mbr_0x188) {
        (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
        return;
      }
    }
  }
  return;
}



// Function at 00521e60

cls_0x5b5808_TBookPane::cls_0x5b5808_TBookPane()

{
  this->mbr_0xc = 0x280;
  this->mbr_0x1c = 0x280;
  this->vftptr_0x0 = (cls_0x5b5808__vftable_5b5808 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x4 = 0;
  this->mbr_0x14 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x10 = 0x1e0;
  this->mbr_0x20 = 0x1e0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->mbr_0x17c = 0;
  this->vftptr_0x0 = &cls_0x5b5808__vftable_5b5808_005b5808;
  return this;
}



// Function at 00521ec0

cls_0x5b5808_TBookPane * cls_0x5b5808_TBookPane::virt_meth_0x521ec0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b5808_TBookPane(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00521ee0

cls_0x5b5808_TBookPane * __thiscall cls_0x5b5808_TBookPane::~cls_0x5b5808_TBookPane(cls_0x5b5808_TBookPane *this)

{
  cls_0x5b5808_TBookPane *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a1326;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5b5808__vftable_5b5808 *)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 0;
  this->virt_meth_0x434f30();
  this->vftptr_0x0 = (cls_0x5b5808__vftable_5b5808 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  this->virt_meth_0x434f30();
  local_4 = 0xffffffff;
  pcVar1 = (cls_0x5b5808_TBookPane *)FUN_004830f0(*(LPCVOID *)&this->field_0x98);
  ExceptionList = local_c;
  return pcVar1;
}



// Function at 00521fb0

TScreen * cls_0x5b5808_TBookPane::virt_meth_0x521fb0(undefined4 param_1)

{
  TScreen *this_00;
  TScreen *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a133b;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (TScreen *)FUN_00482fb0(0xd8);
  local_4 = 0;
  pcVar1 = (TScreen *)0x0;
  if (this_00 != (TScreen *)0x0) {
    TScreen::TScreen(this_00);
    this_00->vftptr_0x0 = (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x522010_005b58c4;
    pcVar1 = this_00;
  }
  ExceptionList = local_c;
  return pcVar1;
}



