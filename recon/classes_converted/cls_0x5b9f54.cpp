#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b9f54



// Function at 00573430

cls_0x5b9f54::cls_0x5b9f54()

{
  cls_0x5ba0a0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a23ac;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0xc = (dword)&this->mbr_0x4;
  this->mbr_0x4 = (dword)&this->mbr_0x8;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = (dword)&this->mbr_0x10;
  this->mbr_0x10 = (dword)&this->mbr_0x14;
  this->mbr_0x14 = 0;
  local_4._0_1_ = 1;
  local_4._1_3_ = 0;
  this->vftptr_0x0 = &cls_0x5b9f54__vftable_5b9f54_005b9f54;
  this_00 = (cls_0x5ba0a0 *)FUN_00482fb0(0x2c);
  local_4 = CONCAT31(local_4._1_3_,2);
  if (this_00 != (cls_0x5ba0a0 *)0x0) {
    cls_0x5ba0a0::cls_0x5ba0a0(this_00);
    (this_00->cls_0x588410).mbr_0x24 = (dword)this;
    this_00->vftptr_0x0 = (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5b9f40__vftable_5b9f40_005b9f40;
    this->mbr_0x1c = (dword)this_00;
    ExceptionList = local_c;
    return this;
  }
  this->mbr_0x1c = 0;
  ExceptionList = local_c;
  return this;
}



// Function at 005734e0

cls_0x5b9f54 * __thiscall cls_0x5b9f54::~cls_0x5b9f54(cls_0x5b9f54 *this)

{
  int *piVar1;
  int *piVar2;
  cls_0x5b9f54 *pcVar3;
  cls_0x5b9f54__vftable_5b9f54 **ppcVar4;
  int iVar5;
  cls_0x5b9f54__vftable_5b9f54 *pcVar6;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a23cc;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5b9f54__vftable_5b9f54_005b9f54;
  local_4 = 1;
  if ((undefined4 *)this->mbr_0x1c != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x1c)(1);
  }
  piVar1 = (int *)this->mbr_0x10;
  iVar5 = *piVar1;
  while (iVar5 != 0) {
    iVar5 = *piVar1;
    if (iVar5 != 0) {
      piVar2 = (int *)piVar1[1];
      *piVar1 = 0;
      *(int **)(iVar5 + 4) = piVar2;
      *piVar2 = iVar5;
    }
    iVar5 = *piVar1;
  }
  pcVar3 = (cls_0x5b9f54 *)this->mbr_0x4;
  pcVar6 = pcVar3->vftptr_0x0;
  while (pcVar6 != (cls_0x5b9f54__vftable_5b9f54 *)0x0) {
    pcVar6 = pcVar3->vftptr_0x0;
    if (pcVar6 != (cls_0x5b9f54__vftable_5b9f54 *)0x0) {
      ppcVar4 = (cls_0x5b9f54__vftable_5b9f54 **)pcVar3->mbr_0x4;
      pcVar3->vftptr_0x0 = (cls_0x5b9f54__vftable_5b9f54 *)0x0;
      pcVar6[1].virt_meth_0x573dd0_0 = (virt_meth_0x573dd0 *)ppcVar4;
      *ppcVar4 = pcVar6;
    }
    pcVar6 = pcVar3->vftptr_0x0;
  }
  ExceptionList = local_c;
  return pcVar3;
}



// Function at 00573dd0

cls_0x5b9f54 * cls_0x5b9f54::virt_meth_0x573dd0()

{
  byte in_stack_00000004;
  
  ~cls_0x5b9f54(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



