#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b45ac



// Function at 00512bf0

void cls_0x5b45ac::virt_meth_0x512bf0()

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



// Function at 00514fb0

cls_0x5a47f0 * cls_0x5b45ac::virt_meth_0x514fb0()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a0e3e;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0x1cc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x515620_005b47b0;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



// Function at 00515590

cls_0x5b45ac * cls_0x5b45ac::virt_meth_0x515590()

{
  byte in_stack_00000004;
  
  cls_0x5155b0::~cls_0x5155b0((cls_0x5155b0 *)this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00515640

cls_0x5b45ac * __thiscall cls_0x5b45ac::~cls_0x5b45ac(cls_0x5b45ac *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a0f88;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5b45ac__vftable_5b45ac *)&PTR_virt_meth_0x515620_005b47b0;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5b45ac *)pcVar1;
}



