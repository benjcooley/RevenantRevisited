#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x56f720



// Function at 0056f720

void cls_0x56f720::meth_0x56f720(undefined4 *param_1, uint param_2)

{
  dword dVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  uint uVar9;
  undefined4 *puVar10;
  undefined4 *in_stack_0000000c;
  
  puVar7 = (undefined4 *)this->mbr_0x8;
  if (param_2 <= (uint)((int)(this->mbr_0xc - (int)puVar7) >> 2)) {
    uVar9 = (int)puVar7 - (int)param_1 >> 2;
    if (uVar9 < param_2) {
      puVar8 = param_1 + param_2;
      if (param_1 != puVar7) {
        puVar6 = puVar8 + -param_2;
        do {
          if (puVar8 != (undefined4 *)0x0) {
            *puVar8 = *puVar6;
          }
          puVar6 = puVar6 + 1;
          puVar8 = puVar8 + 1;
        } while (puVar6 != puVar7);
      }
      puVar8 = puVar7;
      for (iVar4 = param_2 - uVar9; iVar4 != 0; iVar4 = iVar4 + -1) {
        if (puVar8 != (undefined4 *)0x0) {
          *puVar8 = *in_stack_0000000c;
        }
        puVar8 = puVar8 + 1;
      }
      if (param_1 != puVar7) {
        uVar2 = *in_stack_0000000c;
        do {
          *param_1 = uVar2;
          param_1 = param_1 + 1;
        } while (param_1 != puVar7);
      }
      this->mbr_0x8 = (dword)(puVar7 + param_2);
      return;
    }
    if (param_2 != 0) {
      puVar10 = puVar7 + -param_2;
      puVar8 = puVar7;
      for (puVar6 = puVar10; puVar3 = puVar7, puVar6 != puVar7; puVar6 = puVar6 + 1) {
        if (puVar8 != (undefined4 *)0x0) {
          *puVar8 = *puVar6;
        }
        puVar8 = puVar8 + 1;
      }
      for (; param_1 != puVar10; puVar10 = puVar10 + -1) {
        puVar3[-1] = puVar10[-1];
        puVar3 = puVar3 + -1;
      }
      puVar8 = param_1 + param_2;
      if (param_1 != puVar8) {
        uVar2 = *in_stack_0000000c;
        do {
          *param_1 = uVar2;
          param_1 = param_1 + 1;
        } while (param_1 != puVar8);
      }
      this->mbr_0x8 = (dword)(puVar7 + param_2);
    }
    return;
  }
  dVar1 = this->mbr_0x4;
  if ((dVar1 == 0) || (uVar9 = (int)((int)puVar7 - dVar1) >> 2, uVar9 <= param_2)) {
    uVar9 = param_2;
  }
  if (dVar1 == 0) {
    iVar4 = 0;
  }
  else {
    iVar4 = (int)((int)puVar7 - dVar1) >> 2;
  }
  iVar4 = iVar4 + uVar9;
  iVar5 = iVar4;
  if (iVar4 < 0) {
    iVar5 = 0;
  }
  puVar6 = FUN_00482fb0(iVar5 << 2);
  puVar8 = puVar6;
  for (puVar7 = (undefined4 *)this->mbr_0x4; puVar7 != param_1; puVar7 = puVar7 + 1) {
    if (puVar8 != (undefined4 *)0x0) {
      *puVar8 = *puVar7;
    }
    puVar8 = puVar8 + 1;
  }
  puVar7 = puVar8;
  uVar9 = param_2;
  if (param_2 != 0) {
    do {
      if (puVar7 != (undefined4 *)0x0) {
        *puVar7 = *in_stack_0000000c;
      }
      uVar9 = uVar9 - 1;
      puVar7 = puVar7 + 1;
    } while (uVar9 != 0);
  }
  puVar10 = (undefined4 *)this->mbr_0x8;
  puVar7 = puVar8 + param_2;
  if (param_1 != puVar10) {
    puVar8 = (undefined4 *)((int)puVar7 + (param_2 * -4 - (int)puVar8) + (int)param_1);
    do {
      if (puVar7 != (undefined4 *)0x0) {
        *puVar7 = *puVar8;
      }
      puVar8 = puVar8 + 1;
      puVar7 = puVar7 + 1;
    } while (puVar8 != puVar10);
  }
  FUN_004830f0((LPCVOID)this->mbr_0x4);
  this->mbr_0xc = (dword)(puVar6 + iVar4);
  dVar1 = this->mbr_0x4;
  if (dVar1 == 0) {
    this->mbr_0x4 = (dword)puVar6;
    this->mbr_0x8 = (dword)(puVar6 + param_2);
    return;
  }
  this->mbr_0x4 = (dword)puVar6;
  this->mbr_0x8 = (dword)(puVar6 + ((int)(this->mbr_0x8 - dVar1) >> 2) + param_2);
  return;
}



