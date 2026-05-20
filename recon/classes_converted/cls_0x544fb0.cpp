#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x544fb0



// Function at 00544fb0

void cls_0x544fb0::meth_0x544fb0()

{
  byte bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  byte *pbVar11;
  undefined4 *puVar12;
  byte *pbVar13;
  undefined4 *puVar14;
  bool bVar15;
  byte *in_stack_00000004;
  int local_5c;
  char local_50 [80];
  
  iVar3 = *(int *)(DAT_0065b020 + DAT_0065abc4 * 4);
  if (iVar3 == 0) {
    iVar3 = DAT_0065b024;
  }
  if (in_stack_00000004 != (byte *)0x0) {
    iVar7 = -1;
    pbVar11 = in_stack_00000004;
    do {
      if (iVar7 == 0) break;
      iVar7 = iVar7 + -1;
      bVar1 = *pbVar11;
      pbVar11 = pbVar11 + 1;
    } while (bVar1 != 0);
    if (((iVar7 != -2) &&
        (iVar7 = cls_0x53ed70::meth_0x53ed70((cls_0x53ed70 *)((int)&UNK_00667c34 + 4)), iVar7 != 0))
       && (*(char *)(iVar7 + 0x5c) != '\0')) {
      FUN_0058b100(local_50,(byte *)s_SPELL_s_005e52dc);
      iVar7 = cls_0x45f7c0::meth_0x49d6d0((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
      if ((-1 < iVar7) &&
         (iVar7 = cls_0x45f7c0::meth_0x49d780((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310), iVar7 != 0
         )) {
        cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffff88);
        iVar2 = cls_0x5a68d8::meth_0x4be2b0_CompositeBuffer
                          ((cls_0x5a68d8 *)this->mbr_0x180,0,0,0x84,1,(char)iVar7,0);
        iVar7 = DAT_0065a9d8;
        iVar3 = iVar2 * *(int *)(iVar3 + 0x50) + 0x5b;
        iVar10 = 0;
        iVar2 = 0;
        local_5c = 0;
        if (0 < *(int *)(DAT_00667fcc + 0x2ec)) {
          do {
            pbVar11 = *(byte **)(*(int *)(DAT_00667fcc + 0x2fc) + iVar2 * 4);
            pbVar13 = in_stack_00000004;
            do {
              bVar1 = *pbVar11;
              bVar15 = bVar1 < *pbVar13;
              if (bVar1 != *pbVar13) {
LAB_005450e1:
                iVar4 = (1 - (uint)bVar15) - (uint)(bVar15 != 0);
                goto LAB_005450e6;
              }
              if (bVar1 == 0) break;
              bVar1 = pbVar11[1];
              bVar15 = bVar1 < pbVar13[1];
              if (bVar1 != pbVar13[1]) goto LAB_005450e1;
              pbVar11 = pbVar11 + 2;
              pbVar13 = pbVar13 + 2;
            } while (bVar1 != 0);
            iVar4 = 0;
LAB_005450e6:
            if (iVar4 == 0) {
              this->mbr_0x1d0 = iVar10 - this->mbr_0x17c;
              UNK_0065a9dc._76_4_ = 1;
              (**(code **)(iVar7 + 0x90))();
              return;
            }
            iVar10 = iVar10 + *(int *)(this->mbr_0x1a8 + iVar2 * 4);
            iVar2 = iVar2 + 1;
            local_5c = iVar10;
          } while (iVar2 < *(int *)(DAT_00667fcc + 0x2ec));
        }
        puVar5 = FUN_00482fb0(6);
        uVar8 = 0xffffffff;
        do {
          pbVar11 = in_stack_00000004;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pbVar11 = in_stack_00000004 + 1;
          bVar1 = *in_stack_00000004;
          in_stack_00000004 = pbVar11;
        } while (bVar1 != 0);
        uVar8 = ~uVar8;
        puVar6 = (undefined4 *)(pbVar11 + -uVar8);
        for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
          *puVar5 = *puVar6;
          puVar6 = puVar6 + 1;
          puVar5 = puVar5 + 1;
        }
        for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
          *(undefined *)puVar5 = *(undefined *)puVar6;
          puVar6 = (undefined4 *)((int)puVar6 + 1);
          puVar5 = (undefined4 *)((int)puVar5 + 1);
        }
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)(DAT_00667fcc + 0x2ec));
        meth_0x546830(this);
        this->mbr_0x1d0 = local_5c - this->mbr_0x17c;
        this->mbr_0x1a0 = iVar3 + local_5c;
        if ((short)this->mbr_0x1a6 <= (short)this->mbr_0x1a4) {
          iVar7 = (short)this->mbr_0x1a6 + 0x40;
          puVar6 = FUN_00482fb0(iVar7 * 4);
          uVar8 = (uint)(short)this->mbr_0x1a6;
          puVar5 = puVar6 + uVar8;
          for (uVar9 = iVar7 - uVar8 & 0x3fffffff; uVar9 != 0; uVar9 = uVar9 - 1) {
            *puVar5 = 0;
            puVar5 = puVar5 + 1;
          }
          for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
            *(undefined *)puVar5 = 0;
            puVar5 = (undefined4 *)((int)puVar5 + 1);
          }
          puVar5 = (undefined4 *)this->mbr_0x1a8;
          if (puVar5 != (undefined4 *)0x0) {
            puVar12 = puVar5;
            puVar14 = puVar6;
            for (uVar8 = uVar8 & 0x3fffffff; uVar8 != 0; uVar8 = uVar8 - 1) {
              *puVar14 = *puVar12;
              puVar12 = puVar12 + 1;
              puVar14 = puVar14 + 1;
            }
            for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
              *(undefined *)puVar14 = *(undefined *)puVar12;
              puVar12 = (undefined4 *)((int)puVar12 + 1);
              puVar14 = (undefined4 *)((int)puVar14 + 1);
            }
            FUN_004830f0(puVar5);
          }
          this->mbr_0x1a6 = (word)iVar7;
          this->mbr_0x1a8 = (dword)puVar6;
        }
        *(int *)(this->mbr_0x1a8 + (short)this->mbr_0x1a4 * 4) = iVar3;
        this->mbr_0x1a4 = this->mbr_0x1a4 + 1;
        if (local_5c < (int)(this->mbr_0x17c + 0xe5)) {
          (**(code **)(*(int *)this->mbr_0x180 + 0x44))(((int *)this->mbr_0x180)[5]);
          UNK_0065a9dc._76_4_ = 1;
          (**(code **)(DAT_0065a9d8 + 0x90))();
          this->mbr_0x19c = 0;
          return;
        }
        (**(code **)(this->mbr_0x0 + 0x28))();
      }
    }
  }
  return;
}



