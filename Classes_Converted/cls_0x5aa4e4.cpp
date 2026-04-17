#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5aa4e4



// Function at 004e7d80

void cls_0x5aa4e4::virt_meth_0x4e7d80()

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  
  this->mbr_0x104 = 0;
  this->mbr_0x100 = 0;
  this->mbr_0x10c = 0x3e4ccccd;
  this->mbr_0x108 = 0x3e4ccccd;
  this->virt_meth_0x40dd60();
  iVar3 = 0;
  do {
    cls_0x5a7e38::meth_0x4e7c60((cls_0x5a7e38 *)this);
    iVar3 = iVar3 + 1;
  } while (iVar3 < 0x32);
  if (DAT_005d79e4 < 3) {
    piVar1 = (int *)this->mbr_0x4;
    iVar3 = piVar1[0x36];
    if ((((iVar3 != 0) && (*(int *)(iVar3 + 0x11c) != 0)) &&
        (*(int *)(*(int *)(iVar3 + 0x11c) + 200) != 0)) &&
       ((0 < *(int *)(iVar3 + 0xc) && (*(int *)(iVar3 + 0x10) != 0)))) {
      if (*(int *)(iVar3 + 0xc) < 1) {
        piVar2 = (int *)0x0;
      }
      else {
        piVar2 = *(int **)(iVar3 + 0x10);
      }
      (**(code **)(*piVar2 + 0x1b4))(0);
    }
    (**(code **)(*piVar1 + 0x40))(piVar1[2] | 0x1000);
    (**(code **)(*piVar1 + 0x40))(piVar1[2] | 0x8000);
    if (piVar1[0x36] != 0) {
      iVar3 = (**(code **)(*piVar1 + 0x3c))();
      if (iVar3 != 0) {
        (**(code **)(*(int *)piVar1[0x36] + 0xc))();
      }
    }
  }
  this->mbr_0xfc = 0;
  return;
}



// Function at 004f6670

cls_0x5aa4e4 * cls_0x5aa4e4::virt_meth_0x4f6670()

{
  byte in_stack_00000004;
  
  ~cls_0x5aa4e4(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004f6690

cls_0x5aa4e4 * __thiscall cls_0x5aa4e4::~cls_0x5aa4e4(cls_0x5aa4e4 *this)

{
  cls_0x5b0074 *pcVar1;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059f378;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->vftptr_0x0 = &cls_0x5aa4e4__vftable_5aa4e4_005aa4e4;
  local_4 = 0;
  this->virt_meth_0x40de10();
  local_4 = 0xffffffff;
  pcVar1 = cls_0x5b0074::~cls_0x5b0074((cls_0x5b0074 *)this);
  ExceptionList = local_c;
  return (cls_0x5aa4e4 *)pcVar1;
}



