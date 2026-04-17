#include "classes_all.h"

// Decompiled methods and structure for class: TObjectInstance



// Function at 0046d010

TObjectInstance * __thiscall TObjectInstance::~TObjectInstance(TObjectInstance *this)

{
  LPCVOID pvVar1;
  TObjectInstance *pcVar2;
  
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x10;
  this->vftptr_0x0 = &TObjectInstance__vftable_5a5ed4_005a5ed4;
  pcVar2 = (TObjectInstance *)FUN_004830f0(pvVar1);
  return pcVar2;
}



// Function at 0046d150

TObjectInstance * __thiscall TObjectInstance::~TObjectInstance(TObjectInstance *this)

{
  LPCVOID pvVar1;
  TObjectInstance *pcVar2;
  
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x10;
  this->vftptr_0x0 = &TObjectInstance__vftable_5a5ed4_005a5ed4;
  pcVar2 = (TObjectInstance *)FUN_004830f0(pvVar1);
  return pcVar2;
}



// Function at 0046d380

TObjectInstance * __thiscall TObjectInstance::~TObjectInstance(TObjectInstance *this)

{
  LPCVOID pvVar1;
  TObjectInstance *pcVar2;
  
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x10;
  this->vftptr_0x0 = &TObjectInstance__vftable_5a5ed4_005a5ed4;
  pcVar2 = (TObjectInstance *)FUN_004830f0(pvVar1);
  return pcVar2;
}



// Function at 0046d460

TObjectInstance * __thiscall TObjectInstance::~TObjectInstance(TObjectInstance *this)

{
  LPCVOID pvVar1;
  TObjectInstance *pcVar2;
  
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x10;
  this->vftptr_0x0 = &TObjectInstance__vftable_5a5ed4_005a5ed4;
  pcVar2 = (TObjectInstance *)FUN_004830f0(pvVar1);
  return pcVar2;
}



// Function at 0047f530

TObjectInstance * __thiscall TObjectInstance::~TObjectInstance(TObjectInstance *this)

{
  LPCVOID pvVar1;
  TObjectInstance *pcVar2;
  
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x10;
  this->vftptr_0x0 = &TObjectInstance__vftable_5a5ed4_005a5ed4;
  pcVar2 = (TObjectInstance *)FUN_004830f0(pvVar1);
  return pcVar2;
}



// Function at 00487460

TObjectInstance * __thiscall TObjectInstance::~TObjectInstance(TObjectInstance *this)

{
  LPCVOID pvVar1;
  TObjectInstance *pcVar2;
  
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x10;
  this->vftptr_0x0 = &TObjectInstance__vftable_5a5ed4_005a5ed4;
  pcVar2 = (TObjectInstance *)FUN_004830f0(pvVar1);
  return pcVar2;
}



// Function at 00488ee0

TObjectInstance * __thiscall TObjectInstance::~TObjectInstance(TObjectInstance *this)

{
  LPCVOID pvVar1;
  TObjectInstance *pcVar2;
  
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x10;
  this->vftptr_0x0 = &TObjectInstance__vftable_5a5ed4_005a5ed4;
  pcVar2 = (TObjectInstance *)FUN_004830f0(pvVar1);
  return pcVar2;
}



// Function at 0048e880

TObjectInstance::TObjectInstance()

{
  cls_0x41c7f0::cls_0x41c7f0(&this->cls_0x41c7f0,0x20);
  (this->cls_0x41c7f0).mbr_0x14 = 0;
  this->mbr_0x1c = 0;
  this->mbr_0x44 = 0;
  this->mbr_0x40 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x58 = 0;
  this->mbr_0x60 = 0;
  this->mbr_0x64 = 0;
  this->vftptr_0x0 = &TObjectInstance__vftable_5a5ed4_005a5ed4;
  this->mbr_0x4c = 1;
  this->mbr_0x50 = 1;
  this->mbr_0x5c = 0xffffffff;
  return this;
}



// Function at 0048e8d0

TObjectInstance * __thiscall TObjectInstance::~TObjectInstance(TObjectInstance *this)

{
  LPCVOID pvVar1;
  TObjectInstance *pcVar2;
  
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x10;
  this->vftptr_0x0 = &TObjectInstance__vftable_5a5ed4_005a5ed4;
  pcVar2 = (TObjectInstance *)FUN_004830f0(pvVar1);
  return pcVar2;
}



// Function at 0048f0f0

void TObjectInstance::meth_0x48f0f0()

{
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  puStack_c = &DAT_005a5f20;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  local_8 = 0;
  ExceptionList = &local_14;
  (*this->vftptr_0x0->virt_meth_0x46cf20_12)(this);
  ExceptionList = local_14;
  return;
}



