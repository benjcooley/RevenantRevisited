#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x4741b0



// Function at 004741b0

Alignment * cls_0x4741b0::meth_0x4741b0(uint param_1)

{
  Alignment *in_stack_00000008;
  
  if ((uint)(int)(short)this->mbr_0x0 <= param_1) {
    return &DAT_0065a264;
  }
  FUN_0058b100((char *)in_stack_00000008,(byte *)s__s__s__d__d__d_005d4cc4);
  return in_stack_00000008;
}



// Function at 00474210

uint cls_0x4741b0::meth_0x474210()

{
  uint uVar1;
  cls_0x4741b0 *pcVar2;
  cls_0x4741b0 *extraout_ECX;
  byte *pbVar3;
  uint uVar4;
  byte *in_stack_00000004;
  
  pbVar3 = (byte *)this->mbr_0x4;
  pcVar2 = this;
  for (uVar4 = 0; (this != (cls_0x4741b0 *)0x0 && (uVar4 < (uint)(int)(short)this->mbr_0x0));
      uVar4 = uVar4 + 1) {
    uVar1 = FUN_0059a530((uint)pcVar2,pbVar3,in_stack_00000004);
    if (uVar1 == 0) {
      return uVar4;
    }
    pbVar3 = pbVar3 + 0x50;
    pcVar2 = extraout_ECX;
  }
  return 0xffffffff;
}



// Function at 004780c0

int cls_0x4741b0::meth_0x4780c0()

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  undefined4 *in_stack_00000004;
  
  if ((short)this->mbr_0x2 <= (short)this->mbr_0x0) {
    iVar5 = (short)this->mbr_0x2 + 4;
    puVar1 = FUN_00482fb0(iVar5 * 0x50);
    iVar2 = (int)(short)this->mbr_0x2;
    puVar7 = puVar1 + iVar2 * 0x14;
    for (uVar3 = (uint)((iVar5 - iVar2) * 0x50) >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar7 = 0;
      puVar7 = puVar7 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar7 = 0;
      puVar7 = (undefined4 *)((int)puVar7 + 1);
    }
    puVar7 = (undefined4 *)this->mbr_0x4;
    if (puVar7 != (undefined4 *)0x0) {
      puVar6 = puVar7;
      puVar8 = puVar1;
      for (uVar3 = (uint)(iVar2 * 0x50) >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
        *puVar8 = *puVar6;
        puVar6 = puVar6 + 1;
        puVar8 = puVar8 + 1;
      }
      for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
        *(undefined *)puVar8 = *(undefined *)puVar6;
        puVar6 = (undefined4 *)((int)puVar6 + 1);
        puVar8 = (undefined4 *)((int)puVar8 + 1);
      }
      FUN_004830f0(puVar7);
    }
    this->mbr_0x2 = (word)iVar5;
    this->mbr_0x4 = (dword)puVar1;
  }
  puVar7 = (undefined4 *)((short)this->mbr_0x0 * 0x50 + this->mbr_0x4);
  for (iVar2 = 0x14; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar7 = *in_stack_00000004;
    in_stack_00000004 = in_stack_00000004 + 1;
    puVar7 = puVar7 + 1;
  }
  this->mbr_0x0 = this->mbr_0x0 + 1;
  return (short)this->mbr_0x0 + -1;
}



// Function at 00478170

void cls_0x4741b0::meth_0x478170(undefined4 *param_1)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  word wVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  uint in_stack_00000008;
  
  wVar5 = this->mbr_0x2;
  while ((uint)(int)(short)wVar5 <= in_stack_00000008) {
    iVar6 = (short)this->mbr_0x2 + 4;
    puVar1 = FUN_00482fb0(iVar6 * 0x50);
    iVar2 = (int)(short)this->mbr_0x2;
    puVar8 = puVar1 + iVar2 * 0x14;
    for (uVar3 = (uint)((iVar6 - iVar2) * 0x50) >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar8 = 0;
      puVar8 = puVar8 + 1;
    }
    for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
      *(undefined *)puVar8 = 0;
      puVar8 = (undefined4 *)((int)puVar8 + 1);
    }
    puVar8 = (undefined4 *)this->mbr_0x4;
    if (puVar8 != (undefined4 *)0x0) {
      puVar7 = puVar8;
      puVar9 = puVar1;
      for (uVar3 = (uint)(iVar2 * 0x50) >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
        *puVar9 = *puVar7;
        puVar7 = puVar7 + 1;
        puVar9 = puVar9 + 1;
      }
      for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
        *(undefined *)puVar9 = *(undefined *)puVar7;
        puVar7 = (undefined4 *)((int)puVar7 + 1);
        puVar9 = (undefined4 *)((int)puVar9 + 1);
      }
      FUN_004830f0(puVar8);
    }
    wVar5 = (word)iVar6;
    this->mbr_0x2 = wVar5;
    this->mbr_0x4 = (dword)puVar1;
  }
  wVar5 = this->mbr_0x0;
  puVar8 = (undefined4 *)(in_stack_00000008 * 0x50 + this->mbr_0x4);
  for (iVar2 = 0x14; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar8 = *param_1;
    param_1 = param_1 + 1;
    puVar8 = puVar8 + 1;
  }
  if ((int)(short)wVar5 <= (int)in_stack_00000008) {
    this->mbr_0x0 = (short)in_stack_00000008 + 1;
  }
  return;
}



// Function at 00478520

void cls_0x4741b0::meth_0x478520()

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  int in_stack_00000004;
  
  if (in_stack_00000004 < 0) {
    in_stack_00000004 = (short)this->mbr_0x2 + 4;
  }
  else if (in_stack_00000004 < (short)this->mbr_0x2) {
    return;
  }
  puVar1 = FUN_00482fb0(in_stack_00000004 * 0x50);
  iVar2 = (int)(short)this->mbr_0x2;
  puVar6 = puVar1 + iVar2 * 0x14;
  for (uVar3 = (uint)((in_stack_00000004 - iVar2) * 0x50) >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
    *puVar6 = 0;
    puVar6 = puVar6 + 1;
  }
  for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
    *(undefined *)puVar6 = 0;
    puVar6 = (undefined4 *)((int)puVar6 + 1);
  }
  puVar6 = (undefined4 *)this->mbr_0x4;
  if (puVar6 != (undefined4 *)0x0) {
    puVar5 = puVar6;
    puVar7 = puVar1;
    for (uVar3 = (uint)(iVar2 * 0x50) >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar7 = *puVar5;
      puVar5 = puVar5 + 1;
      puVar7 = puVar7 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined *)puVar7 = *(undefined *)puVar5;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
      puVar7 = (undefined4 *)((int)puVar7 + 1);
    }
    FUN_004830f0(puVar6);
  }
  this->mbr_0x4 = (dword)puVar1;
  this->mbr_0x2 = (word)in_stack_00000004;
  return;
}



