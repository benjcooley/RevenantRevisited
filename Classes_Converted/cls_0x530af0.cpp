#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x530af0



// Function at 00530af0

void cls_0x530af0::meth_0x530af0(uint param_1, int param_2)

{
  cls_0x4741b0 *this_00;
  uint uVar1;
  int iVar2;
  char **ppcVar3;
  uint uVar4;
  uint uVar5;
  cls_0x45f7c0 *pcVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  int in_stack_0000000c;
  int local_54;
  undefined *local_4c;
  undefined4 local_48 [18];
  
  uVar5 = this->mbr_0x17c;
  uVar4 = 0;
  if ((uVar5 & 8) == 0) {
    if ((uVar5 & 4) == 0) {
      if ((uVar5 & 0x10) != 0) {
        param_1 = 0;
      }
    }
    else {
      param_1 = 2;
    }
  }
  else {
    param_1 = 1;
  }
  if (((uVar5 & 8) == 0) && ((uVar5 & 4) == 0)) {
    uVar5 = -(uint)(1 < DAT_0065a258) & DAT_0065a14c;
    pcVar6 = (cls_0x45f7c0 *)(uVar5 + 0x24);
    if (0 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0) {
      do {
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar5 + 0x14));
        if ((uVar4 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_2) || (in_stack_0000000c < iVar2)) goto LAB_00530ccd;
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar5 + 0x14));
        if ((uVar4 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_00530bf0;
          if ((iVar2 == 1) &&
             (iVar2 = cls_0x48e630::meth_0x48e630
                                ((cls_0x48e630 *)&DAT_0065a618,*(undefined4 *)(uVar5 + 8)),
             iVar2 != 0)) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
            if (ppcVar3 != (char **)0x0) {
              ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
            }
            iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
            if (iVar2 == 0) goto LAB_00530ccd;
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) goto LAB_00530c9e;
            goto LAB_00530ca5;
          }
        }
        else {
LAB_00530bf0:
          ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
          if (ppcVar3 != (char **)0x0) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
          }
          iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
          if (iVar2 != 0) {
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
LAB_00530c9e:
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
LAB_00530ca5:
            puVar7 = local_48;
            puVar8 = (undefined4 *)
                     (*(int *)&this[1].field_0x18 + *(short *)&this[1].field_0x14 * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar8 = *puVar7;
              puVar7 = puVar7 + 1;
              puVar8 = puVar8 + 1;
            }
            *(short *)&this[1].field_0x14 = *(short *)&this[1].field_0x14 + 1;
          }
        }
LAB_00530ccd:
        uVar4 = uVar4 + 1;
      } while ((int)uVar4 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0);
    }
    uVar5 = 0;
    uVar4 = -(uint)(2 < DAT_0065a258) & DAT_0065a150;
    pcVar6 = (cls_0x45f7c0 *)(uVar4 + 0x24);
    if (0 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0) {
      do {
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_2) || (in_stack_0000000c < iVar2)) goto LAB_00530e77;
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_00530d91;
          if ((iVar2 == 1) &&
             (iVar2 = cls_0x48e630::meth_0x48e630
                                ((cls_0x48e630 *)&DAT_0065a618,*(undefined4 *)(uVar4 + 8)),
             iVar2 != 0)) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
            if (ppcVar3 != (char **)0x0) {
              ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
            }
            iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
            if (iVar2 == 0) goto LAB_00530e77;
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
            goto LAB_00530e5d;
          }
        }
        else {
LAB_00530d91:
          ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
          if (ppcVar3 != (char **)0x0) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
          }
          iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
          if (iVar2 != 0) {
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
LAB_00530e5d:
            local_4c = &this[1].field_0x14;
            puVar7 = local_48;
            puVar8 = (undefined4 *)(*(int *)&this[1].field_0x18 + *(short *)local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar8 = *puVar7;
              puVar7 = puVar7 + 1;
              puVar8 = puVar8 + 1;
            }
            *(short *)local_4c = *(short *)local_4c + 1;
          }
        }
