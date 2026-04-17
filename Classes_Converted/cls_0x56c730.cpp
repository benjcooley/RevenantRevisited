#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x56c730



// Function at 0056c730

void cls_0x56c730::meth_0x56c730(dword param_1)

{
  short sVar1;
  undefined4 *puVar2;
  ushort uVar3;
  short sVar4;
  uint uVar5;
  short sVar6;
  uint uVar7;
  int *piVar8;
  uint uVar9;
  uint uVar10;
  int in_stack_00000008;
  
  this->mbr_0x8 = param_1;
  if ((LPCVOID)this->mbr_0x0 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x0);
  }
  if ((LPCVOID)this->mbr_0x4 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x4);
  }
  puVar2 = FUN_00482fb0(param_1 * 4);
  this->mbr_0x0 = (dword)puVar2;
  if (in_stack_00000008 == 0x20) {
    puVar2 = FUN_00482fb0(param_1 << 4);
    this->mbr_0x4 = (dword)puVar2;
    if (param_1 == 0x10000) {
      uVar5 = 0;
      piVar8 = (int *)this->mbr_0x0;
      do {
        uVar7 = uVar5 & 0xf800;
        uVar9 = uVar5 & 0x7e0;
        uVar10 = uVar5 & 0x1f;
        uVar5 = uVar5 + 1;
        *piVar8 = uVar10 + (uVar9 + uVar7 * 2) * 2;
        piVar8 = piVar8 + 1;
      } while ((int)uVar5 < 0x10000);
      uVar5 = 0;
      do {
        uVar3 = (ushort)uVar5 & 0x3f;
        uVar7 = uVar5 & 0x1fc0;
        uVar9 = uVar5 & 0x7e000;
        if ((uVar5 & 0x20) != 0) {
          uVar3 = 0x1f;
        }
        if ((uVar5 & 0x1000) != 0) {
          uVar7 = 0xfc0;
        }
        if ((uVar5 & 0x40000) != 0) {
          uVar9 = 0x3e000;
        }
        uVar5 = uVar5 + 1;
        *(ushort *)puVar2 = (short)(uVar7 >> 1) + uVar3 + (short)(uVar9 >> 2);
        puVar2 = (undefined4 *)((int)puVar2 + 2);
      } while ((int)uVar5 < 0x80000);
      return;
    }
    uVar5 = 0;
    if (0 < (int)param_1) {
      piVar8 = (int *)this->mbr_0x0;
      do {
        uVar7 = uVar5 & 0x7c00;
        uVar9 = uVar5 & 0x3e0;
        uVar10 = uVar5 & 0x1f;
        uVar5 = uVar5 + 1;
        *piVar8 = uVar10 + (uVar9 + uVar7 * 2) * 2;
        piVar8 = piVar8 + 1;
      } while ((int)uVar5 < (int)param_1);
    }
    uVar5 = 0;
    if (0 < (int)(param_1 * 8)) {
      do {
        uVar3 = (ushort)uVar5 & 0x3f;
        uVar7 = uVar5 & 0xfc0;
        uVar9 = uVar5 & 0x3f000;
        if ((uVar5 & 0x20) != 0) {
          uVar3 = 0x1f;
        }
        if ((uVar5 & 0x800) != 0) {
          uVar7 = 0x7c0;
        }
        if ((uVar5 & 0x20000) != 0) {
          uVar9 = 0x1f000;
        }
        uVar5 = uVar5 + 1;
        *(ushort *)puVar2 = (short)(uVar7 >> 1) + uVar3 + (short)(uVar9 >> 2);
        puVar2 = (undefined4 *)((int)puVar2 + 2);
      } while ((int)uVar5 < (int)(param_1 * 8));
      return;
    }
  }
  else {
    puVar2 = FUN_00482fb0(param_1 * 2);
    this->mbr_0x4 = (dword)puVar2;
    if (param_1 == 0x10000) {
      uVar5 = 0;
      piVar8 = (int *)this->mbr_0x0;
      do {
        uVar7 = uVar5 >> 1;
        uVar5 = uVar5 + 1;
        *piVar8 = (uVar7 & 0x3e0) + (uVar7 & 0xf) + (uVar7 & 0x7800);
        piVar8 = piVar8 + 1;
      } while ((int)uVar5 < 0x10000);
      uVar5 = 0;
      do {
        if ((uVar5 & 0x10) == 0) {
          sVar1 = (short)((uVar5 & 0x1f) << 1);
        }
        else {
          sVar1 = 0x1f;
        }
        if ((uVar5 & 0x400) == 0) {
          sVar6 = (short)((uVar5 & 0x7e0) << 1);
        }
        else {
          sVar6 = 0x7e0;
        }
        if ((uVar5 & 0x8000) == 0) {
          sVar4 = (short)((uVar5 & 0xf800) << 1);
        }
        else {
          sVar4 = -0x800;
        }
        uVar5 = uVar5 + 1;
        *(short *)puVar2 = sVar4 + sVar6 + sVar1;
        puVar2 = (undefined4 *)((int)puVar2 + 2);
      } while ((int)uVar5 < 0x10000);
      return;
    }
    uVar5 = 0;
    if (0 < (int)param_1) {
      piVar8 = (int *)this->mbr_0x0;
      do {
        uVar7 = uVar5 >> 1;
        uVar5 = uVar5 + 1;
        *piVar8 = (uVar7 & 0x3c00) + (uVar7 & 0x1e0) + (uVar7 & 0xf);
        piVar8 = piVar8 + 1;
      } while ((int)uVar5 < (int)param_1);
    }
    uVar5 = 0;
    if (0 < (int)param_1) {
      do {
        if ((uVar5 & 0x10) == 0) {
          sVar1 = (short)((uVar5 & 0x1f) << 1);
        }
        else {
          sVar1 = 0x1f;
        }
        if ((uVar5 & 0x200) == 0) {
          sVar6 = (short)((uVar5 & 0x3e0) << 1);
        }
        else {
          sVar6 = 0x3e0;
        }
        if ((uVar5 & 0x4000) == 0) {
          sVar4 = (short)((uVar5 & 0x7c00) << 1);
        }
        else {
          sVar4 = 0x7c00;
        }
        uVar5 = uVar5 + 1;
        *(short *)puVar2 = sVar4 + sVar6 + sVar1;
        puVar2 = (undefined4 *)((int)puVar2 + 2);
      } while ((int)uVar5 < (int)param_1);
    }
  }
  return;
}



