#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b5f20



// Function at 00523750

cls_0x5a85ac * cls_0x5b5f20::virt_meth_0x523750(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a13cb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x1b4);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&cls_0x5b5f20__vftable_5b5f20_005b5f20;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x50 + 3) = 0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 005237d0

cls_0x5a47f0 * cls_0x5b5f20::virt_meth_0x5237d0()

{
  cls_0x5a47f0 *this_00;
  int iVar1;
  dword *pdVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a13fe;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x408);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x523940_005b6124;
    pdVar2 = &this_00[0xf].mbr_0x24;
    for (iVar1 = 0x14; iVar1 != 0; iVar1 = iVar1 + -1) {
      *pdVar2 = 0;
      pdVar2 = pdVar2 + 1;
    }
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 005238e0

cls_0x5b5f20 * cls_0x5b5f20::virt_meth_0x5238e0()

{
  byte in_stack_00000004;
  
  cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00523910

cls_0x5b5f20 * cls_0x5b5f20::virt_meth_0x523910()

{
  byte in_stack_00000004;
  
  ~cls_0x5b5f20(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00523930

cls_0x5b5f20 * __thiscall cls_0x5b5f20::~cls_0x5b5f20(cls_0x5b5f20 *this)

{
  cls_0x5b5f20 *pcVar1;
  
  this->vftptr_0x0 = &cls_0x5b5f20__vftable_5b5f20_005b5f20;
  pcVar1 = (cls_0x5b5f20 *)TScreen::~TScreen((TScreen *)this);
  return pcVar1;
}



