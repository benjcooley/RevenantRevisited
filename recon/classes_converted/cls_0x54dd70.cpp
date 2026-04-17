#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x54dd70



// Function at 0054dd70

void cls_0x54dd70::meth_0x54dd70(dword param_1, dword param_2, dword param_3, dword param_4)

{
  dword dVar1;
  int iVar2;
  dword in_stack_00000014;
  
  if (in_stack_00000014 != 0xcdcdcdcd) {
    this->mbr_0x28 = in_stack_00000014;
  }
  if (param_1 != 0xcdcdcdcd) {
    this->mbr_0x0 = this->mbr_0x0 + (param_1 - this->mbr_0x18);
  }
  if (param_3 != 0xcdcdcdcd) {
    this->mbr_0x4 = this->mbr_0x4 + (param_3 - this->mbr_0x20);
  }
  if (param_1 != 0xcdcdcdcd) {
    this->mbr_0x18 = param_1;
  }
  if (param_2 != 0xcdcdcdcd) {
    this->mbr_0x1c = param_2;
  }
  if (param_3 != 0xcdcdcdcd) {
    this->mbr_0x20 = param_3;
  }
  if (param_4 != 0xcdcdcdcd) {
    this->mbr_0x24 = param_4;
  }
  dVar1 = this->mbr_0x18;
  this->mbr_0x8 = (this->mbr_0x1c - dVar1) + 1;
  iVar2 = (this->mbr_0x28 * this->mbr_0x20 + dVar1) * 2;
  this->mbr_0xc = ((this->mbr_0x24 - this->mbr_0x20) + 1) * this->mbr_0x28 - dVar1;
  this->mbr_0x10 = this->mbr_0x34 + iVar2;
  this->mbr_0x14 = this->mbr_0x38 + iVar2;
  return;
}



// Function at 0054de20

void cls_0x54dd70::meth_0x54de20(dword param_1)

{
  dword dVar1;
  int iVar2;
  dword in_stack_00000008;
  
  if (param_1 != 0xcdcdcdcd) {
    this->mbr_0x34 = param_1;
  }
  if (in_stack_00000008 != 0xcdcdcdcd) {
    this->mbr_0x38 = in_stack_00000008;
  }
  dVar1 = this->mbr_0x18;
  this->mbr_0x8 = (this->mbr_0x1c - dVar1) + 1;
  iVar2 = (this->mbr_0x28 * this->mbr_0x20 + dVar1) * 2;
  this->mbr_0xc = ((this->mbr_0x24 - this->mbr_0x20) + 1) * this->mbr_0x28 - dVar1;
  this->mbr_0x10 = this->mbr_0x34 + iVar2;
  this->mbr_0x14 = this->mbr_0x38 + iVar2;
  return;
}



// Function at 0054de80

void cls_0x54dd70::meth_0x54de80(dword param_1)

{
  dword dVar1;
  int iVar2;
  dword in_stack_00000008;
  
  if (param_1 != 0xcdcdcdcd) {
    this->mbr_0x0 = this->mbr_0x0 + (this->mbr_0x2c - param_1);
  }
  if (in_stack_00000008 != 0xcdcdcdcd) {
    this->mbr_0x4 = this->mbr_0x4 + (this->mbr_0x30 - in_stack_00000008);
  }
  if (param_1 != 0xcdcdcdcd) {
    this->mbr_0x2c = param_1;
  }
  if (in_stack_00000008 != 0xcdcdcdcd) {
    this->mbr_0x30 = in_stack_00000008;
  }
  dVar1 = this->mbr_0x18;
  this->mbr_0x8 = (this->mbr_0x1c - dVar1) + 1;
  iVar2 = (this->mbr_0x28 * this->mbr_0x20 + dVar1) * 2;
  this->mbr_0xc = ((this->mbr_0x24 - this->mbr_0x20) + 1) * this->mbr_0x28 - dVar1;
  this->mbr_0x10 = this->mbr_0x34 + iVar2;
  this->mbr_0x14 = this->mbr_0x38 + iVar2;
  return;
}