// Function at 00546830

void cls_0x544fb0::meth_0x546830()

{
  cls_0x45f7c0 *this_00;
  char cVar1;
  LPCVOID pvVar2;
  undefined uVar3;
  undefined4 *puVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  undefined4 *puVar10;
  char *pcVar11;
  undefined4 *puVar12;
  undefined4 *puVar13;
  undefined4 uVar14;
  dword local_6c;
  int local_68;
  int local_64;
  int local_5c;
  char local_50 [80];
  
  local_6c = 0;
  local_68 = 0;
  if ((LPCVOID)this->mbr_0x1a8 != (LPCVOID)0x0) {
    FUN_004830f0((LPCVOID)this->mbr_0x1a8);
  }
  iVar9 = DAT_0065b020;
  iVar6 = DAT_0065abc4;
  this->mbr_0x1a8 = 0;
  this->mbr_0x1a4 = 0;
  local_64 = *(int *)(iVar9 + iVar6 * 4);
  this->mbr_0x1a6 = 0;
  if (local_64 == 0) {
    local_64 = DAT_0065b024;
  }
  if (DAT_00667fcc != 0) {
    local_5c = 0;
    if (0 < *(int *)(DAT_00667fcc + 0x2ec)) {
      do {
        pcVar11 = *(char **)(*(int *)(DAT_00667fcc + 0x2fc) + local_5c * 4);
        if (pcVar11 != (char *)0x0) {
          iVar6 = -1;
          do {
            if (iVar6 == 0) break;
            iVar6 = iVar6 + -1;
            cVar1 = *pcVar11;
            pcVar11 = pcVar11 + 1;
          } while (cVar1 != '\0');
          if (iVar6 != -2) {
            local_68 = cls_0x53ed70::meth_0x53ed70((cls_0x53ed70 *)((int)&UNK_00667c34 + 4));
          }
        }
        if (local_68 == 0) {
          this_00 = (cls_0x45f7c0 *)(DAT_00667fcc + 0x2ec);
          pvVar2 = *(LPCVOID *)(*(int *)(DAT_00667fcc + 0x2fc) + local_5c * 4);
          if (pvVar2 != (LPCVOID)0x0) {
            FUN_004830f0(pvVar2);
          }
          cls_0x45f7c0::meth_0x41cb80(this_00);
          local_5c = local_5c + -1;
        }
        else {
          FUN_0058b100(local_50,(byte *)s_SPELL_s_005e5394);
          cls_0x419dd0::cls_0x419dd0((cls_0x419dd0 *)&stack0xffffff78);
          uVar14 = 0;
          uVar3 = cls_0x45f7c0::meth_0x49d800((cls_0x45f7c0 *)&UNK_0065d1c0.field_0x310);
          iVar6 = cls_0x5a68d8::meth_0x4be2b0_CompositeBuffer
                            ((cls_0x5a68d8 *)this->mbr_0x180,0,0,0x84,1,uVar3,uVar14);
          iVar6 = iVar6 * *(int *)(local_64 + 0x50) + 0x5b;
          if ((short)this->mbr_0x1a6 <= (short)this->mbr_0x1a4) {
            iVar9 = (short)this->mbr_0x1a6 + 0x40;
            puVar4 = FUN_00482fb0(iVar9 * 4);
            uVar5 = (uint)(short)this->mbr_0x1a6;
            puVar12 = puVar4 + uVar5;
            for (uVar7 = iVar9 - uVar5 & 0x3fffffff; uVar7 != 0; uVar7 = uVar7 - 1) {
              *puVar12 = 0;
              puVar12 = puVar12 + 1;
            }
            for (iVar8 = 0; iVar8 != 0; iVar8 = iVar8 + -1) {
              *(undefined *)puVar12 = 0;
              puVar12 = (undefined4 *)((int)puVar12 + 1);
            }
            puVar12 = (undefined4 *)this->mbr_0x1a8;
            if (puVar12 != (undefined4 *)0x0) {
              puVar10 = puVar12;
              puVar13 = puVar4;
              for (uVar5 = uVar5 & 0x3fffffff; uVar5 != 0; uVar5 = uVar5 - 1) {
                *puVar13 = *puVar10;
                puVar10 = puVar10 + 1;
                puVar13 = puVar13 + 1;
              }
              for (iVar8 = 0; iVar8 != 0; iVar8 = iVar8 + -1) {
                *(undefined *)puVar13 = *(undefined *)puVar10;
                puVar10 = (undefined4 *)((int)puVar10 + 1);
                puVar13 = (undefined4 *)((int)puVar13 + 1);
              }
              FUN_004830f0(puVar12);
            }
            this->mbr_0x1a6 = (word)iVar9;
            this->mbr_0x1a8 = (dword)puVar4;
          }
          *(int *)(this->mbr_0x1a8 + (short)this->mbr_0x1a4 * 4) = iVar6;
          this->mbr_0x1a4 = this->mbr_0x1a4 + 1;
          local_6c = local_6c + iVar6;
        }
        local_5c = local_5c + 1;
      } while (local_5c < *(int *)(DAT_00667fcc + 0x2ec));
    }
    this->mbr_0x1a0 = local_6c;
  }
  return;
}



