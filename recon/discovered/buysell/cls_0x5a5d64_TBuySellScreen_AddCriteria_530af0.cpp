// FUN_00530af0 @ 00530af0 size=4019

void __thiscall FUN_00530af0(int param_1,uint param_2,int param_3,int param_4)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  uint *puVar6;
  undefined4 *puVar7;
  int local_54;
  short *local_4c;
  undefined4 local_48 [18];
  
  uVar5 = *(uint *)(param_1 + 0x17c);
  uVar4 = 0;
  if ((uVar5 & 8) == 0) {
    if ((uVar5 & 4) == 0) {
      uVar1 = param_2;
      if ((uVar5 & 0x10) != 0) {
        uVar1 = 0;
      }
    }
    else {
      uVar1 = 2;
    }
  }
  else {
    uVar1 = 1;
  }
  if (((uVar5 & 8) == 0) && ((uVar5 & 4) == 0)) {
    uVar5 = -(uint)(1 < DAT_0065a258) & DAT_0065a14c;
    puVar6 = (uint *)(uVar5 + 0x24);
    if (0 < (int)*puVar6) {
      do {
        uVar1 = FUN_00474210(param_2);
        if ((uVar4 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar4), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar4);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_3) || (param_4 < iVar2)) goto LAB_00530ccd;
        uVar1 = FUN_00474210(s_SaleType_005e3e20);
        if ((uVar4 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar4), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar4);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_00530bf0;
          if ((iVar2 == 1) && (iVar2 = FUN_0048e630(*(undefined4 *)(uVar5 + 8),uVar4), iVar2 != 0))
          {
            iVar2 = FUN_00410160(uVar4);
            puVar3 = (undefined4 *)0x0;
            if (iVar2 != 0) {
              puVar3 = (undefined4 *)FUN_0044ce10(uVar4);
            }
            iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
            if (iVar2 == 0) goto LAB_00530ccd;
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) goto LAB_00530c9e;
            goto LAB_00530ca5;
          }
        }
        else {
LAB_00530bf0:
          iVar2 = FUN_00410160(uVar4);
          puVar3 = (undefined4 *)0x0;
          if (iVar2 != 0) {
            puVar3 = (undefined4 *)FUN_0044ce10(uVar4);
          }
          iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
          if (iVar2 != 0) {
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
LAB_00530c9e:
              FUN_00533280(0xffffffff);
            }
LAB_00530ca5:
            puVar3 = local_48;
            puVar7 = (undefined4 *)(*(int *)(param_1 + 0x198) + *(short *)(param_1 + 0x194) * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar7 = *puVar3;
              puVar3 = puVar3 + 1;
              puVar7 = puVar7 + 1;
            }
            *(short *)(param_1 + 0x194) = *(short *)(param_1 + 0x194) + 1;
          }
        }
LAB_00530ccd:
        uVar4 = uVar4 + 1;
      } while ((int)uVar4 < (int)*puVar6);
    }
    uVar5 = 0;
    uVar4 = -(uint)(2 < DAT_0065a258) & DAT_0065a150;
    puVar6 = (uint *)(uVar4 + 0x24);
    if (0 < (int)*puVar6) {
      do {
        uVar1 = FUN_00474210(param_2);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_3) || (param_4 < iVar2)) goto LAB_00530e77;
        uVar1 = FUN_00474210(s_SaleType_005e3e2c);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_00530d91;
          if ((iVar2 == 1) && (iVar2 = FUN_0048e630(*(undefined4 *)(uVar4 + 8),uVar5), iVar2 != 0))
          {
            iVar2 = FUN_00410160(uVar5);
            puVar3 = (undefined4 *)0x0;
            if (iVar2 != 0) {
              puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
            }
            iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
            if (iVar2 == 0) goto LAB_00530e77;
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
            goto LAB_00530e5d;
          }
        }
        else {
LAB_00530d91:
          iVar2 = FUN_00410160(uVar5);
          puVar3 = (undefined4 *)0x0;
          if (iVar2 != 0) {
            puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
          }
          iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
          if (iVar2 != 0) {
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
LAB_00530e5d:
            local_4c = (short *)(param_1 + 0x194);
            puVar3 = local_48;
            puVar7 = (undefined4 *)(*(int *)(param_1 + 0x198) + *local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar7 = *puVar3;
              puVar3 = puVar3 + 1;
              puVar7 = puVar7 + 1;
            }
            *local_4c = *local_4c + 1;
          }
        }
