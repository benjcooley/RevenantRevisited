// FUN_0052ff40 @ 0052ff40 size=1578

/* WARNING: Removing unreachable block (ram,0x005303c0) */
/* WARNING: Removing unreachable block (ram,0x005303e6) */
/* WARNING: Removing unreachable block (ram,0x005303f5) */

undefined4 __thiscall FUN_0052ff40(int *param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  undefined4 uVar8;
  uint uVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
  
  iVar4 = param_1[0x62];
  if (((iVar4 == -1) || ((short)param_1[0x65] < iVar4)) || ((int *)param_1[0x6d] == (int *)0x0)) {
    (**(code **)(*param_1 + 0x2c))(1);
    return 1;
  }
  if (param_3 != 3000) {
    param_1[0x62] = -1;
    (**(code **)(*param_1 + 0x2c))(1);
    return 0;
  }
  if ((param_1[0x5f] & 1U) != 0) {
    iVar7 = param_1[0x66];
    iVar3 = (**(code **)(*(int *)param_1[0x6d] + 0x84))(&DAT_005e3dcc);
    if (iVar3 < *(int *)(iVar7 + iVar4 * 0x48 + 0x38)) {
      iVar4 = param_1[0x68];
      if ((iVar4 == 0) || (iVar7 = param_1[0x6a], iVar7 == 0)) goto LAB_0053051d;
      if (iVar4 == 0) goto LAB_0052fff0;
      piVar2 = *(int **)(iVar4 + 0xd8);
    }
    else {
      iVar4 = (**(code **)(*(int *)param_1[0x6d] + 0x88))();
      if (iVar4 == 0) goto LAB_0053051d;
      iVar7 = *(int *)(param_1[0x66] + 8 + param_1[0x62] * 0x48);
      iVar4 = param_1[0x66] + param_1[0x62] * 0x48;
      uVar8 = 1;
      if (((*(byte *)(param_1 + 0x5f) & 0x10) != 0) &&
         (uVar8 = *(undefined4 *)(iVar4 + 0x3c), iVar7 == 0)) {
        iVar7 = *(int *)(iVar4 + 4);
      }
      if (((DAT_0066829c == 0) || (DAT_00676828 == 0)) || (DAT_0067682c != 0)) {
        (**(code **)(*(int *)param_1[0x6d] + 0x54))(iVar7,uVar8,0xffffffff);
        (**(code **)(*(int *)param_1[0x6d] + 0x78))
                  (&DAT_005e3dd4,*(undefined4 *)(param_1[0x66] + 0x38 + param_1[0x62] * 0x48));
      }
      else {
        FUN_005869a0(param_1[0x6d],iVar7,uVar8);
      }
      iVar4 = param_1[0x68];
      if ((iVar4 == 0) || (iVar7 = param_1[0x69], iVar7 == 0)) goto LAB_0053051d;
      if (iVar4 == 0) goto LAB_0052fff0;
      piVar2 = *(int **)(iVar4 + 0xd8);
    }
    if ((piVar2 == (int *)0x0) || (*piVar2 != 0x10)) {
LAB_0052fff0:
      FUN_004d0a20(iVar7,0xffffffff,0);
      (**(code **)(*param_1 + 0x2c))(1);
      return 1;
    }
    goto LAB_0053051d;
  }
  if ((param_1[0x5f] & 2U) == 0) goto LAB_0053051d;
  uVar9 = -(uint)(2 < DAT_0065a258) & DAT_0065a150;
  uVar5 = FUN_00475210(*(undefined4 *)(param_1[0x66] + 8 + iVar4 * 0x48),0);
  if (uVar5 == 0xffffffff) {
    uVar9 = -(uint)(1 < DAT_0065a258) & DAT_0065a14c;
    uVar5 = FUN_00475210(*(undefined4 *)(param_1[0x66] + 8 + param_1[0x62] * 0x48),0);
    if (uVar5 != 0xffffffff) goto LAB_00530291;
    uVar9 = -(uint)(4 < DAT_0065a258) & DAT_0065a158;
    uVar5 = FUN_00475210(*(undefined4 *)(param_1[0x66] + 8 + param_1[0x62] * 0x48),0);
    if (uVar5 != 0xffffffff) goto LAB_00530291;
    uVar9 = -(uint)(0x12 < DAT_0065a258) & DAT_0065a190;
    uVar5 = FUN_00475210(*(undefined4 *)(param_1[0x66] + 8 + param_1[0x62] * 0x48),0);
    if (uVar5 != 0xffffffff) goto LAB_00530291;
    uVar9 = -(uint)(5 < DAT_0065a258) & DAT_0065a15c;
    uVar5 = FUN_00475210(*(undefined4 *)(param_1[0x66] + 8 + param_1[0x62] * 0x48),0);
    if (uVar5 != 0xffffffff) goto LAB_00530291;
    uVar9 = -(uint)(0x11 < DAT_0065a258) & DAT_0065a18c;
    uVar5 = FUN_00475210(*(undefined4 *)(param_1[0x66] + 8 + param_1[0x62] * 0x48),0);
    if (uVar5 != 0xffffffff) goto LAB_00530291;
    uVar9 = -(uint)(0x15 < DAT_0065a258) & DAT_0065a19c;
    uVar5 = FUN_00475210(*(undefined4 *)(param_1[0x66] + 8 + param_1[0x62] * 0x48),0);
    if (uVar5 != 0xffffffff) goto LAB_00530291;
  }
  else {
LAB_00530291:
    uVar6 = FUN_00474210(s_SaleType_005e3ddc);
    if (uVar5 < *(uint *)(uVar9 + 0x24)) {
      iVar4 = *(int *)(*(int *)(uVar9 + 0x34) + uVar5 * 4);
      if (iVar4 == 0) {
        iVar4 = *(int *)(uVar9 + 0x38);
      }
      if (uVar6 < (uint)(int)*(short *)(iVar4 + 0xc)) {
        iVar4 = *(int *)(*(int *)(uVar9 + 0x34) + uVar5 * 4);
        if (iVar4 == 0) {
          iVar4 = *(int *)(uVar9 + 0x38);
        }
        if ((*(int *)(*(int *)(iVar4 + 0x10) + uVar6 * 4) == 1) &&
           (iVar4 = FUN_0048e630(*(undefined4 *)(uVar9 + 8),uVar5), iVar4 == 0)) {
          FUN_0048e670(*(undefined4 *)(uVar9 + 8),uVar5);
        }
      }
    }
  }
  iVar4 = param_1[0x6d];
  if (iVar4 == 0) {
    FUN_0052f310();
    iVar4 = param_1[0x65];
    uVar5 = param_1[0x62];
    if (uVar5 < (uint)(int)(short)iVar4) {
      iVar7 = *(short *)((int)param_1 + 0x196) + -1;
      if ((int)uVar5 < iVar7) {
        iVar7 = iVar7 - uVar5;
        puVar11 = (undefined4 *)(param_1[0x66] + uVar5 * 0x48);
        do {
          iVar7 = iVar7 + -1;
          puVar10 = puVar11 + 0x12;
          puVar12 = puVar11;
          for (iVar3 = 0x12; iVar3 != 0; iVar3 = iVar3 + -1) {
            *puVar12 = *puVar10;
            puVar10 = puVar10 + 1;
            puVar12 = puVar12 + 1;
          }
          puVar11 = puVar11 + 0x12;
        } while (iVar7 != 0);
      }
      sVar1 = (short)iVar4 + -1;
      *(short *)(param_1 + 0x65) = sVar1;
      if (sVar1 < 1) {
        iVar4 = param_1[0x66];
        goto joined_r0x005304f4;
      }
    }
  }
  else {
    if (((DAT_0066829c == 0) || (DAT_00676828 == 0)) || (DAT_0067682c != 0)) {
      iVar4 = FUN_005330a0(iVar4,*(undefined4 *)(param_1[0x66] + 8 + param_1[0x62] * 0x48));
      if (iVar4 != 0) {
        (**(code **)(*(int *)param_1[0x6d] + 0x54))
                  (&DAT_005e3de8,*(undefined4 *)(param_1[0x66] + 0x38 + param_1[0x62] * 0x48),
                   0xffffffff);
        FUN_00519230();
      }
    }
    else {
      FUN_00586a10(iVar4,*(undefined4 *)(param_1[0x66] + 8 + param_1[0x62] * 0x48));
    }
    FUN_0046dfb0();
    FUN_0052f310();
    iVar4 = param_1[0x65];
    uVar5 = param_1[0x62];
    if (uVar5 < (uint)(int)(short)iVar4) {
      iVar7 = *(short *)((int)param_1 + 0x196) + -1;
      if ((int)uVar5 < iVar7) {
        iVar7 = iVar7 - uVar5;
        puVar11 = (undefined4 *)(param_1[0x66] + uVar5 * 0x48);
        do {
          iVar7 = iVar7 + -1;
          puVar10 = puVar11 + 0x12;
          puVar12 = puVar11;
          for (iVar3 = 0x12; iVar3 != 0; iVar3 = iVar3 + -1) {
            *puVar12 = *puVar10;
            puVar10 = puVar10 + 1;
            puVar12 = puVar12 + 1;
          }
          puVar11 = puVar11 + 0x12;
        } while (iVar7 != 0);
      }
      sVar1 = (short)iVar4 + -1;
      *(short *)(param_1 + 0x65) = sVar1;
      if (sVar1 < 1) {
        iVar4 = param_1[0x66];
joined_r0x005304f4:
        if (iVar4 != 0) {
          FUN_004830f0(iVar4);
        }
        param_1[0x66] = 0;
        *(undefined2 *)(param_1 + 0x65) = 0;
        *(undefined2 *)((int)param_1 + 0x196) = 0;
      }
    }
  }
  param_1[0x62] = -1;
LAB_0053051d:
  (**(code **)(*param_1 + 0x2c))(1);
  return 1;
}


