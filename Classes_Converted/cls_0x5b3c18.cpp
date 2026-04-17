#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b3c18



// Function at 00510180

cls_0x5b3c18 * cls_0x5b3c18::virt_meth_0x510180()

{
  byte in_stack_00000004;
  
  TScreen::~TScreen((TScreen *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00510400

void cls_0x5b3c18::virt_meth_0x510400()

{
  (*this->vftptr_0x0->FUN_0046f250_24)(0);
  this->mbr_0xe0 = 0;
  this->mbr_0x184 = 0x8000;
  this->mbr_0x18c = 1;
  this->mbr_0x188 = 0x100000;
  cls_0x41c7d0::meth_0x49b880((cls_0x41c7d0 *)&DAT_00667548);
  this->mbr_0x190 = 1;
  return;
}



// Function at 00510450

void cls_0x5b3c18::virt_meth_0x510450()

{
  undefined uVar1;
  undefined uVar2;
  undefined4 uVar3;
  
  this->virt_meth_0x510220();
  if ((this->mbr_0xc == 1) && (this->mbr_0x190 != 0)) {
    uVar3 = 1;
    uVar2 = 0x7f;
    this->mbr_0x190 = 0;
    uVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
    cls_0x41c7d0::meth_0x49b990((cls_0x41c7d0 *)&DAT_00667548,uVar1,uVar2,uVar3);
  }
  return;
}



// Function at 00514950

cls_0x5a85ac * cls_0x5b3c18::virt_meth_0x514950(undefined4 param_1)

{
  cls_0x5a85ac *this_00;
  void *unaff_ESI;
  void *local_c;
  undefined4 uStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  uStack_8 = &LAB_005a0ccb;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a85ac *)FUN_00482fb0(0x194);
  local_4 = 0;
  if (this_00 != (cls_0x5a85ac *)0x0) {
    cls_0x5a85ac::cls_0x5a85ac(this_00,param_1);
    local_4 = CONCAT31(local_4._1_3_,1);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&PTR_virt_meth_0x5152e0_005b3e18;
    (*(code *)PTR_virt_meth_0x46f250_005b3e30)(0);
    (this_00->TScreen).mbr_0xe0 = 0;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x24 + 3) = 0x8000;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x28 + 3) = 0x100000;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x2c + 3) = 1;
    uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,2);
    (this_00->TScreen).vftptr_0x0 =
         (TScreen__vftable_5a50e8 *)&cls_0x5b3c18__vftable_5b3c18_005b3c18;
    (*cls_0x5b3c18__vftable_5b3c18_005b3c18.FUN_0046f250_24)(0);
    (this_00->TScreen).mbr_0xe0 = 0;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x24 + 3) = 0x8000;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x2c + 3) = 1;
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x28 + 3) = 0x100000;
    cls_0x41c7d0::meth_0x49b880((cls_0x41c7d0 *)&DAT_00667548);
    *(undefined4 *)((int)&this_00[1].TScreen.mbr_0x30 + 3) = 1;
    ExceptionList = unaff_ESI;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a85ac *)0x0;
}



// Function at 00515310

cls_0x5b3c18 * cls_0x5b3c18::virt_meth_0x515310()

{
  byte in_stack_00000004;
  
  ~cls_0x5b3c18(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00515330

cls_0x5b3c18 * __thiscall cls_0x5b3c18::~cls_0x5b3c18(cls_0x5b3c18 *this)

{
  undefined4 uVar1;
  cls_0x5b3c18 *pcVar2;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0ed8;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b3c18__vftable_5b3c18_005b3c18;
  local_4 = 0;
  uVar1 = cls_0x41c7d0::meth_0x49c430((cls_0x41c7d0 *)&DAT_00667548);
  cls_0x41c7d0::meth_0x49b8e0((cls_0x41c7d0 *)&DAT_00667548,uVar1);
  local_4 = 0xffffffff;
  pcVar2 = (cls_0x5b3c18 *)TScreen::~TScreen((TScreen *)this);
  ExceptionList = local_c;
  return pcVar2;
}



