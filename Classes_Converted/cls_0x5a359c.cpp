#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a359c



// Function at 0040db90

cls_0x5a359c::cls_0x5a359c()

{
  int iVar1;
  dword in_stack_00000004;
  
  iVar1 = DAT_005e872c;
  this->vftptr_0x0 = &cls_0x5a359c__vftable_5a359c_005a359c;
  if (iVar1 < 0x80) {
    (&DAT_005e851c)[iVar1] = this;
    DAT_005e872c = iVar1 + 1;
    this->mbr_0x4 = in_stack_00000004;
    return this;
  }
  this->mbr_0x4 = in_stack_00000004;
  return this;
}



// Function at 0040dc00

cls_0x5a47f0 * cls_0x5a359c::virt_meth_0x40dc00()

{
  cls_0x5a47f0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059c66c;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(0xfc);
  local_4 = 0;
  if (this_00 != (cls_0x5a47f0 *)0x0) {
    cls_0x5a47f0::cls_0x5a47f0(this_00);
    local_4._0_1_ = 1;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x8,0x10);
    local_4 = CONCAT31(local_4._1_3_,2);
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_00[1].mbr_0x1c,0);
    this_00->vftptr_0x0 = (cls_0x5a47f0__vftable_5a47f0 *)&PTR_virt_meth_0x410ca0_005a370c;
    ExceptionList = local_c;
    return this_00;
  }
  ExceptionList = local_c;
  return (cls_0x5a47f0 *)0x0;
}



