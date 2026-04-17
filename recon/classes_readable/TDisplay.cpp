#include "TDisplay.h"

// Decompiled methods and structure for class: TDisplay



// Function at 00429ac0

void TDisplay::meth_0x429ac0(cls_0x5a68d8 *param_1, int *param_2, int param_3, int param_4, int param_5, int param_6)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  dword dVar4;
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
  
  if ((param_1 != (cls_0x5a68d8 *)0x0) && (param_2 != (int *)0x0)) {
    iVar5 = this->model + this->depth_target + param_5;
    iVar15 = param_3 - this->depth_target;
    iVar16 = this->light_params + this->normal_target + param_6;
    dVar1 = this->depth_pass;
    iVar13 = param_4 - this->normal_target;
    dVar2 = this->tile_pip_desc;
    iVar10 = (*param_2 - dVar2) - dVar1;
    dVar3 = this->tile_pipeline;
    dVar4 = this->color_target;
    iVar11 = (param_2[1] - dVar4) - dVar3;
    iVar8 = iVar10 + dVar1;
    iVar9 = iVar11 + dVar3;
    if (0 < (int)dVar3) {
      if (0 < (int)dVar1) {
        cls_0x5a68d8::meth_0x4bd5e0(param_1,iVar15,iVar13,param_2,0,0,dVar1,dVar3);
      }
      if (0 < iVar10) {
        for (iVar12 = dVar1 + iVar15; iVar12 < (int)((iVar5 - dVar2) + iVar15);
            iVar12 = iVar12 + param_4) {
          iVar6 = ((iVar5 - iVar12) - dVar2) + iVar15;
          param_4 = iVar10;
          if (iVar6 <= iVar10) {
            param_4 = iVar6;
          }
          cls_0x5a68d8::meth_0x4bd5e0(param_1,iVar12,iVar13,param_2,dVar1,0,param_4,dVar3);
        }
      }
      if (0 < (int)dVar2) {
        cls_0x5a68d8::meth_0x4bd5e0
                  (param_1,(iVar5 - dVar2) + iVar15,iVar13,param_2,iVar8,0,dVar2,dVar3);
      }
    }
    if (0 < iVar11) {
      if (0 < (int)dVar1) {
        for (iVar12 = dVar3 + iVar13; iVar12 < (int)((iVar16 - dVar4) + iVar13);
            iVar12 = iVar12 + param_4) {
          iVar6 = ((iVar16 - iVar12) - dVar4) + iVar13;
          param_4 = iVar11;
          if (iVar6 <= iVar11) {
            param_4 = iVar6;
          }
          cls_0x5a68d8::meth_0x4bd5e0(param_1,iVar15,iVar12,param_2,0,dVar3,dVar1,param_4);
        }
      }
      if ((0 < iVar10) && (this->default_pass == 0)) {
        iVar12 = dVar3 + iVar13;
        iVar6 = (iVar16 - dVar4) + iVar13;
        if (iVar12 < iVar6) {
          do {
            iVar17 = ((iVar16 - iVar12) - dVar4) + iVar13;
            param_4 = iVar11;
            if (iVar17 <= iVar11) {
              param_4 = iVar17;
            }
            for (iVar17 = dVar1 + iVar15; iVar17 < (int)((iVar5 - dVar2) + iVar15);
                iVar17 = iVar17 + iVar14) {
              iVar7 = ((iVar5 - iVar17) - dVar2) + iVar15;
              iVar14 = iVar10;
              if (iVar7 <= iVar10) {
                iVar14 = iVar7;
              }
              cls_0x5a68d8::meth_0x4bd5e0(param_1,iVar17,iVar12,param_2,dVar1,dVar3,iVar14,param_4);
            }
            iVar12 = iVar12 + param_4;
          } while (iVar12 < iVar6);
        }
      }
      if (0 < (int)dVar2) {
        for (iVar12 = dVar3 + iVar13; iVar12 < (int)((iVar16 - dVar4) + iVar13);
            iVar12 = iVar12 + param_4) {
          iVar6 = ((iVar16 - iVar12) - dVar4) + iVar13;
          param_4 = iVar11;
          if (iVar6 <= iVar11) {
            param_4 = iVar6;
          }
          cls_0x5a68d8::meth_0x4bd5e0
                    (param_1,(iVar5 - dVar2) + iVar15,iVar12,param_2,iVar8,dVar3,dVar2,param_4);
        }
      }
    }
    if (0 < (int)dVar4) {
      if (0 < (int)dVar1) {
        cls_0x5a68d8::meth_0x4bd5e0
                  (param_1,iVar15,(iVar16 - dVar4) + iVar13,param_2,0,iVar9,dVar1,dVar4);
      }
      if (0 < iVar10) {
        for (iVar11 = dVar1 + iVar15; iVar11 < (int)((iVar5 - dVar2) + iVar15);
            iVar11 = iVar11 + param_4) {
          iVar12 = ((iVar5 - iVar11) - dVar2) + iVar15;
          param_4 = iVar10;
          if (iVar12 <= iVar10) {
            param_4 = iVar12;
          }
          cls_0x5a68d8::meth_0x4bd5e0
                    (param_1,iVar11,(iVar16 - dVar4) + iVar13,param_2,dVar1,iVar9,param_4,dVar4);
        }
      }
      if (0 < (int)dVar2) {
        cls_0x5a68d8::meth_0x4bd5e0
                  (param_1,(iVar5 - dVar2) + iVar15,(iVar16 - dVar4) + iVar13,param_2,iVar8,iVar9,
                   dVar2,dVar4);
      }
    }
  }
  return;
}