LAB_00530e77:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0);
    }
    uVar5 = 0;
    uVar4 = -(uint)(2 < DAT_0065a258) & DAT_0065a150;
    pcVar6 = (cls_0x45f7c0 *)(uVar4 + 0x24);
    if (0 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0) {
      do {
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_2) || (in_stack_0000000c < iVar2)) goto LAB_00531021;
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_00530f3b;
          if ((iVar2 == 1) &&
             (iVar2 = cls_0x48e630::meth_0x48e630
                                ((cls_0x48e630 *)&DAT_0065a618,*(undefined4 *)(uVar4 + 8)),
             iVar2 != 0)) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
            if (ppcVar3 != (char **)0x0) {
              ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
            }
            iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
            if (iVar2 == 0) goto LAB_00531021;
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
            goto LAB_00531007;
          }
        }
        else {
LAB_00530f3b:
          ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
          if (ppcVar3 != (char **)0x0) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
          }
          iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
          if (iVar2 != 0) {
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
LAB_00531007:
            local_4c = &this[1].field_0x14;
            puVar7 = local_48;
            puVar8 = (undefined4 *)(*(int *)&this[1].field_0x18 + *(short *)local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar8 = *puVar7;
              puVar7 = puVar7 + 1;
              puVar8 = puVar8 + 1;
            }
            *(short *)local_4c = *(short *)local_4c + 1;
          }
        }
LAB_00531021:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0);
    }
    uVar5 = 0;
    uVar4 = -(uint)(4 < DAT_0065a258) & DAT_0065a158;
    pcVar6 = (cls_0x45f7c0 *)(uVar4 + 0x24);
    if (0 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0) {
      do {
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_2) || (in_stack_0000000c < iVar2)) goto LAB_005311cb;
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_005310e5;
          if ((iVar2 == 1) &&
             (iVar2 = cls_0x48e630::meth_0x48e630
                                ((cls_0x48e630 *)&DAT_0065a618,*(undefined4 *)(uVar4 + 8)),
             iVar2 != 0)) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
            if (ppcVar3 != (char **)0x0) {
              ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
            }
            iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
            if (iVar2 == 0) goto LAB_005311cb;
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
            goto LAB_005311b1;
          }
        }
        else {
LAB_005310e5:
          ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
          if (ppcVar3 != (char **)0x0) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
          }
          iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
          if (iVar2 != 0) {
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
LAB_005311b1:
            local_4c = &this[1].field_0x14;
            puVar7 = local_48;
            puVar8 = (undefined4 *)(*(int *)&this[1].field_0x18 + *(short *)local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar8 = *puVar7;
              puVar7 = puVar7 + 1;
              puVar8 = puVar8 + 1;
            }
            *(short *)local_4c = *(short *)local_4c + 1;
          }
        }
LAB_005311cb:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0);
    }
    uVar5 = 0;
    uVar4 = -(uint)(0x12 < DAT_0065a258) & DAT_0065a190;
    pcVar6 = (cls_0x45f7c0 *)(uVar4 + 0x24);
    if (0 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0) {
      do {
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_2) || (in_stack_0000000c < iVar2)) goto LAB_00531375;
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_0053128f;
          if ((iVar2 == 1) &&
             (iVar2 = cls_0x48e630::meth_0x48e630
                                ((cls_0x48e630 *)&DAT_0065a618,*(undefined4 *)(uVar4 + 8)),
             iVar2 != 0)) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
            if (ppcVar3 != (char **)0x0) {
              ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
            }
            iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
            if (iVar2 == 0) goto LAB_00531375;
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
            goto LAB_0053135b;
          }
        }
        else {
LAB_0053128f:
          ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
          if (ppcVar3 != (char **)0x0) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
          }
          iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
          if (iVar2 != 0) {
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
LAB_0053135b:
            local_4c = &this[1].field_0x14;
            puVar7 = local_48;
            puVar8 = (undefined4 *)(*(int *)&this[1].field_0x18 + *(short *)local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar8 = *puVar7;
              puVar7 = puVar7 + 1;
              puVar8 = puVar8 + 1;
            }
            *(short *)local_4c = *(short *)local_4c + 1;
          }
        }