// Function at 0048f180

void TObjectInstance::meth_0x48f180()

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  puStack_c = &DAT_005a5f30;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  local_8 = 0;
  ExceptionList = &local_14;
  if (((int *)this->mbr_0x40 == (int *)0x0) ||
     (ExceptionList = &local_14, iVar2 = (**(code **)(*(int *)this->mbr_0x40 + 0x20))(), iVar2 != 0)
     ) {
    if (((int *)this->mbr_0x44 == (int *)0x0) ||
       (iVar2 = (**(code **)(*(int *)this->mbr_0x44 + 0x24))(), iVar2 != 0)) {
      uVar3 = 0;
    }
    else {
      uVar3 = *(uint *)(this->mbr_0x44 + 4);
    }
  }
  else {
    uVar3 = *(uint *)(this->mbr_0x40 + 4);
  }
  if ((uVar3 & 0x10) == 0) {
    (*this->vftptr_0x0->virt_meth_0x48fda0_16)(this);
  }
  if (((int *)this->mbr_0x40 != (int *)0x0) &&
     (iVar2 = (**(code **)(*(int *)this->mbr_0x40 + 0x18))(), iVar2 != 0)) {
    (***(code ***)this->mbr_0x40)();
  }
  piVar1 = (int *)this->mbr_0x44;
  if (((piVar1 != (int *)0x0) && (piVar1 != (int *)this->mbr_0x40)) &&
     (iVar2 = (**(code **)(*piVar1 + 0x18))(), iVar2 != 0)) {
    (***(code ***)this->mbr_0x44)();
  }
  piVar1 = (int *)this->mbr_0x40;
  iVar2 = DAT_0065abdc;
  if (((piVar1 != (int *)0x0) || (this->mbr_0x44 != 0)) && (DAT_006682bc == 0)) {
    if (((piVar1 != (int *)0x0) &&
        (iVar4 = (**(code **)(*piVar1 + 0x18))(), iVar2 = DAT_0065abdc, iVar4 != 0)) &&
       ((*(uint *)(this->mbr_0x40 + 4) & 2) != 0)) {
      iVar2 = *(int *)(this->mbr_0x44 + 0x10);
      if (iVar2 < 0) {
        iVar2 = -iVar2;
      }
      if (DAT_0065abd8 == 0) {
        DAT_0065abd8 = iVar2;
        iVar2 = DAT_0065abdc;
      }
    }
    DAT_0065abdc = iVar2;
    iVar2 = DAT_0065abdc;
    if ((((int *)this->mbr_0x44 != (int *)0x0) &&
        (iVar4 = (**(code **)(*(int *)this->mbr_0x44 + 0x18))(), iVar2 = DAT_0065abdc, iVar4 != 0))
       && ((*(uint *)(this->mbr_0x40 + 4) & 4) != 0)) {
      iVar2 = *(int *)(this->mbr_0x44 + 0x10);
      if (0 < iVar2) {
        iVar2 = -iVar2;
      }
      if (DAT_0065abd8 == 0) {
        DAT_0065abd8 = iVar2;
        iVar2 = DAT_0065abdc;
      }
    }
  }
  DAT_0065abdc = iVar2;
  if ((DAT_0065bb14 != (TObjectInstance *)0x0) && (this != DAT_0065bb14)) {
    meth_0x48f180(DAT_0065bb14);
  }
  ExceptionList = local_14;
  return;
}



// Function at 0048f340

void TObjectInstance::meth_0x48f340()

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  puStack_c = &DAT_005a5f40;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  if (DAT_006682bc == 0) {
    local_8 = 0;
    ExceptionList = &local_14;
    if (((int *)this->mbr_0x40 == (int *)0x0) ||
       (ExceptionList = &local_14, iVar2 = (**(code **)(*(int *)this->mbr_0x40 + 0x20))(),
       iVar2 != 0)) {
      if (((int *)this->mbr_0x44 == (int *)0x0) ||
         (iVar2 = (**(code **)(*(int *)this->mbr_0x44 + 0x24))(), iVar2 != 0)) {
        uVar3 = 0;
      }
      else {
        uVar3 = *(uint *)(this->mbr_0x44 + 4);
      }
    }
    else {
      uVar3 = *(uint *)(this->mbr_0x40 + 4);
    }
    if ((uVar3 & 0x20) == 0) {
      (*this->vftptr_0x0->virt_meth_0x48ff00_20)(this);
    }
    if (((int *)this->mbr_0x40 != (int *)0x0) &&
       (iVar2 = (**(code **)(*(int *)this->mbr_0x40 + 0x1c))(), iVar2 != 0)) {
      (**(code **)(*(int *)this->mbr_0x40 + 4))();
    }
    piVar1 = (int *)this->mbr_0x44;
    if (((piVar1 != (int *)0x0) && (piVar1 != (int *)this->mbr_0x40)) &&
       (iVar2 = (**(code **)(*piVar1 + 0x1c))(), iVar2 != 0)) {
      (**(code **)(*(int *)this->mbr_0x44 + 4))();
    }
  }
  ExceptionList = local_14;
  return;
}



