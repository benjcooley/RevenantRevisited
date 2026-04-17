#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5b9fcc



// Function at 00574460

cls_0x5b9fcc::cls_0x5b9fcc *this,dword *param_1,undefined4 *param_2)

{
  dword *this_00;
  dword dVar1;
  char cVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  char *pcVar8;
  undefined4 *puVar9;
  undefined4 *in_stack_0000000c;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a2459;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = (cls_0x5b9fcc__vftable_5b9fcc *)&cls_0x5b9fac__vftable_5b9fac_005b9fac;
  this_00 = &this->mbr_0x4;
  local_4 = 0;
  *(undefined *)this_00 = param_1._0_1_;
  cls_0x57bd90::meth_0x57bd90((cls_0x57bd90 *)this_00);
  local_4._0_1_ = 1;
  *(undefined *)&this->mbr_0x14 = param_1._0_1_;
  cls_0x57bd90::meth_0x57bd90((cls_0x57bd90 *)&this->mbr_0x14);
  this->mbr_0x28 = *param_1;
  local_4 = CONCAT31(local_4._1_3_,2);
  dVar1 = param_1[1];
  this->vftptr_0x0 = (cls_0x5b9fcc__vftable_5b9fcc *)&cls_0x5b9f88__vftable_5b9f88_005b9f88;
  this->mbr_0x2c = dVar1;
  this->mbr_0x30 = param_1[2];
  this->mbr_0x34 = param_1[3];
  uVar3 = 0xffffffff;
  puVar7 = param_2;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar2 = *(char *)puVar7;
    puVar7 = (undefined4 *)((int)puVar7 + 1);
  } while (cVar2 != '\0');
  uVar3 = ~uVar3 - 1;
  cVar2 = cls_0x57bd90::meth_0x57bf80((cls_0x57bd90 *)this_00,(char)uVar3);
  if (cVar2 != '\0') {
    puVar7 = (undefined4 *)this->mbr_0x8;
    for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *puVar7 = *param_2;
      param_2 = param_2 + 1;
      puVar7 = puVar7 + 1;
    }
    for (uVar4 = uVar3 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined *)puVar7 = *(undefined *)param_2;
      param_2 = (undefined4 *)((int)param_2 + 1);
      puVar7 = (undefined4 *)((int)puVar7 + 1);
    }
    cls_0x57bd90::meth_0x57bf60((cls_0x57bd90 *)this_00);
  }
  uVar4 = 0xffffffff;
  puVar7 = in_stack_0000000c;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar2 = *(char *)puVar7;
    puVar7 = (undefined4 *)((int)puVar7 + 1);
  } while (cVar2 != '\0');
  uVar4 = ~uVar4 - 1;
  cVar2 = cls_0x57bd90::meth_0x57bf80((cls_0x57bd90 *)&this->mbr_0x14,(char)uVar4);
  if (cVar2 != '\0') {
    puVar7 = (undefined4 *)this->mbr_0x18;
    puVar6 = puVar7;
    for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      *puVar6 = *in_stack_0000000c;
      in_stack_0000000c = in_stack_0000000c + 1;
      puVar6 = puVar6 + 1;
    }
    this->mbr_0x1c = uVar4;
    for (uVar5 = uVar4 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
      *(undefined *)puVar6 = *(undefined *)in_stack_0000000c;
      in_stack_0000000c = (undefined4 *)((int)in_stack_0000000c + 1);
      puVar6 = (undefined4 *)((int)puVar6 + 1);
    }
    *(undefined *)((int)puVar7 + uVar4) = 0;
  }
  this->mbr_0x24 = 1;
  this->mbr_0x38 = 0;
  *(char *)&this->mbr_0x3c = (char)uVar3;
  this->mbr_0x40 = 0;
  this->mbr_0x44 = 0;
  this->mbr_0x48 = 0;
  uVar3 = 0xffffffff;
  pcVar8 = &DAT_006769dc;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar2 = *pcVar8;
    pcVar8 = pcVar8 + 1;
  } while (cVar2 != '\0');
  uVar3 = ~uVar3 - 1;
  local_4 = 4;
  this->vftptr_0x0 = &cls_0x5b9fcc__vftable_5b9fcc_005b9fcc;
  cVar2 = cls_0x57bd90::meth_0x57bf80((cls_0x57bd90 *)&this->mbr_0x3c,(char)uVar3);
  if (cVar2 != '\0') {
    puVar7 = (undefined4 *)this->mbr_0x40;
    puVar6 = (undefined4 *)&DAT_006769dc;
    puVar9 = puVar7;
    for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *puVar9 = *puVar6;
      puVar6 = puVar6 + 1;
      puVar9 = puVar9 + 1;
    }
    for (uVar4 = uVar3 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined *)puVar9 = *(undefined *)puVar6;
      puVar6 = (undefined4 *)((int)puVar6 + 1);
      puVar9 = (undefined4 *)((int)puVar9 + 1);
    }
    this->mbr_0x44 = uVar3;
    *(undefined *)((int)puVar7 + uVar3) = 0;
  }
  ExceptionList = local_c;
  return this;
}



