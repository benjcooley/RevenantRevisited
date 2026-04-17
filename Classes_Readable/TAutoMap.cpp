#include "TAutoMap.h"

// Decompiled methods and structure for class: TAutoMap



// Function at 00573430

TAutoMap::TAutoMap()

{
  cls_0x5ba0a0 *this_00;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a23ac;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->SavePlayerPixel = (dword)&this->MapArea;
  this->MapArea = (dword)&this->ScrollToTarget;
  this->ScrollToTarget = 0;
  this->MapGrfx = (dword)&this->DraggingMap;
  this->DraggingMap = (dword)&this->CurrentMap;
  this->CurrentMap = 0;
  local_4._0_1_ = 1;
  local_4._1_3_ = 0;
  this->vftptr_0x0 = &cls_0x5b9f54__vftable_5b9f54_005b9f54;
  this_00 = (cls_0x5ba0a0 *)FUN_00482fb0(0x2c);
  local_4 = CONCAT31(local_4._1_3_,2);
  if (this_00 != (cls_0x5ba0a0 *)0x0) {
    cls_0x5ba0a0::cls_0x5ba0a0(this_00);
    (this_00->TLightningAnimator).mbr_0x24 = (dword)this;
    this_00->vftptr_0x0 = (cls_0x5ba0a0__vftable_5ba0a0 *)&cls_0x5b9f40__vftable_5b9f40_005b9f40;
    this->DisplayMap = (dword)this_00;
    ExceptionList = local_c;
    return this;
  }
  this->DisplayMap = 0;
  ExceptionList = local_c;
  return this;
}



// Function at 005734e0

TAutoMap * __thiscall TAutoMap::~TAutoMap(TAutoMap *this)

{
  int *piVar1;
  int *piVar2;
  TAutoMap *pcVar3;
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
  if ((undefined4 *)this->DisplayMap != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->DisplayMap)(1);
  }
  piVar1 = (int *)this->DraggingMap;
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
  pcVar3 = (TAutoMap *)this->MapArea;
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

TAutoMap * TAutoMap::virt_meth_0x573dd0()

{
  byte in_stack_00000004;
  
  ~TAutoMap(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