LAB_00531375:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0);
    }
    uVar5 = 0;
    uVar4 = -(uint)(0x15 < DAT_0065a258) & DAT_0065a19c;
    pcVar6 = (cls_0x45f7c0 *)(uVar4 + 0x24);
    if (0 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0) {
      do {
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_2) || (in_stack_0000000c < iVar2)) goto LAB_0053151f;
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_00531439;
          if ((iVar2 == 1) &&
             (iVar2 = cls_0x48e630::meth_0x48e630
                                ((cls_0x48e630 *)&DAT_0065a618,*(undefined4 *)(uVar4 + 8)),
             iVar2 != 0)) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
            if (ppcVar3 != (char **)0x0) {
              ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
            }
            iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
            if (iVar2 == 0) goto LAB_0053151f;
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
            goto LAB_00531505;
          }
        }
        else {
LAB_00531439:
          ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
          if (ppcVar3 != (char **)0x0) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
          }
          iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
          if (iVar2 != 0) {
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
LAB_00531505:
            local_4c = &this[1].field_0x14;
            puVar7 = local_48;
            puVar8 = (undefined4 *)(*(int *)&this[1].field_0x18 + *(short *)local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar8 = *puVar7;
              puVar7 = puVar7 + 1;
              puVar8 = puVar8 + 1;
            }
            *(short *)local_4c = *(short *)local_4c + 1;
          }
        }
LAB_0053151f:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0);
    }
    uVar5 = 0;
    uVar4 = -(uint)(5 < DAT_0065a258) & DAT_0065a15c;
    pcVar6 = (cls_0x45f7c0 *)(uVar4 + 0x24);
    if (0 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0) {
      do {
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if (uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) {
          iVar2 = *(int *)(*(int *)(uVar4 + 0x34) + uVar5 * 4);
          if (iVar2 == 0) {
            iVar2 = *(int *)(uVar4 + 0x38);
          }
          if ((uint)(int)*(short *)(iVar2 + 0xc) <= uVar1) goto LAB_00531599;
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
LAB_00531599:
          iVar2 = 0;
        }
        if ((iVar2 < param_2) || (in_stack_0000000c < iVar2)) goto LAB_005316cf;
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_005315e9;
          if ((iVar2 == 1) &&
             (iVar2 = cls_0x48e630::meth_0x48e630
                                ((cls_0x48e630 *)&DAT_0065a618,*(undefined4 *)(uVar4 + 8)),
             iVar2 != 0)) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
            if (ppcVar3 != (char **)0x0) {
              ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
            }
            iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
            if (iVar2 == 0) goto LAB_005316cf;
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
            goto LAB_005316b5;
          }
        }
        else {
LAB_005315e9:
          ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
          if (ppcVar3 != (char **)0x0) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
          }
          iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
          if (iVar2 != 0) {
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
LAB_005316b5:
            local_4c = &this[1].field_0x14;
            puVar7 = local_48;
            puVar8 = (undefined4 *)(*(int *)&this[1].field_0x18 + *(short *)local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar8 = *puVar7;
              puVar7 = puVar7 + 1;
              puVar8 = puVar8 + 1;
            }
            *(short *)local_4c = *(short *)local_4c + 1;
          }
        }
LAB_005316cf:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0);
    }
    uVar5 = 0;
    uVar4 = -(uint)(0x11 < DAT_0065a258) & DAT_0065a18c;
    pcVar6 = (cls_0x45f7c0 *)(uVar4 + 0x24);
    if (0 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0) {
      do {
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_2) || (in_stack_0000000c < iVar2)) goto LAB_00531879;
        uVar1 = cls_0x4741b0::meth_0x474210((cls_0x4741b0 *)(uVar4 + 0x14));
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_00531793;
          if ((iVar2 == 1) &&
             (iVar2 = cls_0x48e630::meth_0x48e630
                                ((cls_0x48e630 *)&DAT_0065a618,*(undefined4 *)(uVar4 + 8)),
             iVar2 != 0)) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
            if (ppcVar3 != (char **)0x0) {
              ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
            }
            iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
            if (iVar2 == 0) goto LAB_00531879;
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
            goto LAB_0053185f;
          }
        }
        else {
LAB_00531793:
          ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
          if (ppcVar3 != (char **)0x0) {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
          }
          iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
          if (iVar2 != 0) {
            if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
              cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
            }
LAB_0053185f:
            local_4c = &this[1].field_0x14;
            puVar7 = local_48;
            puVar8 = (undefined4 *)(*(int *)&this[1].field_0x18 + *(short *)local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar8 = *puVar7;
              puVar7 = puVar7 + 1;
              puVar8 = puVar8 + 1;
            }
            *(short *)local_4c = *(short *)local_4c + 1;
          }
        }