// Function at 0057b940

cls_0x5b9fcc * cls_0x5b9fcc::virt_meth_0x57b940()

{
  byte in_stack_00000004;
  
  ~cls_0x5b9fcc(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 0057b960

cls_0x5b9fcc * __thiscall cls_0x5b9fcc::~cls_0x5b9fcc(cls_0x5b9fcc *this)

{
  char cVar1;
  dword dVar2;
  cls_0x5b9fcc *pcVar3;
  
  dVar2 = this->mbr_0x40;
  if (dVar2 != 0) {
    cVar1 = *(char *)(dVar2 - 1);
    if ((cVar1 == '\0') || (cVar1 == -1)) {
      FUN_004830f0((char *)(dVar2 - 1));
    }
    else {
      *(char *)(dVar2 - 1) = cVar1 + -1;
    }
  }
  this->mbr_0x40 = 0;
  this->mbr_0x44 = 0;
  this->mbr_0x48 = 0;
  this->vftptr_0x0 = (cls_0x5b9fcc__vftable_5b9fcc *)&cls_0x5b9f88__vftable_5b9f88_005b9f88;
  if ((HGLOBAL)this->mbr_0x38 != (HGLOBAL)0x0) {
    GlobalFree((HGLOBAL)this->mbr_0x38);
    this->mbr_0x38 = 0;
  }
  dVar2 = this->mbr_0x18;
  if (dVar2 != 0) {
    cVar1 = *(char *)(dVar2 - 1);
    if ((cVar1 == '\0') || (cVar1 == -1)) {
      FUN_004830f0((char *)(dVar2 - 1));
    }
    else {
      *(char *)(dVar2 - 1) = cVar1 + -1;
    }
  }
  this->mbr_0x18 = 0;
  this->mbr_0x1c = 0;
  pcVar3 = (cls_0x5b9fcc *)this->mbr_0x8;
  this->mbr_0x20 = 0;
  if (pcVar3 != (cls_0x5b9fcc *)0x0) {
    cVar1 = *(char *)((int)&pcVar3[-1].mbr_0x48 + 3);
    pcVar3 = (cls_0x5b9fcc *)((int)&pcVar3[-1].mbr_0x48 + 3);
    if ((cVar1 != '\0') && (cVar1 != -1)) {
      this->mbr_0x8 = 0;
      this->mbr_0xc = 0;
      this->mbr_0x10 = 0;
      this->vftptr_0x0 = (cls_0x5b9fcc__vftable_5b9fcc *)&cls_0x5b9fac__vftable_5b9fac_005b9fac;
      *(char *)&pcVar3->vftptr_0x0 = cVar1 + -1;
      return pcVar3;
    }
    pcVar3 = (cls_0x5b9fcc *)FUN_004830f0(pcVar3);
  }
  this->mbr_0x8 = 0;
  this->mbr_0xc = 0;
  this->mbr_0x10 = 0;
  this->vftptr_0x0 = (cls_0x5b9fcc__vftable_5b9fcc *)&cls_0x5b9fac__vftable_5b9fac_005b9fac;
  return pcVar3;
}