// Function at 0048f450

void TObjectInstance::meth_0x48f450()

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  undefined4 in_stack_00000004;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  puStack_c = &DAT_005a5f50;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  local_8 = 0;
  ExceptionList = &local_14;
  if (((int *)this->mbr_0x40 == (int *)0x0) ||
     (ExceptionList = &local_14, iVar2 = (**(code **)(*(int *)this->mbr_0x40 + 0x20))(), iVar2 != 0)
     ) {
    if (((int *)this->mbr_0x44 == (int *)0x0) ||
       (iVar2 = (**(code **)(*(int *)this->mbr_0x44 + 0x24))(), iVar2 != 0)) {
      uVar3 = 0;
    }
    else {
      uVar3 = *(uint *)(this->mbr_0x44 + 4);
    }
  }
  else {
    uVar3 = *(uint *)(this->mbr_0x40 + 4);
  }
  if ((uVar3 & 0x20) == 0) {
    (*this->vftptr_0x0->virt_meth_0x490030_24)(this);
  }
  if (((int *)this->mbr_0x40 != (int *)0x0) &&
     (iVar2 = (**(code **)(*(int *)this->mbr_0x40 + 0x1c))(), iVar2 != 0)) {
    (**(code **)(*(int *)this->mbr_0x40 + 8))(in_stack_00000004);
  }
  piVar1 = (int *)this->mbr_0x44;
  if (((piVar1 != (int *)0x0) && (piVar1 != (int *)this->mbr_0x40)) &&
     (iVar2 = (**(code **)(*piVar1 + 0x1c))(), iVar2 != 0)) {
    (**(code **)(*(int *)this->mbr_0x44 + 8))(in_stack_00000004);
  }
  ExceptionList = local_14;
  return;
}



// Function at 0048f560

void TObjectInstance::meth_0x48f560()

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  puStack_c = &DAT_005a5f60;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  if (DAT_006682bc == 0) {
    local_8 = 0;
    ExceptionList = &local_14;
    if (((int *)this->mbr_0x40 == (int *)0x0) ||
       (ExceptionList = &local_14, iVar2 = (**(code **)(*(int *)this->mbr_0x40 + 0x20))(),
       iVar2 != 0)) {
      if (((int *)this->mbr_0x44 == (int *)0x0) ||
         (iVar2 = (**(code **)(*(int *)this->mbr_0x44 + 0x24))(), iVar2 != 0)) {
        uVar3 = 0;
      }
      else {
        uVar3 = *(uint *)(this->mbr_0x44 + 4);
      }
    }
    else {
      uVar3 = *(uint *)(this->mbr_0x40 + 4);
    }
    if ((uVar3 & 0x20) == 0) {
      FUN_00416f70();
      (*this->vftptr_0x0->virt_meth_0x490110_28)(this);
      FUN_00416fb0();
    }
    if (((int *)this->mbr_0x40 != (int *)0x0) &&
       (iVar2 = (**(code **)(*(int *)this->mbr_0x40 + 0x1c))(), iVar2 != 0)) {
      (**(code **)(*(int *)this->mbr_0x40 + 0xc))();
    }
    piVar1 = (int *)this->mbr_0x44;
    if (((piVar1 != (int *)0x0) && (piVar1 != (int *)this->mbr_0x40)) &&
       (iVar2 = (**(code **)(*piVar1 + 0x1c))(), iVar2 != 0)) {
      (**(code **)(*(int *)this->mbr_0x44 + 0xc))();
    }
  }
  ExceptionList = local_14;
  return;
}



// Function at 0048f680

void TObjectInstance::meth_0x48f680()

