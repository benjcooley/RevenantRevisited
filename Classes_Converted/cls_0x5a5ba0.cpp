#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a5ba0



// Function at 00488910

cls_0x5a5ba0::cls_0x5a5ba0()

{
  this->mbr_0xc = 0xbc;
  this->mbr_0x1c = 0xbc;
  this->mbr_0x4 = 0x1c4;
  this->mbr_0x14 = 0x1c4;
  this->mbr_0x10 = 0x132;
  this->mbr_0x20 = 0x132;
  this->vftptr_0x0 = (cls_0x5a5ba0__vftable_5a5ba0 *)&cls_0x5a4494__vftable_5a4494_005a4494;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x48 = 0;
  this->mbr_0x4c = 0;
  this->mbr_0x50 = 0;
  this->mbr_0x54 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&this->field_0x88,0x10);
  this->vftptr_0x0 = &cls_0x5a5ba0__vftable_5a5ba0_005a5ba0;
  return this;
}



// Function at 00488970

cls_0x5a5ba0 * __thiscall cls_0x5a5ba0::~cls_0x5a5ba0(cls_0x5a5ba0 *this)

{
  LPCVOID pvVar1;
  SIZE_T SVar2;
  cls_0x5a5ba0 *pcVar3;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d6f6;
  local_c = ExceptionList;
  pvVar1 = (LPCVOID)this->mbr_0x19c;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5a5ba0__vftable_5a5ba0_005a5ba0;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  FUN_0058cfab(pvVar1);
  this->mbr_0x19c = 0;
  this->vftptr_0x0 = (cls_0x5a5ba0__vftable_5a5ba0 *)&PTR_virt_meth_0x434e40_005a45c8;
  local_4 = 0;
  this->virt_meth_0x434f30();
  this->vftptr_0x0 = (cls_0x5a5ba0__vftable_5a5ba0 *)&cls_0x5a4510__vftable_5a4510_005a4510;
  local_4 = 1;
  this->virt_meth_0x434f30();
  pvVar1 = *(LPCVOID *)&this->field_0x98;
  if (pvVar1 != (LPCVOID)0x0) {
    SVar2 = FUN_0058cff3(pvVar1);
    DAT_0065ba08 = DAT_0065ba08 - SVar2;
  }
  pcVar3 = (cls_0x5a5ba0 *)FUN_0058cfab(pvVar1);
  ExceptionList = local_c;
  return pcVar3;
}



// Function at 00488ae0

cls_0x5a5ba0 * cls_0x5a5ba0::virt_meth_0x488ae0()

{
  SIZE_T SVar1;
  byte in_stack_00000004;
  
  ~cls_0x5a5ba0(this);
  if ((in_stack_00000004 & 1) != 0) {
    if (this != (cls_0x5a5ba0 *)0x0) {
      SVar1 = FUN_0058cff3(this);
      DAT_0065ba08 = DAT_0065ba08 - SVar1;
    }
    FUN_0058cfab(this);
  }
  return this;
}



// Function at 00546b50

undefined4 cls_0x5a5ba0::virt_meth_0x546b50()

{
  dword dVar1;
  cls_0x5a68d8__vftable_5a68d8 *pcVar2;
  byte *pbVar3;
  byte bVar4;
  cls_0x5a68d8 *pcVar5;
  int *piVar6;
  DWORD DVar7;
  undefined4 *puVar8;
  undefined3 extraout_var;
  dword local_114 [65];
  void *pvStack_10;
  void *pvStack_c;
  undefined4 uStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  uStack_8 = &LAB_005a1fb2;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  this->virt_meth_0x434e40();
  pcVar5 = (cls_0x5a68d8 *)FUN_00482fb0(0x78);
  local_4 = 0;
  if (pcVar5 == (cls_0x5a68d8 *)0x0) {
    pcVar5 = (cls_0x5a68d8 *)0x0;
  }
  else {
    local_114[0] = this->mbr_0xc;
    dVar1 = this->mbr_0x10;
    cls_0x5a68d8::cls_0x5a68d8(pcVar5);
    local_4 = CONCAT31(local_4._1_3_,1);
    pcVar5->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3980__vftable_5a3980_005a3980;
    pcVar5->mbr_0x68 = 0;
    cls_0x5a68d8::meth_0x4a5740(pcVar5,local_114[0],dVar1);
    pcVar5->mbr_0x70 = 1;
  }
  pcVar2 = pcVar5->vftptr_0x0;
  local_4 = 0xffffffff;
  this->mbr_0x18c = (dword)pcVar5;
  (*pcVar2->virt_meth_0x419e60_64)(pcVar5,this->mbr_0x4);
  pcVar5 = (cls_0x5a68d8 *)FUN_00482fb0(0x6c);
  uStack_8 = (undefined *)0x2;
  if (pcVar5 == (cls_0x5a68d8 *)0x0) {
    pcVar5 = (cls_0x5a68d8 *)0x0;
  }
  else {
    cls_0x46d6b0::meth_0x46d710(DAT_0065bc3c);
    cls_0x5a68d8::cls_0x5a68d8(pcVar5);
    uStack_8 = (undefined *)CONCAT31(uStack_8._1_3_,3);
    pcVar5->vftptr_0x0 = (cls_0x5a68d8__vftable_5a68d8 *)&cls_0x5a3ee4__vftable_5a3ee4_005a3ee4;
    cls_0x5a68d8::meth_0x4a3a40(pcVar5);
  }
  uStack_8 = (undefined *)0xffffffff;
  this->mbr_0x188 = (dword)pcVar5;
  FUN_0058b100((char *)local_114,&DAT_005e53b4);
  piVar6 = FUN_004a13f0((char *)local_114,(uint *)&DAT_005e53bc,0);
  if (piVar6 == (int *)0x0) {
    FUN_00481c10((byte *)s_Unable_to_load_STATPANE_DEF_005e53c0);
  }
  DVar7 = FUN_004a17b0(piVar6);
  this->mbr_0x1a4 = DVar7;
  if ((int)DVar7 < 1) {
    this->mbr_0x1a0 = 0;
    this->mbr_0x1a4 = 0;
  }
  else {
    puVar8 = FUN_00482fb0(DVar7 + 1);
    this->mbr_0x1a0 = (dword)puVar8;
    FUN_004a15a0(puVar8,this->mbr_0x1a4,1,piVar6);
    dVar1 = this->mbr_0x1a4;
    pbVar3 = (byte *)this->mbr_0x1a0;
    pbVar3[dVar1] = 0;
    bVar4 = FUN_004834e0(pbVar3,dVar1);
    if (CONCAT31(extraout_var,bVar4) != 0) {
      FUN_00483540((byte *)this->mbr_0x1a0,this->mbr_0x1a4);
    }
  }
  this->mbr_0x1a8 = 0;
  this->mbr_0x17c = 0;
  this->mbr_0x184 = 0;
  this->mbr_0x190 = 0;
  this->mbr_0x194 = 0;
  this->mbr_0x1ac = 0;
  this->mbr_0x1b0 = 0;
  this->mbr_0x19c = 0;
  (*this->vftptr_0x0->virt_meth_0x5496a0_184)(this);
  ExceptionList = pvStack_10;
  return 1;
}



