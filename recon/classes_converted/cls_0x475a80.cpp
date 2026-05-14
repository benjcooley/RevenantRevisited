#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x475a80



// Function at 00475a80

uint cls_0x475a80::meth_0x475a80(int param_1, uint param_2, uint *param_3)

{
  char cVar1;
  char *pcVar2;
  byte bVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  byte bVar7;
  uint uVar8;
  int *piVar9;
  uint uVar10;
  char *pcVar11;
  byte bVar12;
  int in_stack_00000010;
  int local_114;
  uint local_104;
  uint local_100 [64];
  
  local_114 = 0;
  if (param_3 != (uint *)0x0) {
    *param_3 = 0;
  }
  piVar9 = (int *)this->mbr_0x34;
  local_104 = 0;
  bVar3 = 0;
  do {
    while( true ) {
      if ((&this->mbr_0x24 == (dword *)0x0) || (this->mbr_0x24 <= local_104)) {
        if (local_114 < 1) {
          return 0xffffffff;
        }
        iVar4 = FUN_00483300_RandomRange(0,local_114 + -1);
        return local_100[iVar4];
      }
      if ((char **)*piVar9 != (char **)0x0) break;
LAB_00475c5e:
      piVar9 = piVar9 + 1;
      local_104 = local_104 + 1;
    }
    if (-1 < in_stack_00000010) {
      pcVar2 = *(char **)*piVar9;
      uVar8 = 0xffffffff;
      pcVar11 = pcVar2;
      do {
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        cVar1 = *pcVar11;
        pcVar11 = pcVar11 + 1;
      } while (cVar1 != '\0');
      if (((pcVar2[~uVar8 - 2] < '0') || ('9' < pcVar2[~uVar8 - 2])) ||
         (iVar4 = FUN_0058b42c(pcVar2 + (~uVar8 - 2),(byte *)(pcVar2 + (~uVar8 - 2))),
         iVar4 != in_stack_00000010)) goto LAB_00475c5e;
    }
    uVar8 = *(uint *)(*(int *)(*piVar9 + 0x10) + param_1 * 4);
    uVar5 = 0;
    bVar12 = (uVar8 & 0xf0000000) != 0;
    if ((bool)bVar12) {
      uVar5 = 0xff000000;
    }
    if ((uVar8 & 0xf00000) != 0) {
      uVar5 = uVar5 | 0xff0000;
      bVar12 = bVar12 + 1;
    }
    if ((uVar8 & 0xf000) != 0) {
      uVar5 = CONCAT22((short)(uVar5 >> 0x10),0xff00);
      bVar12 = bVar12 + 1;
    }
    if ((uVar8 & 0xf0) != 0) {
      uVar5 = CONCAT31((int3)(uVar5 >> 8),0xff);
      bVar12 = bVar12 + 1;
    }
    if (bVar12 < bVar3) goto LAB_00475c5e;
    if (uVar8 != (uVar5 & param_2)) {
      if ((uVar5 != 0xffffffff) && (param_3 != (uint *)0x0)) {
        uVar10 = 1000;
        iVar4 = 0;
        do {
          if (((uVar8 & 0xf0 << ((byte)iVar4 & 0x1f)) != 0) &&
             (uVar6 = uVar8 >> ((byte)iVar4 & 0x1f) & 0xf, uVar6 < uVar10)) {
            uVar10 = uVar6;
          }
          iVar4 = iVar4 + 8;
        } while (iVar4 < 0x20);
        if (uVar10 < 0xf) {
          iVar4 = 0;
          do {
            bVar7 = (byte)iVar4;
            if (((uVar8 & 0xf0 << (bVar7 & 0x1f)) != 0) &&
               (uVar10 <= ((int)uVar8 >> (bVar7 & 0x1f) & 0xfU))) {
              uVar8 = uVar8 - (uVar10 << (bVar7 & 0x1f));
            }
            iVar4 = iVar4 + 8;
          } while (iVar4 < 0x20);
          if (uVar8 == (uVar5 & param_2)) {
            if (bVar12 == bVar3) {
              local_100[local_114] = local_104;
              local_114 = local_114 + 1;
            }
            else {
              local_114 = 1;
              local_100[0] = local_104;
            }
            *param_3 = uVar10;
            bVar3 = bVar12;
          }
        }
      }
      goto LAB_00475c5e;
    }
    if (bVar12 == bVar3) {
      local_100[local_114] = local_104;
      local_114 = local_114 + 1;
    }
    else {
      local_114 = 1;
      local_100[0] = local_104;
    }
    bVar3 = bVar12;
    if (param_3 == (uint *)0x0) goto LAB_00475c5e;
    piVar9 = piVar9 + 1;
    local_104 = local_104 + 1;
    *param_3 = 0;
  } while( true );
}