{
  int iVar1;
  uint uVar2;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  puStack_c = &DAT_005a5f70;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  if (DAT_006682bc == 0) {
    local_8 = 0;
    ExceptionList = &local_14;
    if (((int *)this->mbr_0x40 == (int *)0x0) ||
       (ExceptionList = &local_14, iVar1 = (**(code **)(*(int *)this->mbr_0x40 + 0x20))(),
       iVar1 != 0)) {
      if (((int *)this->mbr_0x44 == (int *)0x0) ||
         (iVar1 = (**(code **)(*(int *)this->mbr_0x44 + 0x24))(), iVar1 != 0)) {
        uVar2 = 0;
      }
      else {
        uVar2 = *(uint *)(this->mbr_0x44 + 4);
      }
    }
    else {
      uVar2 = *(uint *)(this->mbr_0x40 + 4);
    }
    if ((uVar2 & 0x20) == 0) {
      (*this->vftptr_0x0->virt_meth_0x4901e0_32)(this);
    }
  }
  ExceptionList = local_14;
  return;
}



// Function at 0048f760

void TObjectInstance::meth_0x48f760()

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &DAT_005a5f80;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  ExceptionList = &local_14;
  if ((int *)this->mbr_0x40 != (int *)0x0) {
    ExceptionList = &local_14;
    iVar2 = (**(code **)(*(int *)this->mbr_0x40 + 0x20))();
    if (iVar2 == 0) {
      uVar3 = *(uint *)(this->mbr_0x40 + 4);
      goto LAB_0048f7ba;
    }
  }
  if ((int *)this->mbr_0x44 != (int *)0x0) {
    iVar2 = (**(code **)(*(int *)this->mbr_0x44 + 0x24))();
    if (iVar2 == 0) {
      uVar3 = *(uint *)(this->mbr_0x44 + 4);
      goto LAB_0048f7ba;
    }
  }
  uVar3 = 0;
LAB_0048f7ba:
  if ((uVar3 & 0x10) == 0) {
    (*this->vftptr_0x0->virt_meth_0x4902c0_36)(this);
  }
  local_8 = 0;
  if ((int *)this->mbr_0x40 != (int *)0x0) {
    iVar2 = (**(code **)(*(int *)this->mbr_0x40 + 0x18))();
    if (iVar2 != 0) {
      (**(code **)(*(int *)this->mbr_0x40 + 0x14))();
    }
  }
  piVar1 = (int *)this->mbr_0x44;
  if ((piVar1 != (int *)0x0) && (piVar1 != (int *)this->mbr_0x40)) {
    iVar2 = (**(code **)(*piVar1 + 0x18))();
    if (iVar2 != 0) {
      (**(code **)(*(int *)this->mbr_0x44 + 0x14))();
    }
  }
  if ((DAT_0065bb14 != (TObjectInstance *)0x0) && (this != DAT_0065bb14)) {
    meth_0x48f760(DAT_0065bb14);
  }
  ExceptionList = local_14;
  return;
}



// Function at 0048f950

void TObjectInstance::meth_0x48f950(undefined4 param_1, undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &DAT_005a5fa0;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  if (DAT_006682bc == 0) {
    ExceptionList = &local_14;
    if (((int *)this->mbr_0x40 == (int *)0x0) ||
       (ExceptionList = &local_14, iVar1 = (**(code **)(*(int *)this->mbr_0x40 + 0x20))(),
       iVar1 != 0)) {
      if (((int *)this->mbr_0x44 == (int *)0x0) ||
         (iVar1 = (**(code **)(*(int *)this->mbr_0x44 + 0x24))(), iVar1 != 0)) {
        uVar2 = 0;
      }
      else {
        uVar2 = *(uint *)(this->mbr_0x44 + 4);
      }
    }
    else {
      uVar2 = *(uint *)(this->mbr_0x40 + 4);
    }
    if ((uVar2 & 8) == 0) {
      local_8 = 0;
      (*this->vftptr_0x0->virt_meth_0x490530_44)(this);
    }
    ExceptionList = local_14;
    return;
  }
  return;
}



// Function at 0048fcc0

void TObjectInstance::meth_0x48fcc0(undefined4 param_1)

{
  int iVar1;
  uint uVar2;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &DAT_005a5fd0;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  if (DAT_006682bc == 0) {
    ExceptionList = &local_14;
    if (((int *)this->mbr_0x40 == (int *)0x0) ||
       (ExceptionList = &local_14, iVar1 = (**(code **)(*(int *)this->mbr_0x40 + 0x20))(),
       iVar1 != 0)) {
      if (((int *)this->mbr_0x44 == (int *)0x0) ||
         (iVar1 = (**(code **)(*(int *)this->mbr_0x44 + 0x24))(), iVar1 != 0)) {
        uVar2 = 0;
      }
      else {
        uVar2 = *(uint *)(this->mbr_0x44 + 4);
      }
    }
    else {
      uVar2 = *(uint *)(this->mbr_0x40 + 4);
    }
    if ((uVar2 & 8) == 0) {
      local_8 = 0;
      (*this->vftptr_0x0->virt_meth_0x490860_56)(this);
    }
    ExceptionList = local_14;
    return;
  }
  return;
}



