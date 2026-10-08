// FUN_00414d70 @ 00414d70 size=1186

undefined4
FUN_00414d70(int param_1,int param_2,undefined4 param_3,int *param_4,int *param_5,int param_6,
            int param_7,undefined4 param_8,int param_9,int param_10,int param_11,int param_12,
            undefined4 param_13,uint param_14)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  uint uVar13;
  uint uVar14;
  int iVar15;
  undefined4 uVar16;
  int *piVar17;
  int *piVar18;
  uint uVar19;
  int iStack_58;
  int local_54;
  int iStack_48;
  int iStack_44;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  
  uVar16 = 0;
  if (param_4 != (int *)0x0) {
    if (param_5 == (int *)0x0) goto LAB_00414db1;
    iVar4 = (**(code **)(*param_4 + 8))();
    iVar5 = (**(code **)(*param_5 + 8))();
    if (iVar4 != iVar5) {
      return 0;
    }
  }
  if ((param_5 != (int *)0x0) && (param_4 == (int *)0x0)) {
    return 0;
  }
LAB_00414db1:
  if (((param_6 < 1) || (param_7 < 1)) ||
     ((param_4 != (int *)0x0 && ((param_11 < 1 || (param_12 < 1)))))) {
    return 0;
  }
  local_54 = 0;
  if (param_4 == (int *)0x0) {
    uVar10 = 0;
  }
  else {
    if (((param_14 & 0x3f) == 0) && ((param_4[0xe] & 0x30000U) != 0)) {
      param_14 = param_14 | 4;
    }
    iVar4 = (**(code **)(*param_4 + 8))();
    if (iVar4 == 0x10) {
      if ((param_5 != (int *)0x0) && (param_4[0x1a] != param_5[0x1a])) {
        return 0;
      }
      iStack_48 = 0;
      if (param_4[0x1a] < 1) {
        return 0;
      }
      iStack_44 = 0;
      do {
        piVar18 = (int *)(iStack_44 + param_4[0x1b]);
        piVar1 = *(int **)(param_4[0x1c] + iStack_48 * 4);
        if (param_5 == (int *)0x0) {
          piVar17 = (int *)0x0;
        }
        else {
          piVar17 = *(int **)(param_5[0x1c] + iStack_48 * 4);
        }
        if (piVar1 == (int *)0x0) {
          uStack_24 = 0;
          uStack_28 = 0;
        }
        else {
          uStack_24 = (**(code **)(*piVar1 + 0xc))();
          uStack_28 = (**(code **)(*piVar1 + 0x10))();
        }
        uVar16 = 0;
        if (piVar17 == (int *)0x0) {
          uStack_2c = 0;
        }
        else {
          uStack_2c = (**(code **)(*piVar17 + 0xc))();
          uVar16 = (**(code **)(*piVar17 + 0x10))();
        }
        iVar4 = *piVar18;
        uVar13 = (uint)param_4[0x19] >> 0x1d & 2;
        if ((((iVar4 <= param_9 + param_11 + -1) && (iVar5 = piVar18[2], param_9 <= iVar5)) &&
            (iVar2 = piVar18[1], iVar2 <= param_10 + param_12 + -1)) &&
           (iVar3 = piVar18[3], param_10 <= iVar3)) {
          uVar19 = (param_9 - iVar4) - uVar13;
          uVar19 = ((int)uVar19 < 1) - 1 & uVar19;
          uVar14 = (param_10 - iVar2) - uVar13;
          uVar14 = ((int)uVar14 < 1) - 1 & uVar14;
          iVar15 = (((iVar5 + uVar13 * -2) - iVar4) - uVar19) + 1;
          local_54 = (((param_9 - iVar4) - uVar19) - uVar13) + param_11;
          if (iVar15 <= local_54) {
            local_54 = iVar15;
          }
          iVar15 = (((iVar3 + uVar13 * -2) - iVar2) - uVar14) + 1;
          iStack_58 = (((param_10 - iVar2) - uVar14) - uVar13) + param_12;
          if (iVar15 <= iStack_58) {
            iStack_58 = iVar15;
          }
          uVar6 = (iVar4 - param_9) + uVar13;
          iVar15 = (int)((uVar6 & ((int)uVar6 < 1) - 1) * param_6) / param_11;
          uVar6 = (iVar2 - param_10) + uVar13;
          iVar7 = (int)((uVar6 & ((int)uVar6 < 1) - 1) * param_7) / param_12;
          if ((int)((iVar5 - uVar13) + 1) < param_9 + param_11) {
            iVar8 = (local_54 * param_6) / param_11;
          }
          else {
            iVar8 = param_6 - iVar15;
          }
          if ((int)((iVar3 - uVar13) + 1) < param_10 + param_12) {
            iVar9 = (iStack_58 * param_7) / param_12;
          }
          else {
            iVar9 = param_7 - iVar7;
          }
          local_54 = FUN_00414550(iVar15 + param_1,iVar7 + param_2,param_3,uStack_24,uStack_28,
                                  uStack_2c,uVar16,iVar8,iVar9,param_8,(iVar5 - iVar4) + 1,
                                  (iVar3 - iVar2) + 1,uVar19,uVar14,local_54,iStack_58,param_13,
                                  param_14);
        }
        iStack_48 = iStack_48 + 1;
        iStack_44 = iStack_44 + 0x10;
      } while (iStack_48 < param_4[0x1a]);
      return local_54;
    }
    uVar10 = (**(code **)(*param_4 + 0xc))();
    uVar16 = (**(code **)(*param_4 + 0x10))();
  }
  if (param_5 == (int *)0x0) {
    uVar11 = 0;
    uVar12 = 0;
  }
  else {
    uVar11 = (**(code **)(*param_4 + 0xc))();
    uVar12 = (**(code **)(*param_4 + 0x10))();
  }
  uVar16 = FUN_00414550(param_1,param_2,param_3,uVar10,uVar16,uVar11,uVar12,param_6,param_7,param_8,
                        param_4[1],param_4[2],param_9,param_10,param_11,param_12,param_13,param_14);
  return uVar16;
}


