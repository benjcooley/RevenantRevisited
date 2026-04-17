#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x414310



// Function at 00414310

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x414310::meth_0x414310()

{
  int iVar1;
  int iVar2;
  int iVar3;
  int in_stack_00000004;
  
  if (this->mbr_0x0 != 0) {
    DAT_005e87dc = in_stack_00000004;
    iVar3 = (DAT_005c61c0 * in_stack_00000004 * 8) / 100;
    iVar1 = (DAT_005e8848 >> 0x10 & 0xff) * iVar3;
    iVar2 = (DAT_005e8848 >> 8 & 0xff) * iVar3;
    iVar3 = (DAT_005e8848 & 0xff) * iVar3;
    in_stack_00000004 =
         (((int)(iVar3 + (iVar3 >> 0x1f & 0xffU)) >> 8) +
          ((int)(iVar2 + (iVar2 >> 0x1f & 0xffU)) >> 8) +
         ((int)(iVar1 + (iVar1 >> 0x1f & 0xffU)) >> 8)) / 3;
    if (0xfe < in_stack_00000004) {
      in_stack_00000004 = 0xff;
    }
    DAT_005e8914 = (float)in_stack_00000004 * _DAT_005a3830;
  }
  return;
}



// Function at 004143d0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x414310::meth_0x4143d0()

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte *pbVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  byte bVar10;
  byte bVar11;
  uint uVar12;
  byte *in_stack_00000004;
  
  pbVar4 = in_stack_00000004;
  if (this->mbr_0x0 != 0) {
    bVar1 = in_stack_00000004[1];
    bVar2 = *in_stack_00000004;
    in_stack_00000004 = (byte *)(uint)bVar2;
    bVar11 = bVar1;
    if (bVar1 <= bVar2) {
      bVar11 = bVar2;
    }
    bVar3 = pbVar4[2];
    bVar10 = bVar3;
    if ((bVar3 <= bVar11) && (bVar10 = bVar1, bVar1 <= bVar2)) {
      bVar10 = bVar2;
    }
    if (bVar10 == 0) {
      uVar12 = 1;
    }
    else {
      bVar11 = bVar1;
      if (bVar1 <= bVar2) {
        bVar11 = bVar2;
      }
      bVar10 = bVar3;
      if ((bVar3 <= bVar11) && (bVar10 = bVar2, bVar2 < bVar1)) {
        bVar10 = bVar1;
      }
      uVar12 = (uint)bVar10;
    }
    uVar5 = (uint)(((ulonglong)bVar3 << 8) / (ulonglong)(longlong)(int)uVar12);
    if (0xfe < uVar5) {
      uVar5 = 0xff;
    }
    uVar6 = (uint)(((ulonglong)bVar1 << 8) / (ulonglong)(longlong)(int)uVar12);
    if (0xfe < uVar6) {
      uVar6 = 0xff;
    }
    uVar12 = (uint)((int)in_stack_00000004 << 8) / uVar12;
    if (0xfe < uVar12) {
      uVar12 = 0xff;
    }
    DAT_005e8848 = ((uVar5 & 0xff | 0xffffff00) << 8 | uVar6 & 0xff) << 8 | uVar12 & 0xff;
    iVar9 = (DAT_005c61c0 * DAT_005e87dc * 8) / 100;
    iVar7 = (uVar5 & 0xff) * iVar9;
    iVar8 = (uVar6 & 0xff) * iVar9;
    iVar9 = (uVar12 & 0xff) * iVar9;
    in_stack_00000004 =
         (byte *)((((int)(iVar9 + (iVar9 >> 0x1f & 0xffU)) >> 8) +
                   ((int)(iVar8 + (iVar8 >> 0x1f & 0xffU)) >> 8) +
                  ((int)(iVar7 + (iVar7 >> 0x1f & 0xffU)) >> 8)) / 3);
    if (0xfe < (int)in_stack_00000004) {
      in_stack_00000004 = (byte *)0xff;
    }
    DAT_005e8914 = (float)(int)in_stack_00000004 * _DAT_005a3830;
  }
  return;
}