// Function at 00546d70

void cls_0x5a5ba0::virt_meth_0x546d70()

{
  FUN_004830f0((LPCVOID)this->mbr_0x1a0);
  this->mbr_0x1a0 = 0;
  this->mbr_0x1a4 = 0;
  if ((undefined4 *)this->mbr_0x18c != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x18c)(1);
    this->mbr_0x18c = 0;
  }
  if ((undefined4 *)this->mbr_0x188 != (undefined4 *)0x0) {
    (***(code ***)(undefined4 *)this->mbr_0x188)(1);
    this->mbr_0x188 = 0;
  }
  if ((LPCVOID)this->mbr_0x19c != (LPCVOID)0x0) {
    FUN_00482f80((LPCVOID)this->mbr_0x19c);
  }
  this->mbr_0x19c = 0;
  this->virt_meth_0x434f30();
  return;
}



// Function at 00549010

void cls_0x5a5ba0::virt_meth_0x549010()

{
  if (this->mbr_0x50 != 0) {
    (*this->vftptr_0x0->virt_meth_0x5491c0_84)(this);
    (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  }
  return;
}



// Function at 005493b0

void cls_0x5a5ba0::virt_meth_0x5493b0(int param_1, int param_2)

{
  int in_stack_0000000c;
  
  if (param_1 == 2) {
    (*this->vftptr_0x0->virt_meth_0x5496a0_184)(this);
    this->virt_meth_0x436530(2,param_2);
    return;
  }
  if ((((param_1 == 4) && (-1 < param_2)) && (-1 < in_stack_0000000c)) &&
     ((param_2 < (int)this->mbr_0xc && (in_stack_0000000c < (int)this->mbr_0x10)))) {
    FUN_0043a100(0,0,0);
    FUN_0043a140((int *)0x0);
    FUN_0043a170(&UNK_00670228.field_0x420);
    FUN_0043a240((undefined4 *)0x0,0,0);
    (*this->vftptr_0x0->virt_meth_0x445210_40)(this);
  }
  this->virt_meth_0x436530(param_1,param_2);
  return;
}



// Function at 005496a0

void cls_0x5a5ba0::virt_meth_0x5496a0()

{
  char cVar1;
  undefined4 *puVar2;
  uint uVar3;
  uint uVar4;
  undefined4 *puVar5;
  char *pcVar6;
  char *pcVar7;
  undefined4 *puVar8;
  int *in_stack_00000004;
  undefined4 uVar9;
  
  if ((int *)this->mbr_0x198 != (int *)0x0) {
    (**(code **)(*(int *)this->mbr_0x198 + 0xa0))();
  }
  this->mbr_0x198 = (dword)in_stack_00000004;
  if ((LPCVOID)this->mbr_0x19c != (LPCVOID)0x0) {
    FUN_00482f80((LPCVOID)this->mbr_0x19c);
  }
  uVar3 = 0xffffffff;
  pcVar6 = s_page1_005e5714;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  puVar2 = FUN_00482ef0(~uVar3);
  uVar3 = 0xffffffff;
  pcVar6 = s_page1_005e570c;
  do {
    pcVar7 = pcVar6;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar7 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar7;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  uVar9 = 1;
  puVar5 = (undefined4 *)(pcVar7 + -uVar3);
  puVar8 = puVar2;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar8 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar8 = puVar8 + 1;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined *)puVar8 = *(undefined *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + 1);
    puVar8 = (undefined4 *)((int)puVar8 + 1);
  }
  this->mbr_0x19c = (dword)puVar2;
  (*this->vftptr_0x0->virt_meth_0x444fc0_44)(this);
  if (in_stack_00000004 != (int *)0x0) {
    (**(code **)(*in_stack_00000004 + 0x9c))(uVar9);
  }
  return;
}