// Function at 0048fda0

void TObjectInstance::virt_meth_0x48fda0()

{
  dword dVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  if ((DAT_0066829c != 0) && (0 < (int)this->mbr_0x1c)) {
    if (DAT_00667fcc == (cls_0x5b4f30 *)0x0) goto LAB_0048fe30;
    if (((0x77 < (int)(this->mbr_0x48 - DAT_00668504)) &&
        (((DAT_00667fcc->TPlayScreen).TScreen.mbr_0x8 & 0x2800080) == 0)) &&
       ((*(byte *)&(DAT_00667fcc->TPlayScreen).mbr_0x36c & 2) == 0)) {
      cls_0x5b4f30::meth_0x51d680(DAT_00667fcc);
    }
  }
  if (((DAT_00667fcc != (cls_0x5b4f30 *)0x0) && (DAT_006682bc != 0)) &&
     (((*(byte *)&(DAT_00667fcc->TPlayScreen).mbr_0x36c & 2) != 0 &&
      (((DAT_00667fcc->TPlayScreen).TScreen.mbr_0x110 & 0x100000) != 0)))) {
    cls_0x5b4f30::meth_0x51d680(DAT_00667fcc);
  }
LAB_0048fe30:
  dVar1 = this->mbr_0x1c;
  if (((int)dVar1 < 1) || (((&this->field_0x2c)[dVar1 * 4] & 8) == 0)) {
    uVar4 = 0;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        dVar1 = (this->cls_0x41c7f0).mbr_0x10;
        if ((((dVar1 != 0) && (uVar4 < (this->cls_0x41c7f0).mbr_0x0)) &&
            (*(int *)(dVar1 + uVar4 * 4) != 0)) &&
           ((iVar3 = (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x3c))(),
            iVar3 == 0 &&
            (piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4), piVar2[0x11] == 0)))) {
          DAT_00666644 = piVar2;
          (**(code **)(*piVar2 + 0x24))(0);
          (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x4c))();
        }
        uVar4 = uVar4 + 1;
      } while ((int)uVar4 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
  }
  else {
    piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + (&this->mbr_0x1c)[dVar1] * 4);
    if (((piVar2 != (int *)0x0) && (iVar3 = (**(code **)(*piVar2 + 0x3c))(), iVar3 == 0)) &&
       (piVar2[0x11] == 0)) {
      DAT_00666644 = piVar2;
      (**(code **)(*piVar2 + 0x24))(0);
      (**(code **)(*piVar2 + 0x4c))();
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      return;
    }
  }
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  return;
}



// Function at 0048ff00

void TObjectInstance::virt_meth_0x48ff00()

{
  dword dVar1;
  int *piVar2;
  undefined *puVar3;
  int iVar4;
  uint uVar5;
  
  if (DAT_006682bc == 0) {
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    dVar1 = this->mbr_0x1c;
    if ((0 < (int)dVar1) && (((&this->field_0x2c)[dVar1 * 4] & 0x10) != 0)) {
      piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + (&this->mbr_0x1c)[dVar1] * 4);
      if (piVar2 != (int *)0x0) {
        iVar4 = (**(code **)(*piVar2 + 0x3c))();
        if ((iVar4 == 0) && (piVar2[0x11] == 0)) {
          DAT_00666644 = piVar2;
          if (this->mbr_0x50 != 0) {
            (**(code **)(*piVar2 + 0x28))();
          }
          (**(code **)(*piVar2 + 0x24))(0);
          (**(code **)(*piVar2 + 0x50))();
          (**(code **)(*piVar2 + 0x2c))(0);
        }
      }
      puVar3 = PTR_DAT_005d79e0;
      this->mbr_0x50 = 0;
      (**(code **)(*(int *)puVar3 + 0x24))();
      return;
    }
    uVar5 = 0;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        dVar1 = (this->cls_0x41c7f0).mbr_0x10;
        if (((dVar1 != 0) && (uVar5 < (this->cls_0x41c7f0).mbr_0x0)) &&
           (*(int *)(dVar1 + uVar5 * 4) != 0)) {
          iVar4 = (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar5 * 4) + 0x3c))();
          if ((iVar4 == 0) &&
             (piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar5 * 4), piVar2[0x11] == 0)) {
            if (this->mbr_0x50 != 0) {
              (**(code **)(*piVar2 + 0x28))();
            }
            DAT_00666644 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar5 * 4);
            (**(code **)(*DAT_00666644 + 0x24))(0);
            (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar5 * 4) + 0x50))();
            (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar5 * 4) + 0x2c))(0);
          }
        }
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
    puVar3 = PTR_DAT_005d79e0;
    this->mbr_0x50 = 0;
    (**(code **)(*(int *)puVar3 + 0x24))();
  }
  return;
}