LAB_00531879:
        uVar5 = uVar5 + 1;
        if ((int)(pcVar6->cls_0x41c7f0).mbr_0x0 <= (int)uVar5) {
          return;
        }
      } while( true );
    }
  }
  else {
    if (param_1 < DAT_0065a258) {
      local_54 = (&DAT_0065a148)[param_1];
    }
    else {
      local_54 = 0;
    }
    uVar5 = 0;
    pcVar6 = (cls_0x45f7c0 *)(local_54 + 0x24);
    if (0 < *(int *)(local_54 + 0x24)) {
      do {
        this_00 = (cls_0x4741b0 *)(local_54 + 0x14);
        uVar4 = cls_0x4741b0::meth_0x474210(this_00);
        if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
           (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6), uVar4 < (uint)(int)*(short *)(iVar2 + 0xc))
           ) {
          iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar4 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((param_2 <= iVar2) && (iVar2 <= in_stack_0000000c)) {
          uVar4 = cls_0x4741b0::meth_0x474210(this_00);
          if ((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
             (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6),
             uVar4 < (uint)(int)*(short *)(iVar2 + 0xc))) {
            iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6);
            local_4c = *(undefined **)(*(int *)(iVar2 + 0x10) + uVar4 * 4);
          }
          else {
            local_4c = (undefined *)0x0;
          }
          uVar4 = cls_0x4741b0::meth_0x474210(this_00);
          if (((uVar5 < (pcVar6->cls_0x41c7f0).mbr_0x0) &&
              (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6),
              uVar4 < (uint)(int)*(short *)(iVar2 + 0xc))) &&
             (iVar2 = cls_0x45f7c0::meth_0x44ce10(pcVar6),
             *(int *)(*(int *)(iVar2 + 0x10) + uVar4 * 4) != 0)) {
            if ((local_4c == (undefined *)0x1) &&
               (iVar2 = cls_0x48e630::meth_0x48e630
                                  ((cls_0x48e630 *)&DAT_0065a618,*(undefined4 *)(local_54 + 8)),
               iVar2 != 0)) {
              ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
              if (ppcVar3 != (char **)0x0) {
                ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
              }
              iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
              if (iVar2 != 0) {
                if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
                  cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
                }
                puVar7 = (undefined4 *)
                         (*(int *)&this[1].field_0x18 + *(short *)&this[1].field_0x14 * 0x48);
                goto LAB_00531a7f;
              }
            }
          }
          else {
            ppcVar3 = (char **)cls_0x45f7c0::meth_0x410160(pcVar6);
            if (ppcVar3 != (char **)0x0) {
              ppcVar3 = (char **)cls_0x45f7c0::meth_0x44ce10(pcVar6);
            }
            iVar2 = FUN_0052da90(local_48,*ppcVar3,(byte)this->mbr_0x17c,1);
            if (iVar2 != 0) {
              if (*(short *)&this[1].field_0x16 <= *(short *)&this[1].field_0x14) {
                cls_0x533280::meth_0x533280((cls_0x533280 *)&this[1].field_0x14);
              }
              puVar7 = (undefined4 *)
                       (*(int *)&this[1].field_0x18 + *(short *)&this[1].field_0x14 * 0x48);
LAB_00531a7f:
              puVar8 = local_48;
              for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
                *puVar7 = *puVar8;
                puVar8 = puVar8 + 1;
                puVar7 = puVar7 + 1;
              }
              *(short *)&this[1].field_0x14 = *(short *)&this[1].field_0x14 + 1;
            }
          }
        }
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)(pcVar6->cls_0x41c7f0).mbr_0x0);
    }
  }
  return;
}



