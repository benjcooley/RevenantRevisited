#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x57d820



// Function at 0057d820

cls_0x57d820 * __thiscall cls_0x57d820::~cls_0x57d820(cls_0x57d820 *this)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  cls_0x588410 *pcVar4;
  cls_0x588050 *this_00;
  dword dVar5;
  cls_0x57d820 *pcVar6;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_005a28ca;
  local_c = ExceptionList;
  local_4 = 2;
  ExceptionList = &local_c;
  while ((*(int *)this->mbr_0x80 != 0 &&
         (piVar1 = (int *)((int *)this->mbr_0x80)[2], piVar1 != (int *)0x0))) {
    iVar2 = *piVar1;
    if (iVar2 != 0) {
      piVar3 = (int *)piVar1[1];
      *piVar1 = 0;
      *(int **)(iVar2 + 4) = piVar3;
      *piVar3 = iVar2;
    }
    FUN_004830f0(piVar1);
  }
  while ((*(int *)this->mbr_0x8c != 0 &&
         (piVar1 = (int *)((int *)this->mbr_0x8c)[2], piVar1 != (int *)0x0))) {
    iVar2 = *piVar1;
    if (iVar2 != 0) {
      piVar3 = (int *)piVar1[1];
      *piVar1 = 0;
      *(int **)(iVar2 + 4) = piVar3;
      *piVar3 = iVar2;
    }
    FUN_004830f0(piVar1);
  }
  piVar1 = (int *)this->mbr_0x8c;
  iVar2 = *piVar1;
  while (iVar2 != 0) {
    iVar2 = *piVar1;
    if (iVar2 != 0) {
      piVar3 = (int *)piVar1[1];
      *piVar1 = 0;
      *(int **)(iVar2 + 4) = piVar3;
      *piVar3 = iVar2;
    }
    iVar2 = *piVar1;
  }
  piVar1 = (int *)this->mbr_0x80;
  iVar2 = *piVar1;
  while (iVar2 != 0) {
    iVar2 = *piVar1;
    if (iVar2 != 0) {
      piVar3 = (int *)piVar1[1];
      *piVar1 = 0;
      *(int **)(iVar2 + 4) = piVar3;
      *piVar3 = iVar2;
    }
    iVar2 = *piVar1;
  }
  this->mbr_0x0 = (dword)&cls_0x5ba0b4__vftable_5ba0b4_005ba0b4;
  local_4 = 0;
  while ((*(int *)this->mbr_0x48 != 0 &&
         (pcVar4 = (cls_0x588410 *)((int *)this->mbr_0x48)[2], pcVar4 != (cls_0x588410 *)0x0))) {
    local_4._0_1_ = 6;
    cls_0x588050::meth_0x588050((cls_0x588050 *)&pcVar4->mbr_0x1c);
    local_4 = CONCAT31(local_4._1_3_,5);
    cls_0x588410::meth_0x588480(pcVar4);
    FUN_004830f0(pcVar4);
  }
  while( true ) {
    if ((*(int *)this->mbr_0x3c == 0) ||
       (pcVar4 = (cls_0x588410 *)((int *)this->mbr_0x3c)[2], pcVar4 == (cls_0x588410 *)0x0)) break;
    local_4._0_1_ = 7;
    cls_0x588050::meth_0x588050((cls_0x588050 *)&pcVar4->mbr_0x1c);
    local_4 = CONCAT31(local_4._1_3_,5);
    cls_0x588410::meth_0x588480(pcVar4);
    FUN_004830f0(pcVar4);
  }
  this_00 = (cls_0x588050 *)this->mbr_0x48;
  local_4 = CONCAT31(local_4._1_3_,4);
  dVar5 = this_00->mbr_0x0;
  while (dVar5 != 0) {
    cls_0x588050::meth_0x588050(this_00);
    this_00 = (cls_0x588050 *)this->mbr_0x48;
    dVar5 = this_00->mbr_0x0;
  }
  piVar1 = (int *)this->mbr_0x3c;
  iVar2 = *piVar1;
  while (iVar2 != 0) {
    iVar2 = *piVar1;
    if (iVar2 != 0) {
      piVar3 = (int *)piVar1[1];
      *piVar1 = 0;
      *(int **)(iVar2 + 4) = piVar3;
      *piVar3 = iVar2;
    }
    iVar2 = *piVar1;
  }
  local_4 = 0xffffffff;
  pcVar6 = (cls_0x57d820 *)cls_0x5ba0a0::~cls_0x5ba0a0((cls_0x5ba0a0 *)this);
  ExceptionList = local_c;
  return pcVar6;
}



