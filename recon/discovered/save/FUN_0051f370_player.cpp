// FUN_0051f370 @ 0051f370 size=820

void __fastcall FUN_0051f370(int *param_1)

{
  byte bVar1;
  int *piVar2;
  int iVar3;
  undefined4 *puVar4;
  byte *pbVar5;
  byte *pbVar6;
  int *piVar7;
  int iVar8;
  undefined4 *puVar9;
  byte *pbVar10;
  int iVar11;
  undefined4 *puVar12;
  int *piVar13;
  bool bVar14;
  int local_8;
  int local_4;
  
  iVar8 = 0;
  if (0 < param_1[5]) {
    do {
      FUN_00520040(iVar8);
      iVar8 = iVar8 + 1;
    } while (iVar8 < param_1[5]);
  }
  param_1[5] = 0;
  param_1[6] = 0;
  local_4 = 0;
  if (0 < *param_1) {
    do {
      iVar8 = *(int *)(param_1[4] + local_4 * 4);
      if (iVar8 != 0) {
        if (*(char *)(iVar8 + 0x494) != '\0') {
          iVar11 = 0;
          if (0 < param_1[5]) {
            do {
              iVar3 = FUN_0059a530(iVar8 + 0x494,*(int *)(param_1[9] + iVar11 * 4) + 4);
              if (iVar3 == 0) break;
              iVar11 = iVar11 + 1;
            } while (iVar11 < param_1[5]);
          }
          if (param_1[5] <= iVar11) {
            iVar11 = *(int *)(iVar8 + 0x4dc);
            if ((((iVar11 < 0) || (*param_1 <= iVar11)) ||
                (iVar11 = *(int *)(param_1[4] + iVar11 * 4), iVar11 == 0)) ||
               ((iVar11 = FUN_0059a530(iVar8 + 0x494,iVar11 + 0x494), iVar11 != 0 ||
                (iVar11 = FUN_0059a530(iVar8 + 0x4c6,
                                       *(int *)(param_1[4] + *(int *)(iVar8 + 0x4dc) * 4) + 0x4c6),
                iVar11 != 0)))) {
              *(int *)(iVar8 + 0x4dc) = local_4;
            }
            puVar4 = (undefined4 *)FUN_00482fb0(0x60);
            puVar9 = (undefined4 *)(iVar8 + 0x490);
            puVar12 = puVar4;
            for (iVar11 = 0x18; iVar11 != 0; iVar11 = iVar11 + -1) {
              *puVar12 = *puVar9;
              puVar9 = puVar9 + 1;
              puVar12 = puVar12 + 1;
            }
            FUN_0041c840(puVar4);
          }
        }
      }
      local_4 = local_4 + 1;
    } while (local_4 < *param_1);
  }
  local_4 = 0;
  if (0 < param_1[5]) {
    do {
      piVar2 = *(int **)(param_1[9] + local_4 * 4);
      if ((piVar2 != (int *)0x0) && (local_8 = 0, 0 < *param_1)) {
        do {
          iVar8 = *(int *)(param_1[4] + local_8 * 4);
          if (iVar8 != 0) {
            pbVar5 = (byte *)(piVar2 + 1);
            iVar8 = FUN_0059a530(pbVar5,iVar8 + 0x494);
            if (iVar8 == 0) {
              pbVar6 = (byte *)((int)piVar2 + 0x36);
              iVar8 = FUN_0059a530(pbVar6,*(int *)(param_1[4] + local_8 * 4) + 0x4c6);
              if (iVar8 == 0) {
                iVar8 = *(int *)(param_1[4] + local_8 * 4);
                if (piVar2 != (int *)0x0) {
                  pbVar10 = (byte *)(iVar8 + 0x494);
                  do {
                    bVar1 = *pbVar5;
                    bVar14 = bVar1 < *pbVar10;
                    if (bVar1 != *pbVar10) {
LAB_0051f53e:
                      iVar11 = (1 - (uint)bVar14) - (uint)(bVar14 != 0);
                      goto LAB_0051f543;
                    }
                    if (bVar1 == 0) break;
                    bVar1 = pbVar5[1];
                    bVar14 = bVar1 < pbVar10[1];
                    if (bVar1 != pbVar10[1]) goto LAB_0051f53e;
                    pbVar5 = pbVar5 + 2;
                    pbVar10 = pbVar10 + 2;
                  } while (bVar1 != 0);
                  iVar11 = 0;
LAB_0051f543:
                  if (iVar11 == 0) {
                    pbVar5 = (byte *)(iVar8 + 0x4c6);
                    do {
                      bVar1 = *pbVar6;
                      bVar14 = bVar1 < *pbVar5;
                      if (bVar1 != *pbVar5) {
LAB_0051f577:
                        iVar11 = (1 - (uint)bVar14) - (uint)(bVar14 != 0);
                        goto LAB_0051f57c;
                      }
                      if (bVar1 == 0) break;
                      bVar1 = pbVar6[1];
                      bVar14 = bVar1 < pbVar5[1];
                      if (bVar1 != pbVar5[1]) goto LAB_0051f577;
                      pbVar6 = pbVar6 + 2;
                      pbVar5 = pbVar5 + 2;
                    } while (bVar1 != 0);
                    iVar11 = 0;
LAB_0051f57c:
                    if ((((iVar11 == 0) && (*piVar2 == *(int *)(iVar8 + 0x490))) &&
                        (piVar2[0x12] == *(int *)(iVar8 + 0x4d8))) &&
                       (piVar2[0x13] == *(int *)(iVar8 + 0x4dc))) goto LAB_0051f5c8;
                  }
                  piVar7 = piVar2;
                  piVar13 = (int *)(iVar8 + 0x490);
                  for (iVar11 = 0x18; iVar11 != 0; iVar11 = iVar11 + -1) {
                    *piVar13 = *piVar7;
                    piVar7 = piVar7 + 1;
                    piVar13 = piVar13 + 1;
                  }
                  FUN_00584960(iVar8,0);
                }
LAB_0051f5c8:
                piVar7 = (int *)(**(code **)(**(int **)(param_1[4] + local_8 * 4) + 0xa8))
                                          (s_Swag_Bag_005e2b64);
                if (piVar7 == (int *)0x0) {
                  if ((DAT_0067682c == 0) ||
                     (piVar7 = (int *)FUN_00474e20(s_Swag_Bag_005e2b70,0,0xffffffff,1),
                     piVar7 == (int *)0x0)) goto LAB_0051f663;
                  (**(code **)(**(int **)(param_1[4] + local_8 * 4) + 0x58))(piVar7,0xffffffff);
                }
                if (local_8 == piVar2[0x13]) {
                  iVar11 = *piVar7;
                  iVar8 = 0;
                }
                else {
                  iVar8 = (**(code **)(**(int **)(param_1[4] + piVar2[0x13] * 4) + 0xa8))
                                    (s_Swag_Bag_005e2b7c);
                  if (iVar8 == 0) goto LAB_0051f663;
                  iVar11 = *piVar7;
                }
                (**(code **)(iVar11 + 0x174))(iVar8);
              }
            }
          }
LAB_0051f663:
          local_8 = local_8 + 1;
        } while (local_8 < *param_1);
      }
      local_4 = local_4 + 1;
    } while (local_4 < param_1[5]);
  }
  FUN_0051f6b0();
  FUN_0057b1f0();
  return;
}


