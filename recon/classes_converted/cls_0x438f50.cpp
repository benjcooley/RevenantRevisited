#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x438f50



// Function at 00438f50

undefined4 cls_0x438f50::meth_0x438f50(int param_1)

{
  dword *this_00;
  LPCVOID pvVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *in_stack_00000008;
  
  if (this->mbr_0x0 == 0) {
    this_00 = &this->mbr_0x4;
    iVar3 = 0;
    if (0 < (int)this->mbr_0x4) {
      do {
        if ((-1 < iVar3) &&
           (pvVar1 = *(LPCVOID *)(this->mbr_0x14 + iVar3 * 4), pvVar1 != (LPCVOID)0x0)) {
          FUN_004830f0(pvVar1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
        iVar3 = iVar3 + 1;
      } while (iVar3 < (int)*this_00);
    }
    *this_00 = 0;
    this->mbr_0x8 = 0;
    if (0 < param_1) {
      do {
        puVar2 = FUN_00482fb0(0x4c);
        puVar4 = in_stack_00000008;
        for (iVar3 = 0x13; iVar3 != 0; iVar3 = iVar3 + -1) {
          *puVar2 = *puVar4;
          puVar4 = puVar4 + 1;
          puVar2 = puVar2 + 1;
        }
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)this_00);
        in_stack_00000008 = in_stack_00000008 + 0x13;
        param_1 = param_1 + -1;
      } while (param_1 != 0);
    }
    this->mbr_0x20 = 0;
    this->mbr_0x1c = 0;
    this->mbr_0x0 = 1;
    return 1;
  }
  return 1;
}



// Function at 00439ba0

void cls_0x438f50::meth_0x439ba0()

{
  char cVar1;
  int iVar2;
  dword dVar3;
  int *piVar4;
  uint uVar5;
  int iVar6;
  int *piVar7;
  int *piVar8;
  int iVar9;
  char *pcVar10;
  char *pcVar11;
  uint local_d0;
  int local_c8;
  undefined4 local_c4;
  char local_c0 [62];
  char acStack_82 [66];
  CHAR local_40 [64];
  
  local_c4 = this;
  FUN_00482160(s_Controls_005ce628);
  local_d0 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      if (((this->mbr_0x14 != 0) && (local_d0 < this->mbr_0x4)) &&
         (*(int *)(this->mbr_0x14 + local_d0 * 4) != 0)) {
        dVar3 = *(dword *)(this->mbr_0x14 + local_d0 * 4);
        if (dVar3 == 0) {
          dVar3 = this->mbr_0x18;
        }
        local_c0[0] = '\0';
        piVar7 = (int *)(dVar3 + 0xc);
        local_c8 = 0;
        do {
          if (0 < *piVar7) {
            if (0 < local_c8) {
              uVar5 = 0xffffffff;
              pcVar10 = local_c0;
              do {
                if (uVar5 == 0) break;
                uVar5 = uVar5 - 1;
                cVar1 = *pcVar10;
                pcVar10 = pcVar10 + 1;
              } while (cVar1 != '\0');
              iVar9 = -(~uVar5 - 1);
              _strncpy(local_c0 + (~uVar5 - 1),&DAT_005ce624,iVar9 + 0x3f);
              (local_c0 + (~uVar5 - 1))[iVar9 + 0x3f] = '\0';
            }
            acStack_82[2] = 0;
            if (0 < *piVar7) {
              iVar9 = 0;
              piVar8 = piVar7;
              while (iVar2 = *piVar8, 0 < iVar2) {
                if (((iVar2 < 0x41) || (0x5a < iVar2)) && ((iVar2 < 0x30 || (0x39 < iVar2)))) {
                  iVar6 = 0;
                  piVar4 = &DAT_005cdc98;
                  while (*piVar4 != iVar2) {
                    piVar4 = piVar4 + 2;
                    iVar6 = iVar6 + 1;
                    if ((int *)0x5ce187 < piVar4) goto LAB_00439d23;
                  }
                  pcVar10 = (&PTR_DAT_005cdc9c)[iVar6 * 2];
                }
                else {
                  UNK_00656391._3_1_ = (undefined)iVar2;
                  UNK_00656391._4_1_ = 0;
                  pcVar10 = (char *)((int)&UNK_00656391 + 3);
                }
                if (pcVar10 == (char *)0x0) break;
                if (0 < iVar9) {
                  uVar5 = 0xffffffff;
                  pcVar11 = acStack_82 + 2;
                  do {
                    if (uVar5 == 0) break;
                    uVar5 = uVar5 - 1;
                    cVar1 = *pcVar11;
                    pcVar11 = pcVar11 + 1;
                  } while (cVar1 != '\0');
                  FUN_00439fd0(acStack_82 + ~uVar5 + 1,&DAT_005ce620,0x40 - (~uVar5 - 1));
                }
                uVar5 = 0xffffffff;
                pcVar11 = acStack_82 + 2;
                do {
                  if (uVar5 == 0) break;
                  uVar5 = uVar5 - 1;
                  cVar1 = *pcVar11;
                  pcVar11 = pcVar11 + 1;
                } while (cVar1 != '\0');
                FUN_00439fd0(acStack_82 + ~uVar5 + 1,pcVar10,0x40 - (~uVar5 - 1));
                iVar9 = iVar9 + 1;
                piVar8 = piVar8 + 1;
                if (2 < iVar9) break;
              }
            }
LAB_00439d23:
            uVar5 = 0xffffffff;
            pcVar10 = local_c0;
            do {
              if (uVar5 == 0) break;
              uVar5 = uVar5 - 1;
              cVar1 = *pcVar10;
              pcVar10 = pcVar10 + 1;
            } while (cVar1 != '\0');
            iVar9 = -(~uVar5 - 1);
            _strncpy(local_c0 + (~uVar5 - 1),acStack_82 + 2,iVar9 + 0x3f);
            (local_c0 + (~uVar5 - 1))[iVar9 + 0x3f] = '\0';
            this = local_c4;
          }
          local_c8 = local_c8 + 1;
          piVar7 = piVar7 + 4;
        } while (local_c8 < 3);
        FUN_004825a0(*(LPCSTR *)(dVar3 + 4),local_c0,local_40,0x40);
        FUN_00439ab0((uint)this,local_40,dVar3);
      }
      local_d0 = local_d0 + 1;
    } while ((int)local_d0 < (int)this->mbr_0x4);
  }
  return;
}