// Function at 00490030

void TObjectInstance::virt_meth_0x490030()

{
  dword dVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  undefined4 unaff_retaddr;
  undefined4 in_stack_00000004;
  
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  dVar1 = this->mbr_0x1c;
  if ((0 < (int)dVar1) && (((&this->field_0x2c)[dVar1 * 4] & 0x20) != 0)) {
    piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + (&this->mbr_0x1c)[dVar1] * 4);
    if ((piVar2 != (int *)0x0) &&
       ((iVar3 = (**(code **)(*piVar2 + 0x3c))(), iVar3 == 0 && (piVar2[0x11] == 0)))) {
      DAT_00666644 = piVar2;
      (**(code **)(*piVar2 + 0x24))(0);
      (**(code **)(*piVar2 + 0x58))(unaff_retaddr);
    }
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    return;
  }
  uVar4 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      dVar1 = (this->cls_0x41c7f0).mbr_0x10;
      if ((((dVar1 != 0) && (uVar4 < (this->cls_0x41c7f0).mbr_0x0)) &&
          (*(int *)(dVar1 + uVar4 * 4) != 0)) &&
         ((iVar3 = (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x3c))(),
          iVar3 == 0 &&
          (piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4), piVar2[0x11] == 0)))) {
        DAT_00666644 = piVar2;
        (**(code **)(*piVar2 + 0x24))(0);
        (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x58))
                  (in_stack_00000004);
      }
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  return;
}



// Function at 004901e0

void TObjectInstance::virt_meth_0x4901e0()

{
  dword dVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  
  if (DAT_006682bc == 0) {
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    dVar1 = this->mbr_0x1c;
    if ((0 < (int)dVar1) && (((&this->field_0x2c)[dVar1 * 4] & 0x80) != 0)) {
      piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + (&this->mbr_0x1c)[dVar1] * 4);
      if (piVar2 != (int *)0x0) {
        iVar3 = (**(code **)(*piVar2 + 0x3c))();
        if ((iVar3 == 0) && (piVar2[0x11] == 0)) {
          DAT_00666644 = piVar2;
          (**(code **)(*piVar2 + 0x24))(0);
          (**(code **)(*piVar2 + 0x5c))();
        }
      }
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      return;
    }
    uVar4 = 0;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        dVar1 = (this->cls_0x41c7f0).mbr_0x10;
        if (((dVar1 != 0) && (uVar4 < (this->cls_0x41c7f0).mbr_0x0)) &&
           (*(int *)(dVar1 + uVar4 * 4) != 0)) {
          iVar3 = (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x3c))();
          if ((iVar3 == 0) &&
             (piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4), piVar2[0x11] == 0)) {
            DAT_00666644 = piVar2;
            (**(code **)(*piVar2 + 0x24))(0);
            (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x5c))();
          }
        }
        uVar4 = uVar4 + 1;
      } while ((int)uVar4 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  }
  return;
}



// Function at 004903b0

void TObjectInstance::virt_meth_0x4903b0()

{
  dword dVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  undefined4 unaff_retaddr;
  int in_stack_00000004;
  int in_stack_00000008;
  int in_stack_0000000c;
  
  if ((DAT_006682bc == 0) &&
     ((((int *)this->mbr_0x40 == (int *)0x0 ||
       (iVar3 = (**(code **)(*(int *)this->mbr_0x40 + 0x18))(), iVar3 == 0)) ||
      ((*(byte *)(this->mbr_0x40 + 4) & 8) == 0)))) {
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    dVar1 = this->mbr_0x1c;
    if ((0 < (int)dVar1) && (((&this->field_0x2c)[dVar1 * 4] & 1) != 0)) {
      piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + (&this->mbr_0x1c)[dVar1] * 4);
      if ((piVar2 != (int *)0x0) &&
         ((iVar3 = (**(code **)(*piVar2 + 0x3c))(), iVar3 == 0 && (piVar2[0x11] == 0)))) {
        DAT_00666644 = piVar2;
        (**(code **)(*piVar2 + 0x24))(0);
        (**(code **)(*piVar2 + 100))
                  (unaff_retaddr,in_stack_00000004 - piVar2[1],in_stack_00000008 - piVar2[2]);
      }
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      return;
    }
    uVar5 = 0;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        dVar1 = (this->cls_0x41c7f0).mbr_0x10;
        if ((((dVar1 != 0) && (uVar5 < (this->cls_0x41c7f0).mbr_0x0)) &&
            (*(int *)(dVar1 + uVar5 * 4) != 0)) &&
           (((iVar3 = (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar5 * 4) + 0x3c))(),
             iVar3 == 0 &&
             (piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar5 * 4), piVar2[0x11] == 0)) &&
            (iVar3 = (**(code **)(*piVar2 + 0x44))(), iVar3 == 0)))) {
          DAT_00666644 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar5 * 4);
          (**(code **)(*DAT_00666644 + 0x24))(0);
          piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar5 * 4);
          iVar3 = in_stack_00000008 - piVar2[1];
          iVar4 = in_stack_0000000c - piVar2[2];
          if ((((-1 < iVar3) && (-1 < iVar4)) && ((iVar3 < piVar2[3] && (iVar4 < piVar2[4])))) ||
             (((in_stack_00000004 == 4 || (in_stack_00000004 == 5)) || (in_stack_00000004 == 6)))) {
            (**(code **)(*piVar2 + 100))(in_stack_00000004,iVar3,iVar4);
          }
        }
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  }
  return;
}



