#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a5978



// Function at 00488460

cls_0x5a5978::cls_0x5a5978()

{
  this->mbr_0x4 = 0x1c4;
  this->mbr_0x14 = 0x1c4;
  this->mbr_0x8 = 0x132;
  this->mbr_0x18 = 0x132;
  this->mbr_0xc = 0xbc;
  this->mbr_0x1c = 0xbc;
  this->vftptr_0x0 = (cls_0x5a5978__vftable_5a5978 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x10 = 0xae;
  this->mbr_0x20 = 0xae;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a5978__vftable_5a5978_005a5978;
  return this;
}



// Function at 004884c0

cls_0x5a5978 * __thiscall cls_0x5a5978::~cls_0x5a5978(cls_0x5a5978 *this)

{
  LPCVOID pvVar1;
  SIZE_T SVar2;
  cls_0x5a5978 *pcVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059d696;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5a5978__vftable_5a5978 *)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 0;
  this->virt_meth_0x434f30();
  this->vftptr_0x0 = (cls_0x5a5978__vftable_5a5978 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  this->virt_meth_0x434f30();
  pvVar1 = *(LPCVOID *)&this->field_0x98;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  pcVar3 = (cls_0x5a5978 *)FUN_0058cfab(pvVar1);
  ExceptionList = local_c;
  return pcVar3;
}



// Function at 00488540

cls_0x5a5978 * cls_0x5a5978::virt_meth_0x488540()

{
  SIZE_T SVar1;
  byte in_stack_00000004;
  
  ~cls_0x5a5978(this);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a5978 *)0x0) {
      SVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - SVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 005432a0

undefined4 cls_0x5a5978::virt_meth_0x5432a0()

{
  cls_0x46d6b0 *pcVar1;
  dword dVar2;
  cls_0x5a3c68 *pcVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1e97;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->virt_meth_0x434e40();
  dVar2 = cls_0x46d6b0::meth_0x46d710(DAT_0065a574);
  this->mbr_0x18c = dVar2;
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x150);
  pcVar1 = DAT_0065a574;
  local_4 = 0;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065a574);
    uVar5 = cls_0x46d6b0::meth_0x46d710(pcVar1);
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,s_spell_005e5100,0x18,0x88,0x8e,0x24,0,&LAB_00542d50,uVar4,uVar5,0,0,1,
               0xffffffff,0,0);
    (pcVar3->cls_0x5a3ab8).vftptr_0x0 =
         (cls_0x5a3ab8__vftable_5a3ab8 *)&cls_0x5b9bf0__vftable_5b9bf0_005b9bf0;
    pcVar3[1].cls_0x5a3ab8.vftptr_0x0 = (cls_0x5a3ab8__vftable_5a3ab8 *)0x0;
    *(undefined4 *)&pcVar3[1].cls_0x5a3ab8.field_0x4 = 7;
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  pcVar1 = DAT_0065a574;
  uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065a574);
  uVar5 = cls_0x46d6b0::meth_0x46d710(pcVar1);
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 1;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005e5120,0xa4,0x8f,0x16,0x14,8,&LAB_00542e30,uVar4,uVar5,0,0,0,0xffffffff
               ,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  pcVar1 = DAT_0065a574;
  uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065a574);
  uVar5 = cls_0x46d6b0::meth_0x46d710(pcVar1);
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 2;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005e5140,0xa6,0x52,0x14,0x1a,0,&LAB_00542f00,uVar4,uVar5,0,0,0,0xffffffff
               ,8,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  pcVar1 = DAT_0065a574;
  uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065a574);
  uVar5 = cls_0x46d6b0::meth_0x46d710(pcVar1);
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 3;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005e515c,0xa6,0x70,0x14,0x1a,0,&LAB_00542ed0,uVar4,uVar5,0,0,0,0xffffffff
               ,8,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  pcVar1 = DAT_0065a574;
  uVar4 = cls_0x46d6b0::meth_0x46d710(DAT_0065a574);
  uVar5 = cls_0x46d6b0::meth_0x46d710(pcVar1);
  pcVar3 = (cls_0x5a3c68 *)FUN_00482fb0(0x148);
  local_4 = 4;
  if (pcVar3 != (cls_0x5a3c68 *)0x0) {
    cls_0x5a3c68::cls_0x5a3c68
              (pcVar3,&DAT_005e517c,0xa6,0x25,0x12,0x12,0,&LAB_00542f30,uVar4,uVar5,0,0,0,0xffffffff
               ,0,0);
  }
  local_4 = 0xffffffff;
  cls_0x5b98b8::meth_0x436ae0((cls_0x5b98b8 *)this);
  this->mbr_0x17c = 1;
  this->mbr_0x180 = 0;
  this->mbr_0x184 = 0xffffffff;
  (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
  ExceptionList = pvStack_c;
  return 1;
}



// Function at 005435c0

void cls_0x5a5978::virt_meth_0x5435c0()

{
  if (this->mbr_0x50 != 0) {
    (*this->vftptr_0x0->virt_meth_0x5435f0_84)(this);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  this->virt_meth_0x435de0();
  return;
}



// Function at 00543c40

void cls_0x5a5978::virt_meth_0x543c40(int param_1, undefined4 param_2)

{
  dword dVar1;
  dword dVar2;
  uint uVar3;
  
  this->virt_meth_0x436660(param_1,param_2);
  if ((param_1 == 1) && (-1 < (int)this->mbr_0x184)) {
    dVar1 = this->mbr_0x188;
    dVar2 = cls_0x5b93c4::meth_0x543ca0((cls_0x5b93c4 *)this,param_2);
    uVar3 = (uint)(dVar2 == this->mbr_0x184);
    this->mbr_0x188 = uVar3;
    if (dVar1 != uVar3) {
      (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
    }
  }
  return;
}



