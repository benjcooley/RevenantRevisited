#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x56c5a0



// Function at 0056c5a0

void cls_0x56c5a0::meth_0x56c5a0()

{
  sbyte sVar1;
  sbyte sVar2;
  int iVar3;
  word *pwVar4;
  int iVar5;
  dword *pdVar6;
  dword *pdVar7;
  dword *pdVar8;
  int iVar9;
  int iVar10;
  int in_stack_00000004;
  int local_8;
  
  if (in_stack_00000004 == 0x10000) {
    sVar1 = 0xb;
    sVar2 = 6;
  }
  else {
    sVar1 = 10;
    sVar2 = 5;
  }
  local_8 = 0;
  pdVar6 = &this->mbr_0x5400;
  pdVar8 = &this->mbr_0x800;
  do {
    iVar5 = 0;
    pdVar7 = pdVar8;
    iVar9 = 0;
    do {
      iVar10 = iVar5 / 0x1f;
      ((cls_0x56c5a0 *)(pdVar7 + -0x200))->mbr_0x0 = (word)(iVar10 << sVar1);
      *(short *)pdVar7 = (short)(iVar10 << sVar2);
      *(char *)(iVar9 + -0x2c00 + (int)pdVar6) = (char)iVar10;
      iVar3 = 0x1f;
      if (iVar10 < 0x10) {
        iVar3 = iVar10 * 2;
      }
      pdVar8 = (dword *)((int)pdVar7 + 2);
      *(word *)(pdVar7 + 0x900) = (word)(iVar3 << sVar1);
      iVar10 = iVar9 + 1;
      iVar5 = iVar5 + local_8;
      *(word *)(pdVar7 + 0xb00) = (word)(iVar3 << sVar2);
      *(char *)(iVar9 + (int)pdVar6) = (char)iVar3;
      pdVar7 = pdVar8;
      iVar9 = iVar10;
    } while (iVar10 < 0x20);
    pdVar6 = pdVar6 + 8;
    local_8 = local_8 + 1;
  } while (local_8 < 0x20);
  if (sVar2 == 6) {
    iVar9 = 0x20;
    pwVar4 = &this->mbr_0x3400;
    do {
      iVar5 = 0x20;
      do {
        if (0x400 < pwVar4[-0x1600]) {
          pwVar4[-0x1600] = pwVar4[-0x1600] + 0x20;
        }
        if (0x400 < *pwVar4) {
          *pwVar4 = *pwVar4 + 0x20;
        }
        pwVar4 = pwVar4 + 1;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      iVar9 = iVar9 + -1;
    } while (iVar9 != 0);
  }
  return;
}