// Function at 00490660

void TObjectInstance::virt_meth_0x490660()

{
  dword dVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  undefined4 in_stack_00000004;
  undefined4 in_stack_00000008;
  
  if (DAT_006682bc == 0) {
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    dVar1 = this->mbr_0x1c;
    if ((0 < (int)dVar1) && (((&this->field_0x2c)[dVar1 * 4] & 2) != 0)) {
      piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + (&this->mbr_0x1c)[dVar1] * 4);
      if ((piVar2 != (int *)0x0) &&
         ((iVar3 = (**(code **)(*piVar2 + 0x3c))(), iVar3 == 0 && (piVar2[0x11] == 0)))) {
        DAT_00666644 = piVar2;
        (**(code **)(*piVar2 + 0x6c))(in_stack_00000004,in_stack_00000008);
      }
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      return;
    }
    uVar4 = 0;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        dVar1 = (this->cls_0x41c7f0).mbr_0x10;
        if ((((dVar1 != 0) && (uVar4 < (this->cls_0x41c7f0).mbr_0x0)) &&
            (*(int *)(dVar1 + uVar4 * 4) != 0)) &&
           (((iVar3 = (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x3c))(),
             iVar3 == 0 &&
             (piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4), piVar2[0x11] == 0)) &&
            (iVar3 = (**(code **)(*piVar2 + 0x44))(), iVar3 == 0)))) {
          DAT_00666644 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4);
          (**(code **)(*DAT_00666644 + 0x24))(0);
          (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x6c))
                    (in_stack_00000004,in_stack_00000008);
        }
        uVar4 = uVar4 + 1;
      } while ((int)uVar4 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  }
  return;
}



// Function at 00490760

void TObjectInstance::virt_meth_0x490760()

{
  dword dVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  undefined4 in_stack_00000004;
  undefined4 in_stack_00000008;
  
  if (DAT_006682bc == 0) {
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    dVar1 = this->mbr_0x1c;
    if ((0 < (int)dVar1) && (((&this->field_0x2c)[dVar1 * 4] & 2) != 0)) {
      piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + (&this->mbr_0x1c)[dVar1] * 4);
      if ((piVar2 != (int *)0x0) &&
         ((iVar3 = (**(code **)(*piVar2 + 0x3c))(), iVar3 == 0 && (piVar2[0x11] == 0)))) {
        DAT_00666644 = piVar2;
        (**(code **)(*piVar2 + 0x70))(in_stack_00000004,in_stack_00000008);
      }
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      return;
    }
    uVar4 = 0;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        dVar1 = (this->cls_0x41c7f0).mbr_0x10;
        if ((((dVar1 != 0) && (uVar4 < (this->cls_0x41c7f0).mbr_0x0)) &&
            (*(int *)(dVar1 + uVar4 * 4) != 0)) &&
           (((iVar3 = (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x3c))(),
             iVar3 == 0 &&
             (piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4), piVar2[0x11] == 0)) &&
            (iVar3 = (**(code **)(*piVar2 + 0x44))(), iVar3 == 0)))) {
          DAT_00666644 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4);
          (**(code **)(*DAT_00666644 + 0x24))(0);
          (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x70))
                    (in_stack_00000004,in_stack_00000008);
        }
        uVar4 = uVar4 + 1;
      } while ((int)uVar4 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  }
  return;
}



// Function at 00490860

void TObjectInstance::virt_meth_0x490860()