LAB_00530e77:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)*puVar6);
    }
    uVar5 = 0;
    uVar4 = -(uint)(2 < DAT_0065a258) & DAT_0065a150;
    puVar6 = (uint *)(uVar4 + 0x24);
    if (0 < (int)*puVar6) {
      do {
        uVar1 = FUN_00474210(param_2);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_3) || (param_4 < iVar2)) goto LAB_00531021;
        uVar1 = FUN_00474210(s_SaleType_005e3e38);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_00530f3b;
          if ((iVar2 == 1) && (iVar2 = FUN_0048e630(*(undefined4 *)(uVar4 + 8),uVar5), iVar2 != 0))
          {
            iVar2 = FUN_00410160(uVar5);
            puVar3 = (undefined4 *)0x0;
            if (iVar2 != 0) {
              puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
            }
            iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
            if (iVar2 == 0) goto LAB_00531021;
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
            goto LAB_00531007;
          }
        }
        else {
LAB_00530f3b:
          iVar2 = FUN_00410160(uVar5);
          puVar3 = (undefined4 *)0x0;
          if (iVar2 != 0) {
            puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
          }
          iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
          if (iVar2 != 0) {
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
LAB_00531007:
            local_4c = (short *)(param_1 + 0x194);
            puVar3 = local_48;
            puVar7 = (undefined4 *)(*(int *)(param_1 + 0x198) + *local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar7 = *puVar3;
              puVar3 = puVar3 + 1;
              puVar7 = puVar7 + 1;
            }
            *local_4c = *local_4c + 1;
          }
        }
LAB_00531021:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)*puVar6);
    }
    uVar5 = 0;
    uVar4 = -(uint)(4 < DAT_0065a258) & DAT_0065a158;
    puVar6 = (uint *)(uVar4 + 0x24);
    if (0 < (int)*puVar6) {
      do {
        uVar1 = FUN_00474210(param_2);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_3) || (param_4 < iVar2)) goto LAB_005311cb;
        uVar1 = FUN_00474210(s_SaleType_005e3e44);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_005310e5;
          if ((iVar2 == 1) && (iVar2 = FUN_0048e630(*(undefined4 *)(uVar4 + 8),uVar5), iVar2 != 0))
          {
            iVar2 = FUN_00410160(uVar5);
            puVar3 = (undefined4 *)0x0;
            if (iVar2 != 0) {
              puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
            }
            iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
            if (iVar2 == 0) goto LAB_005311cb;
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
            goto LAB_005311b1;
          }
        }
        else {
LAB_005310e5:
          iVar2 = FUN_00410160(uVar5);
          puVar3 = (undefined4 *)0x0;
          if (iVar2 != 0) {
            puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
          }
          iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
          if (iVar2 != 0) {
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
LAB_005311b1:
            local_4c = (short *)(param_1 + 0x194);
            puVar3 = local_48;
            puVar7 = (undefined4 *)(*(int *)(param_1 + 0x198) + *local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar7 = *puVar3;
              puVar3 = puVar3 + 1;
              puVar7 = puVar7 + 1;
            }
            *local_4c = *local_4c + 1;
          }
        }
LAB_005311cb:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)*puVar6);
    }
    uVar5 = 0;
    uVar4 = -(uint)(0x12 < DAT_0065a258) & DAT_0065a190;
    puVar6 = (uint *)(uVar4 + 0x24);
    if (0 < (int)*puVar6) {
      do {
        uVar1 = FUN_00474210(param_2);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_3) || (param_4 < iVar2)) goto LAB_00531375;
        uVar1 = FUN_00474210(s_SaleType_005e3e50);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_0053128f;
          if ((iVar2 == 1) && (iVar2 = FUN_0048e630(*(undefined4 *)(uVar4 + 8),uVar5), iVar2 != 0))
          {
            iVar2 = FUN_00410160(uVar5);
            puVar3 = (undefined4 *)0x0;
            if (iVar2 != 0) {
              puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
            }
            iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
            if (iVar2 == 0) goto LAB_00531375;
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
            goto LAB_0053135b;
          }
        }
        else {
LAB_0053128f:
          iVar2 = FUN_00410160(uVar5);
          puVar3 = (undefined4 *)0x0;
          if (iVar2 != 0) {
            puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
          }
          iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
          if (iVar2 != 0) {
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
LAB_0053135b:
            local_4c = (short *)(param_1 + 0x194);
            puVar3 = local_48;
            puVar7 = (undefined4 *)(*(int *)(param_1 + 0x198) + *local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar7 = *puVar3;
              puVar3 = puVar3 + 1;
              puVar7 = puVar7 + 1;
            }
            *local_4c = *local_4c + 1;
          }
        }
