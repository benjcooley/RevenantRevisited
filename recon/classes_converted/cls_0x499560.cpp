#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x499560



// Function at 00499560

void cls_0x499560::meth_0x499560_TSector_WalkmapHandler(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7, int param_8, int param_9)

{
  int iVar1;
  int iVar2;
  int iVar3;
  ushort *puVar4;
  int iVar5;
  byte *pbVar6;
  int in_stack_00000028;
  
  if ((this->mbr_0x0 != 0) && (param_2 != 0)) {
    iVar3 = param_4 - ((int)(this->mbr_0x8 << 10) >> 4);
    iVar5 = 0;
    iVar1 = param_5 - ((int)(this->mbr_0xc << 10) >> 4);
    iVar2 = 0;
    if (iVar3 < 0) {
      param_6 = param_6 + iVar3;
      iVar5 = -iVar3;
      iVar3 = 0;
    }
    if (iVar1 < 0) {
      param_7 = param_7 + iVar1;
      iVar2 = -iVar1;
      iVar1 = 0;
    }
    if (0x40 < param_6 + iVar3) {
      param_6 = 0x40 - iVar3;
    }
    if (0x40 < param_7 + iVar1) {
      param_7 = 0x40 - iVar1;
    }
    if ((param_6 != 0) || (param_7 != 0)) {
      puVar4 = (ushort *)(this->mbr_0x0 + (iVar1 * 0x40 + iVar3) * 2);
      pbVar6 = (byte *)(param_2 + iVar2 * param_8 + iVar5);
      param_4 = param_7;
      if (0 < param_7) {
        do {
          param_2 = param_6;
          if (0 < param_6) {
            do {
              if (param_1 == 0) {
                if (*pbVar6 != 0) {
                  iVar1 = (uint)*pbVar6 + param_3;
                  if (iVar1 < 1) {
LAB_0049966b:
                    iVar1 = 1;
                  }
                  else if (iVar1 < 0x400) {
                    if (iVar1 < 1) goto LAB_0049966b;
                  }
                  else {
                    iVar1 = 0x3ff;
                  }
                  if (((int)(*puVar4 & 0x3ff) < iVar1) || (in_stack_00000028 != 0)) {
                    *puVar4 = (ushort)iVar1 & 0x3ff | (ushort)(param_9 << 10);
                  }
                }
              }
              else if (param_1 == 1) {
                if (*puVar4 == 0) {
                  *pbVar6 = 0;
                }
                else {
                  iVar1 = (*puVar4 & 0x3ff) - param_3;
                  if (iVar1 < 1) {
LAB_004996fb:
                    iVar1 = 1;
                  }
                  else {
                    if (0xff < iVar1) {
                      *pbVar6 = 0xff;
                      goto LAB_0049969a;
                    }
                    if (iVar1 < 1) goto LAB_004996fb;
                  }
                  *pbVar6 = (byte)iVar1;
                }
              }
              else if (param_1 == 2) {
                *puVar4 = 0;
              }
LAB_0049969a:
              puVar4 = puVar4 + 1;
              pbVar6 = pbVar6 + 1;
              param_2 = param_2 + -1;
            } while (param_2 != 0);
          }
          puVar4 = puVar4 + (0x40 - param_6);
          pbVar6 = pbVar6 + (param_8 - param_6);
          param_4 = param_4 + -1;
        } while (param_4 != 0);
      }
    }
  }
  return;
}



