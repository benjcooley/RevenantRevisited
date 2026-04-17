#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x48f870



// Function at 0048f870

void cls_0x48f870::meth_0x48f870(undefined4 param_1, undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  undefined4 in_stack_0000000c;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &DAT_005a5f90;
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
      (**(code **)(this->mbr_0x0 + 0x28))(param_1,param_2,in_stack_0000000c);
    }
    ExceptionList = local_14;
    return;
  }
  return;
}



// Function at 0048fa30

void cls_0x48f870::meth_0x48fa30(int param_1)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  undefined4 in_stack_00000008;
  void *local_14;
  undefined *puStack_10;
  undefined *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &DAT_005a5fb0;
  puStack_10 = &LAB_0058b6b8;
  local_14 = ExceptionList;
  if (DAT_006682bc != 0) {
    return;
  }
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
  if ((uVar3 & 8) == 0) {
    local_8 = 0;
    if (param_1 == 0x1b) {
      if ((DAT_00666920 == 0) && (iVar2 = (**(code **)(DAT_006668d8 + 0x40))(), iVar2 != 0)) {
        bVar1 = false;
      }
      else {
        bVar1 = true;
      }
      if (!bVar1) {
        if ((DAT_0066f790 == 0) && (iVar2 = (**(code **)(DAT_0066f748 + 0x40))(), iVar2 != 0)) {
          bVar1 = false;
        }
        else {
          bVar1 = true;
        }
        if (bVar1) {
          (**(code **)(this->mbr_0x0 + 0x28))(5,1,1);
          (**(code **)(this->mbr_0x0 + 0x30))(0x26,0);
          (**(code **)(this->mbr_0x0 + 0x30))(0x25,0);
          (**(code **)(this->mbr_0x0 + 0x30))(0x27,0);
          (**(code **)(this->mbr_0x0 + 0x30))(0x28,0);
          (**(code **)(this->mbr_0x0 + 0x30))(0x52,0);
          (**(code **)(this->mbr_0x0 + 0x30))(0x22,0);
          (**(code **)(this->mbr_0x0 + 0x30))(0x21,0);
          (**(code **)(this->mbr_0x0 + 0x30))(0x24,0);
          (**(code **)(this->mbr_0x0 + 0x30))(0x23,0);
        }
      }
    }
    (**(code **)(this->mbr_0x0 + 0x30))(param_1,in_stack_00000008);
    ExceptionList = local_14;
    return;
  }
  ExceptionList = local_14;
  return;
}



