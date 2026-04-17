#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x56ff60



// Function at 0056ff60

cls_0x56ff60::cls_0x56ff60()

{
  this->mbr_0x0 = 1;
  return this;
}



// Function at 0056ff80

void cls_0x56ff60::meth_0x56ff80(char *param_1, undefined param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  uint uVar19;
  undefined3 in_stack_00000009;
  char in_stack_0000000c;
  
  if (in_stack_0000000c != '\0') {
    this->mbr_0x0 = 1;
  }
  uVar3 = this->mbr_0x0 & 0xffff;
  uVar19 = this->mbr_0x0 >> 0x10;
  while (0 < (int)_param_2) {
    uVar1 = _param_2;
    if (0x15af < (int)_param_2) {
      uVar1 = 0x15b0;
    }
    _param_2 = _param_2 - uVar1;
    if (0xf < (int)uVar1) {
      uVar2 = uVar1 >> 4;
      uVar1 = uVar1 + uVar2 * -0x10;
      do {
        iVar4 = uVar3 + (int)*param_1;
        iVar5 = iVar4 + param_1[1];
        iVar6 = iVar5 + param_1[2];
        iVar7 = iVar6 + param_1[3];
        iVar8 = iVar7 + param_1[4];
        iVar9 = iVar8 + param_1[5];
        iVar10 = iVar9 + param_1[6];
        iVar11 = iVar10 + param_1[7];
        iVar12 = iVar11 + param_1[8];
        iVar13 = iVar12 + param_1[9];
        iVar14 = iVar13 + param_1[10];
        iVar15 = iVar14 + param_1[0xb];
        iVar16 = iVar15 + param_1[0xc];
        iVar17 = iVar16 + param_1[0xd];
        iVar18 = iVar17 + param_1[0xe];
        uVar3 = iVar18 + param_1[0xf];
        uVar19 = uVar19 + iVar4 + iVar5 + iVar6 + iVar7 + iVar8 + iVar9 + iVar10 + iVar11 + iVar12 +
                 iVar13 + iVar14 + iVar15 + iVar16 + iVar17 + iVar18 + uVar3;
        param_1 = param_1 + 0x10;
        uVar2 = uVar2 - 1;
      } while (uVar2 != 0);
    }
    for (; uVar1 != 0; uVar1 = uVar1 - 1) {
      uVar3 = uVar3 + (int)*param_1;
      param_1 = param_1 + 1;
      uVar19 = uVar19 + uVar3;
    }
    uVar3 = uVar3 % 0xfff1;
    uVar19 = uVar19 % 0xfff1;
  }
  this->mbr_0x0 = uVar19 << 0x10 | uVar3;
  return;
}



