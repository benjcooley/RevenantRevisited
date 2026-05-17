#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a4f28



// Function at 0046a510

undefined4 cls_0x5a4f28::virt_meth_0x46a510()

{
  dword dVar1;
  cls_0x5a4494__vftable_5a4494 *pcVar2;
  cls_0x46d6b0 *this_00;
  int iVar3;
  cls_0x5a4494_TPane *this_01;
  void *unaff_EBX;
  cls_0x419dd0 cStack_2c;
  cls_0x419dd0 local_14;
  cls_0x419dd0 *local_10;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 uStack_4;
  
  this_00 = DAT_0065abc0;
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059d165;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->TObjectInstance).mbr_0x54 = 0;
  cStack_2c.mbr_0x0 = 0x46a543;
  iVar3 = cls_0x46d6b0::meth_0x46d710(this_00);
  cStack_2c.mbr_0x0 = 0x46a549;
  FUN_0043a020(iVar3);
  dVar1 = this->mbr_0x70;
  cStack_2c.mbr_0x0 = 0;
  cls_0x419dd0::cls_0x419dd0(&local_14);
  local_10 = &cStack_2c;
  cls_0x419dd0::cls_0x419dd0(&cStack_2c);
  (**(code **)(dVar1 + 0x30))(8);
  (this->TObjectInstance).mbr_0x40 = (dword)&this->mbr_0x70;
  (this->TObjectInstance).mbr_0x44 = (dword)&this->mbr_0x70;
  this_01 = (cls_0x5a4494_TPane *)FUN_00482fb0(400);
  local_10 = (cls_0x419dd0 *)0x0;
  if (this_01 == (cls_0x5a4494_TPane *)0x0) {
    this_01 = (cls_0x5a4494_TPane *)0x0;
  }
  else {
    this_01->mbr_0xc = 0x280;
    this_01->mbr_0x1c = 0x280;
    this_01->vftptr_0x0 = &cls_0x5a4494__vftable_5a4494_005a4494;
    this_01->mbr_0x4 = 0;
    this_01->mbr_0x14 = 0;
    this_01->mbr_0x8 = 0;
    this_01->mbr_0x18 = 0;
    this_01->mbr_0x10 = 0x1e0;
    this_01->mbr_0x20 = 0x1e0;
    this_01->mbr_0x48 = 0;
    this_01->mbr_0x4c = 0;
    this_01->mbr_0x50 = 0;
    this_01->mbr_0x54 = 0;
    cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this_01->field_0x88,0x10);
    this_01->vftptr_0x0 = (cls_0x5a4494__vftable_5a4494 *)&cls_0x5a4b2c__vftable_5a4b2c_005a4b2c;
  }
  pcVar2 = this_01->vftptr_0x0;
  local_10 = (cls_0x419dd0 *)0xffffffff;
  this->mbr_0x88 = (dword)this_01;
  (*pcVar2->virt_meth_0x491900_0)(this_01);
  cls_0x5b96f4::meth_0x48ed90((cls_0x5b96f4 *)this);
  ExceptionList = unaff_EBX;
  return 1;
}



// Function at 0046a620

void cls_0x5a4f28::virt_meth_0x46a620()

{
  meth_0x48ee10(this);
  (**(code **)(*(int *)this->mbr_0x88 + 4))();
  if ((int *)this->mbr_0x88 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x88 + 0x7c))(1);
  }
  this->mbr_0x88 = 0;
  return;
}



// Function at 0046d410

cls_0x5a4f28::cls_0x5a4f28()

{
  TObjectInstance::TObjectInstance(&this->TObjectInstance);
  this->mbr_0x70 = (dword)&cls_0x5a4c30__vftable_5a4c30_005a4c30;
  this->mbr_0x78 = 0;
  this->mbr_0x7c = 0;
  this->mbr_0x80 = 0;
  this->mbr_0x74 = 0;
  (this->TObjectInstance).vftptr_0x0 =
       (TObjectInstance__vftable_5a5ed4 *)&cls_0x5a4f28__vftable_5a4f28_005a4f28;
  return this;
}



// Function at 0046d440

cls_0x5a4f28 * cls_0x5a4f28::virt_meth_0x46d440()

{
  byte in_stack_00000004;
  
  TObjectInstance::~TObjectInstance(&this->TObjectInstance);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0048ee10

undefined4 cls_0x5a4f28::meth_0x48ee10()

{
  cls_0x41c7f0 *this_00;
  dword dVar1;
  int iVar2;
  int *piVar3;
  int in_stack_00000004;
  cls_0x492080 local_c;
  
  this_00 = &(this->TObjectInstance).cls_0x41c7f0;
  cls_0x492080::meth_0x492080(&local_c);
  while( true ) {
    if (((uint *)local_c.mbr_0x0 == (uint *)0x0) || (*(uint *)local_c.mbr_0x0 <= local_c.mbr_0x8))
    goto LAB_0048ee5b;
    if (in_stack_00000004 == *(int *)local_c.mbr_0x4) break;
    local_c.mbr_0x4 = local_c.mbr_0x4 + 4;
    local_c.mbr_0x8 = local_c.mbr_0x8 + 1;
  }
  if (-1 < (int)local_c.mbr_0x8) {
    meth_0x48ef30(this);
  }
LAB_0048ee5b:
  dVar1 = this_00->mbr_0x0;
  iVar2 = 0;
  if (0 < (int)dVar1) {
    piVar3 = (int *)(this->TObjectInstance).cls_0x41c7f0.mbr_0x10;
    do {
      if (*piVar3 == in_stack_00000004) {
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
        *(undefined4 *)(in_stack_00000004 + 0x3c) = 0;
        return 1;
      }
      iVar2 = iVar2 + 1;
      piVar3 = piVar3 + 1;
    } while (iVar2 < (int)dVar1);
  }
  return 0;
}



// Function at 0048ef30

void cls_0x5a4f28::meth_0x48ef30()

{
  bool bVar1;
  uint *puVar2;
  int iVar3;
  uint *puVar4;
  uint in_stack_00000004;
  dword local_4;
  
  iVar3 = 0;
  bVar1 = false;
  local_4 = 0;
  if (0 < (int)(this->TObjectInstance).mbr_0x1c) {
    puVar2 = (uint *)&(this->TObjectInstance).field_0x30;
    puVar4 = puVar2;
    do {
      if (puVar4[-4] == in_stack_00000004) {
        if ((*puVar4 & 0x100) != 0) {
          (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x46cf30_60)(this);
        }
        bVar1 = true;
      }
      else {
        puVar2[-4] = puVar4[-4];
        *puVar2 = *puVar4;
        local_4 = local_4 + 1;
        puVar2 = puVar2 + 1;
      }
      iVar3 = iVar3 + 1;
      puVar4 = puVar4 + 1;
    } while (iVar3 < (int)(this->TObjectInstance).mbr_0x1c);
  }
  (this->TObjectInstance).mbr_0x1c = local_4;
  if (bVar1) {
    (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490960_64)(&this->TObjectInstance,0x102);
  }
  if (0 < (int)(this->TObjectInstance).mbr_0x1c) {
    (*((this->TObjectInstance).vftptr_0x0)->virt_meth_0x490960_64)(&this->TObjectInstance,0x101);
  }
  return;
}



