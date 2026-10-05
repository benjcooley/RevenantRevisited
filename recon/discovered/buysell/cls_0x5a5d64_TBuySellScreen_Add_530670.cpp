// FUN_00530670 @ 00530670 size=1139

void __thiscall FUN_00530670(int param_1,undefined4 param_2,undefined4 param_3)

{
  short *psVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  undefined4 local_48 [18];
  
  if ((*(uint *)(param_1 + 0x17c) & 0x10) != 0) {
    uVar8 = -(uint)(2 < DAT_0065a258) & DAT_0065a150;
    uVar2 = FUN_00475210(param_2,0);
    if (uVar2 == 0xffffffff) {
      uVar8 = -(uint)(1 < DAT_0065a258) & DAT_0065a14c;
      uVar2 = FUN_00475210(param_2,0);
      if (uVar2 == 0xffffffff) {
        uVar8 = -(uint)(4 < DAT_0065a258) & DAT_0065a158;
        uVar2 = FUN_00475210(param_2,0);
        if (uVar2 == 0xffffffff) {
          uVar8 = -(uint)(0x12 < DAT_0065a258) & DAT_0065a190;
          uVar2 = FUN_00475210(param_2,0);
          if (uVar2 == 0xffffffff) {
            uVar8 = -(uint)(5 < DAT_0065a258) & DAT_0065a15c;
            uVar2 = FUN_00475210(param_2,0);
            if (uVar2 == 0xffffffff) {
              uVar8 = -(uint)(0x11 < DAT_0065a258) & DAT_0065a18c;
              uVar2 = FUN_00475210(param_2,0);
              if (uVar2 == 0xffffffff) {
                uVar8 = -(uint)(0x15 < DAT_0065a258) & DAT_0065a19c;
                uVar2 = FUN_00475210(param_2,0);
                if (uVar2 == 0xffffffff) {
                  return;
                }
              }
            }
          }
        }
      }
    }
    uVar3 = FUN_00474210(s_SaleType_005e3df0);
    if (uVar2 < *(uint *)(uVar8 + 0x24)) {
      iVar4 = *(int *)(*(int *)(uVar8 + 0x34) + uVar2 * 4);
      if (iVar4 == 0) {
        iVar4 = *(int *)(uVar8 + 0x38);
      }
      if (uVar3 < (uint)(int)*(short *)(iVar4 + 0xc)) {
        iVar4 = *(int *)(*(int *)(uVar8 + 0x34) + uVar2 * 4);
        if (iVar4 == 0) {
          iVar4 = *(int *)(uVar8 + 0x38);
        }
        iVar4 = *(int *)(*(int *)(iVar4 + 0x10) + uVar3 * 4);
        if (iVar4 != 0) {
          if (iVar4 != 1) {
            return;
          }
          iVar4 = FUN_0048e630(*(undefined4 *)(uVar8 + 8),uVar2);
          if (iVar4 == 0) {
            return;
          }
          iVar4 = FUN_0052da90(param_2,*(undefined4 *)(param_1 + 0x17c),param_3);
          if (iVar4 == 0) {
            return;
          }
          psVar1 = (short *)(param_1 + 0x194);
          if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
            FUN_00533280(0xffffffff);
          }
          puVar9 = local_48;
          puVar5 = (undefined4 *)(*(int *)(param_1 + 0x198) + *psVar1 * 0x48);
          for (iVar4 = 0x12; iVar4 != 0; iVar4 = iVar4 + -1) {
            *puVar5 = *puVar9;
            puVar9 = puVar9 + 1;
            puVar5 = puVar5 + 1;
          }
          *psVar1 = *psVar1 + 1;
          return;
        }
      }
    }
    iVar4 = FUN_0052da90(param_2,*(undefined4 *)(param_1 + 0x17c),param_3);
    if (iVar4 == 0) {
      return;
    }
    psVar1 = (short *)(param_1 + 0x194);
    if (*(short *)(param_1 + 0x196) <= *(short *)(param_1 + 0x194)) {
      FUN_00533280(0xffffffff);
    }
    puVar9 = local_48;
    puVar5 = (undefined4 *)(*(int *)(param_1 + 0x198) + *psVar1 * 0x48);
    for (iVar4 = 0x12; iVar4 != 0; iVar4 = iVar4 + -1) {
      *puVar5 = *puVar9;
      puVar9 = puVar9 + 1;
      puVar5 = puVar5 + 1;
    }
    *psVar1 = *psVar1 + 1;
    return;
  }
  uVar2 = ((*(uint *)(param_1 + 0x17c) & 4) != 0) + 1;
  if (uVar2 < DAT_0065a258) {
    iVar4 = (&DAT_0065a148)[uVar2];
  }
  else {
    iVar4 = 0;
  }
  uVar2 = FUN_00475210(param_2,0);
  if (uVar2 == 0xffffffff) {
    return;
  }
  uVar8 = FUN_00474210(s_SaleType_005e3dfc);
  if (uVar2 < *(uint *)(iVar4 + 0x24)) {
    iVar7 = *(int *)(*(int *)(iVar4 + 0x34) + uVar2 * 4);
    if (iVar7 == 0) {
      iVar7 = *(int *)(iVar4 + 0x38);
    }
    if ((uint)(int)*(short *)(iVar7 + 0xc) <= uVar8) goto LAB_00530942;
    iVar7 = *(int *)(*(int *)(iVar4 + 0x34) + uVar2 * 4);
    if (iVar7 == 0) {
      iVar7 = *(int *)(iVar4 + 0x38);
    }
    iVar7 = *(int *)(*(int *)(iVar7 + 0x10) + uVar8 * 4);
    if (iVar7 == 0) goto LAB_00530942;
    if (iVar7 != 1) {
      return;
    }
    iVar4 = FUN_0048e630(*(undefined4 *)(iVar4 + 8),uVar2);
    if (iVar4 == 0) {
      return;
    }
    iVar4 = FUN_0052da90(param_2,*(undefined4 *)(param_1 + 0x17c),param_3);
    if (iVar4 == 0) {
      return;
    }
    if (*(short *)(param_1 + 0x194) < *(short *)(param_1 + 0x196)) goto LAB_00530ab4;
    iVar7 = *(short *)(param_1 + 0x196) + 4;
    puVar5 = (undefined4 *)FUN_00482fb0(iVar7 * 0x48);
    iVar4 = (int)*(short *)(param_1 + 0x196);
    puVar9 = puVar5 + iVar4 * 0x12;
    for (uVar2 = (uint)((iVar7 - iVar4) * 0x48) >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar9 = 0;
      puVar9 = puVar9 + 1;
    }
    for (iVar6 = 0; iVar6 != 0; iVar6 = iVar6 + -1) {
      *(undefined1 *)puVar9 = 0;
      puVar9 = (undefined4 *)((int)puVar9 + 1);
    }
    puVar9 = *(undefined4 **)(param_1 + 0x198);
    if (puVar9 != (undefined4 *)0x0) {
      puVar10 = puVar9;
      puVar11 = puVar5;
      for (uVar2 = (uint)(iVar4 * 0x48) >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
        *puVar11 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar11 = puVar11 + 1;
      }
      for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
        *(undefined1 *)puVar11 = *(undefined1 *)puVar10;
        puVar10 = (undefined4 *)((int)puVar10 + 1);
        puVar11 = (undefined4 *)((int)puVar11 + 1);
      }
      FUN_004830f0(puVar9);
    }
  }
  else {
LAB_00530942:
    iVar4 = FUN_0052da90(param_2,*(undefined4 *)(param_1 + 0x17c),param_3);
    if (iVar4 == 0) {
      return;
    }
    if (*(short *)(param_1 + 0x194) < *(short *)(param_1 + 0x196)) goto LAB_00530ab4;
    iVar7 = *(short *)(param_1 + 0x196) + 4;
    puVar5 = (undefined4 *)FUN_00482fb0(iVar7 * 0x48);
    iVar4 = (int)*(short *)(param_1 + 0x196);
    puVar9 = puVar5 + iVar4 * 0x12;
    for (uVar2 = (uint)((iVar7 - iVar4) * 0x48) >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar9 = 0;
      puVar9 = puVar9 + 1;
    }
    for (iVar6 = 0; iVar6 != 0; iVar6 = iVar6 + -1) {
      *(undefined1 *)puVar9 = 0;
      puVar9 = (undefined4 *)((int)puVar9 + 1);
    }
    puVar9 = *(undefined4 **)(param_1 + 0x198);
    if (puVar9 != (undefined4 *)0x0) {
      puVar10 = puVar9;
      puVar11 = puVar5;
      for (uVar2 = (uint)(iVar4 * 0x48) >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
        *puVar11 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar11 = puVar11 + 1;
      }
      for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
        *(undefined1 *)puVar11 = *(undefined1 *)puVar10;
        puVar10 = (undefined4 *)((int)puVar10 + 1);
        puVar11 = (undefined4 *)((int)puVar11 + 1);
      }
      FUN_004830f0(puVar9);
    }
  }
  *(undefined4 **)(param_1 + 0x198) = puVar5;
  *(short *)(param_1 + 0x196) = (short)iVar7;
LAB_00530ab4:
  puVar9 = local_48;
  puVar5 = (undefined4 *)(*(int *)(param_1 + 0x198) + *(short *)(param_1 + 0x194) * 0x48);
  for (iVar4 = 0x12; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar5 = *puVar9;
    puVar9 = puVar9 + 1;
    puVar5 = puVar5 + 1;
  }
  *(short *)(param_1 + 0x194) = *(short *)(param_1 + 0x194) + 1;
  return;
}