{
  dword dVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  undefined4 in_stack_00000004;
  undefined4 in_stack_00000008;
  
  if (DAT_006682bc == 0) {
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
    dVar1 = this->mbr_0x1c;
    if ((0 < (int)dVar1) && (((&this->field_0x2c)[dVar1 * 4] & 4) != 0)) {
      piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + (&this->mbr_0x1c)[dVar1] * 4);
      if ((piVar2 != (int *)0x0) &&
         ((iVar3 = (**(code **)(*piVar2 + 0x3c))(), iVar3 == 0 && (piVar2[0x11] == 0)))) {
        DAT_00666644 = piVar2;
        (**(code **)(*piVar2 + 0x74))(in_stack_00000004,in_stack_00000008);
      }
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
      return;
    }
    uVar4 = 0;
    if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
      do {
        dVar1 = (this->cls_0x41c7f0).mbr_0x10;
        if ((((dVar1 != 0) && (uVar4 < (this->cls_0x41c7f0).mbr_0x0)) &&
            (*(int *)(dVar1 + uVar4 * 4) != 0)) &&
           (((iVar3 = (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x3c))(),
             iVar3 == 0 &&
             (piVar2 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4), piVar2[0x11] == 0)) &&
            (iVar3 = (**(code **)(*piVar2 + 0x44))(), iVar3 == 0)))) {
          DAT_00666644 = *(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4);
          (**(code **)(*DAT_00666644 + 0x24))(0);
          (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar4 * 4) + 0x74))
                    (in_stack_00000004,in_stack_00000008);
        }
        uVar4 = uVar4 + 1;
      } while ((int)uVar4 < (int)(this->cls_0x41c7f0).mbr_0x0);
    }
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
  }
  return;
}



// Function at 00490960

void TObjectInstance::virt_meth_0x490960(int param_1)

{
  dword dVar1;
  uint uVar2;
  undefined4 in_stack_00000008;
  
  if (param_1 == 0x103) {
    FUN_00416fb0();
  }
  uVar2 = 0;
  if (0 < (int)(this->cls_0x41c7f0).mbr_0x0) {
    do {
      dVar1 = (this->cls_0x41c7f0).mbr_0x10;
      if (((dVar1 != 0) && (uVar2 < (this->cls_0x41c7f0).mbr_0x0)) &&
         (*(int *)(dVar1 + uVar2 * 4) != 0)) {
        (**(code **)(**(int **)((this->cls_0x41c7f0).mbr_0x10 + uVar2 * 4) + 0x78))
                  (param_1,in_stack_00000008);
      }
      uVar2 = uVar2 + 1;
    } while ((int)uVar2 < (int)(this->cls_0x41c7f0).mbr_0x0);
  }
  return;
}



// Function at 00491870

void TObjectInstance::virt_meth_0x491870()

{
  int *piVar1;
  int iVar2;
  
  if (DAT_006682bc == 0) {
    meth_0x48f340(this);
    cls_0x4aa0c0::meth_0x4aa930((cls_0x4aa0c0 *)PTR_DAT_005d79e0);
    meth_0x48f450(this);
    meth_0x48f560(this);
    meth_0x48f680(this);
    FUN_0043a480();
    if ((int *)this->mbr_0x40 != (int *)0x0) {
      iVar2 = (**(code **)(*(int *)this->mbr_0x40 + 0x1c))();
      if (iVar2 != 0) {
        (**(code **)(*(int *)this->mbr_0x40 + 0x10))();
      }
    }
    piVar1 = (int *)this->mbr_0x44;
    if ((piVar1 != (int *)0x0) && (piVar1 != (int *)this->mbr_0x40)) {
      iVar2 = (**(code **)(*piVar1 + 0x1c))();
      if (iVar2 != 0) {
        (**(code **)(*(int *)this->mbr_0x44 + 0x10))();
      }
    }
    cls_0x4aa0c0::meth_0x4aa0c0((cls_0x4aa0c0 *)PTR_DAT_005d79e0);
  }
  return;
}



// Function at 00492060

TObjectInstance * TObjectInstance::virt_meth_0x492060()

{
  byte in_stack_00000004;
  
  ~TObjectInstance(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 00533cb0

TObjectInstance * __thiscall TObjectInstance::~TObjectInstance(TObjectInstance *this)

{
  LPCVOID pvVar1;
  TObjectInstance *pcVar2;
  
  pvVar1 = (LPCVOID)(this->cls_0x41c7f0).mbr_0x10;
  this->vftptr_0x0 = &TObjectInstance__vftable_5a5ed4_005a5ed4;
  pcVar2 = (TObjectInstance *)FUN_004830f0(pvVar1);
  return pcVar2;
}