LAB_00531375:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)*puVar6);
    }
    uVar5 = 0;
    uVar4 = -(uint)(0x15 < DAT_0065a258) & DAT_0065a19c;
    puVar6 = (uint *)(uVar4 + 0x24);
    if (0 < (int)*puVar6) {
      do {
        uVar1 = FUN_00474210(param_2);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_3) || (param_4 < iVar2)) goto LAB_0053151f;
        uVar1 = FUN_00474210(s_SaleType_005e3e5c);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_00531439;
          if ((iVar2 == 1) && (iVar2 = FUN_0048e630(*(undefined4 *)(uVar4 + 8),uVar5), iVar2 != 0))
          {
            iVar2 = FUN_00410160(uVar5);
            puVar3 = (undefined4 *)0x0;
            if (iVar2 != 0) {
              puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
            }
            iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
            if (iVar2 == 0) goto LAB_0053151f;
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
            goto LAB_00531505;
          }
        }
        else {
LAB_00531439:
          iVar2 = FUN_00410160(uVar5);
          puVar3 = (undefined4 *)0x0;
          if (iVar2 != 0) {
            puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
          }
          iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
          if (iVar2 != 0) {
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
LAB_00531505:
            local_4c = (short *)(param_1 + 0x194);
            puVar3 = local_48;
            puVar7 = (undefined4 *)(*(int *)(param_1 + 0x198) + *local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar7 = *puVar3;
              puVar3 = puVar3 + 1;
              puVar7 = puVar7 + 1;
            }
            *local_4c = *local_4c + 1;
          }
        }
LAB_0053151f:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)*puVar6);
    }
    uVar5 = 0;
    uVar4 = -(uint)(5 < DAT_0065a258) & DAT_0065a15c;
    puVar6 = (uint *)(uVar4 + 0x24);
    if (0 < (int)*puVar6) {
      do {
        uVar1 = FUN_00474210(param_2);
        if (uVar5 < *puVar6) {
          iVar2 = *(int *)(*(int *)(uVar4 + 0x34) + uVar5 * 4);
          if (iVar2 == 0) {
            iVar2 = *(int *)(uVar4 + 0x38);
          }
          if ((uint)(int)*(short *)(iVar2 + 0xc) <= uVar1) goto LAB_00531599;
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
LAB_00531599:
          iVar2 = 0;
        }
        if ((iVar2 < param_3) || (param_4 < iVar2)) goto LAB_005316cf;
        uVar1 = FUN_00474210(s_SaleType_005e3e68);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_005315e9;
          if ((iVar2 == 1) && (iVar2 = FUN_0048e630(*(undefined4 *)(uVar4 + 8),uVar5), iVar2 != 0))
          {
            iVar2 = FUN_00410160(uVar5);
            puVar3 = (undefined4 *)0x0;
            if (iVar2 != 0) {
              puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
            }
            iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
            if (iVar2 == 0) goto LAB_005316cf;
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
            goto LAB_005316b5;
          }
        }
        else {
LAB_005315e9:
          iVar2 = FUN_00410160(uVar5);
          puVar3 = (undefined4 *)0x0;
          if (iVar2 != 0) {
            puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
          }
          iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
          if (iVar2 != 0) {
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
LAB_005316b5:
            local_4c = (short *)(param_1 + 0x194);
            puVar3 = local_48;
            puVar7 = (undefined4 *)(*(int *)(param_1 + 0x198) + *local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar7 = *puVar3;
              puVar3 = puVar3 + 1;
              puVar7 = puVar7 + 1;
            }
            *local_4c = *local_4c + 1;
          }
        }
LAB_005316cf:
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)*puVar6);
    }
    uVar5 = 0;
    uVar4 = -(uint)(0x11 < DAT_0065a258) & DAT_0065a18c;
    puVar6 = (uint *)(uVar4 + 0x24);
    if (0 < (int)*puVar6) {
      do {
        uVar1 = FUN_00474210(param_2);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((iVar2 < param_3) || (param_4 < iVar2)) goto LAB_00531879;
        uVar1 = FUN_00474210(s_SaleType_005e3e74);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar1 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar1 * 4);
          if (iVar2 == 0) goto LAB_00531793;
          if ((iVar2 == 1) && (iVar2 = FUN_0048e630(*(undefined4 *)(uVar4 + 8),uVar5), iVar2 != 0))
          {
            iVar2 = FUN_00410160(uVar5);
            puVar3 = (undefined4 *)0x0;
            if (iVar2 != 0) {
              puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
            }
            iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
            if (iVar2 == 0) goto LAB_00531879;
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
            goto LAB_0053185f;
          }
        }
        else {
LAB_00531793:
          iVar2 = FUN_00410160(uVar5);
          puVar3 = (undefined4 *)0x0;
          if (iVar2 != 0) {
            puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
          }
          iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
          if (iVar2 != 0) {
            if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
              FUN_00533280(0xffffffff);
            }
LAB_0053185f:
            local_4c = (short *)(param_1 + 0x194);
            puVar3 = local_48;
            puVar7 = (undefined4 *)(*(int *)(param_1 + 0x198) + *local_4c * 0x48);
            for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
              *puVar7 = *puVar3;
              puVar3 = puVar3 + 1;
              puVar7 = puVar7 + 1;
            }
            *local_4c = *local_4c + 1;
          }
        }
