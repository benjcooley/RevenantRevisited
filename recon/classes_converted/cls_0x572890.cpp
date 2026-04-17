#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x572890



// Function at 00572890

void cls_0x572890::meth_0x572890(int param_1)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  undefined4 *puVar4;
  byte bVar5;
  int iVar6;
  dword dVar7;
  undefined4 *puVar8;
  byte *pbVar9;
  dword in_stack_00000008;
  
  if (param_1 < 0) {
    param_1 = 0;
  }
  else if ((int)this->mbr_0x1c <= param_1) {
    param_1 = this->mbr_0x1c - 1;
  }
  if ((int)in_stack_00000008 < 0) {
    dVar7 = 0;
  }
  else {
    dVar7 = this->mbr_0x1c;
    if ((int)in_stack_00000008 <= (int)dVar7) goto LAB_005728ca;
  }
  in_stack_00000008 = dVar7;
LAB_005728ca:
  puVar4 = (undefined4 *)(((int)((param_1 >> 0x1f & 7U) + param_1) >> 3) + this->mbr_0x38);
  bVar1 = '\x01' << ((byte)param_1 & 7);
  iVar6 = param_1;
  while (iVar6 < (int)in_stack_00000008) {
    iVar6 = iVar6 + 1;
    bVar5 = bVar1 << 1;
    *(byte *)puVar4 = *(byte *)puVar4 | bVar1;
    bVar1 = bVar5;
    if (bVar5 == 0) {
      puVar4 = (undefined4 *)((int)puVar4 + 1);
      bVar1 = 1;
      if (iVar6 < (int)(in_stack_00000008 - 8)) {
        uVar2 = ((in_stack_00000008 - 8) - iVar6) + 7;
        uVar3 = uVar2 >> 3;
        iVar6 = iVar6 + uVar3 * 8;
        puVar8 = puVar4;
        for (uVar2 = uVar2 >> 5; uVar2 != 0; uVar2 = uVar2 - 1) {
          *puVar8 = 0xffffffff;
          puVar8 = puVar8 + 1;
        }
        param_1._0_1_ = 1;
        puVar4 = (undefined4 *)((int)puVar4 + uVar3);
        for (uVar3 = uVar3 & 3; bVar1 = (byte)param_1, uVar3 != 0; uVar3 = uVar3 - 1) {
          *(undefined *)puVar8 = 0xff;
          puVar8 = (undefined4 *)((int)puVar8 + 1);
        }
      }
    }
  }
  dVar7 = this->mbr_0x1c;
  pbVar9 = (byte *)this->mbr_0x38;
  iVar6 = 0;
  this->mbr_0x20 = 0;
  if (0 < (int)dVar7) {
    do {
      if (((iVar6 < (int)(dVar7 - 8)) && (bVar1 = *pbVar9, bVar1 != 0xff)) && (bVar1 != 0)) {
        for (bVar5 = 1; (iVar6 < (int)dVar7 && (bVar5 != 0)); bVar5 = bVar5 << 1) {
          if ((bVar5 & bVar1) != 0) {
            this->mbr_0x20 = this->mbr_0x20 + 1;
          }
          iVar6 = iVar6 + 1;
        }
      }
      else if (*pbVar9 == 0xff) {
        pbVar9 = pbVar9 + 1;
        iVar6 = iVar6 + 8;
        this->mbr_0x20 = this->mbr_0x20 + 8;
      }
      pbVar9 = pbVar9 + 1;
    } while (iVar6 < (int)dVar7);
  }
  return;
}