LAB_00531879:
        uVar5 = uVar5 + 1;
        if ((int)*puVar6 <= (int)uVar5) {
          return;
        }
      } while( true );
    }
  }
  else {
    if (uVar1 < DAT_0065a258) {
      local_54 = (&DAT_0065a148)[uVar1];
    }
    else {
      local_54 = 0;
    }
    uVar5 = 0;
    puVar6 = (uint *)(local_54 + 0x24);
    if (0 < *(int *)(local_54 + 0x24)) {
      do {
        uVar4 = FUN_00474210(param_2);
        if ((uVar5 < *puVar6) &&
           (iVar2 = FUN_0044ce10(uVar5), uVar4 < (uint)(int)*(short *)(iVar2 + 0xc))) {
          iVar2 = FUN_0044ce10(uVar5);
          iVar2 = *(int *)(*(int *)(iVar2 + 0x10) + uVar4 * 4);
        }
        else {
          iVar2 = 0;
        }
        if ((param_3 <= iVar2) && (iVar2 <= param_4)) {
          uVar4 = FUN_00474210(s_SaleType_005e3e08);
          if ((uVar5 < *puVar6) &&
             (iVar2 = FUN_0044ce10(uVar5), uVar4 < (uint)(int)*(short *)(iVar2 + 0xc))) {
            iVar2 = FUN_0044ce10(uVar5);
            local_4c = *(short **)(*(int *)(iVar2 + 0x10) + uVar4 * 4);
          }
          else {
            local_4c = (short *)0x0;
          }
          uVar4 = FUN_00474210(s_SaleType_005e3e14);
          if (((uVar5 < *puVar6) &&
              (iVar2 = FUN_0044ce10(uVar5), uVar4 < (uint)(int)*(short *)(iVar2 + 0xc))) &&
             (iVar2 = FUN_0044ce10(uVar5), *(int *)(*(int *)(iVar2 + 0x10) + uVar4 * 4) != 0)) {
            if ((local_4c == (short *)0x1) &&
               (iVar2 = FUN_0048e630(*(undefined4 *)(local_54 + 8),uVar5), iVar2 != 0)) {
              iVar2 = FUN_00410160(uVar5);
              puVar3 = (undefined4 *)0x0;
              if (iVar2 != 0) {
                puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
              }
              iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
              if (iVar2 != 0) {
                if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
                  FUN_00533280(0xffffffff);
                }
                puVar3 = (undefined4 *)
                         (*(int *)(param_1 + 0x198) + *(short *)(param_1 + 0x194) * 0x48);
                goto LAB_00531a7f;
              }
            }
          }
          else {
            iVar2 = FUN_00410160(uVar5);
            puVar3 = (undefined4 *)0x0;
            if (iVar2 != 0) {
              puVar3 = (undefined4 *)FUN_0044ce10(uVar5);
            }
            iVar2 = FUN_0052da90(*puVar3,*(undefined4 *)(param_1 + 0x17c),1);
            if (iVar2 != 0) {
              if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
                FUN_00533280(0xffffffff);
              }
              puVar3 = (undefined4 *)
                       (*(int *)(param_1 + 0x198) + *(short *)(param_1 + 0x194) * 0x48);
LAB_00531a7f:
              puVar7 = local_48;
              for (iVar2 = 0x12; iVar2 != 0; iVar2 = iVar2 + -1) {
                *puVar3 = *puVar7;
                puVar7 = puVar7 + 1;
                puVar3 = puVar3 + 1;
              }
              *(short *)(param_1 + 0x194) = *(short *)(param_1 + 0x194) + 1;
            }
          }
        }
        uVar5 = uVar5 + 1;
      } while ((int)uVar5 < (int)*puVar6);
    }
  }
  return;
}


