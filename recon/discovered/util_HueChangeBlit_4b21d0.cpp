// FUN_004b21d0 @ 004b21d0 size=974

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_004b21d0(undefined4 *param_1,uint *param_2)

{
  uint uVar1;
  int iVar2;
  ushort uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  int iVar11;
  uint uVar12;
  undefined4 *puVar13;
  uint *puVar14;
  uint uVar15;
  uint uVar16;
  undefined4 *puVar17;
  uint *puVar18;
  uint uVar19;
  uint uVar20;
  ushort local_ec;
  ushort *local_e4;
  int local_dc;
  ushort *local_d8;
  int local_cc;
  uint local_b4 [16];
  int local_74;
  int local_70;
  undefined4 local_60 [4];
  int local_50;
  int local_34;
  
  uVar19 = param_2[0x10];
  if (((((int)uVar19 < 1) || ((int)param_2[0x11] < 1)) || ((int)param_2[0xc] < 1)) ||
     (((int)param_2[0xd] < 1 || (uVar1 = param_1[2], (uVar1 & 0x4000) != 0)))) {
    return 0;
  }
  puVar13 = param_1;
  puVar17 = local_60;
  for (iVar10 = 0x16; iVar10 != 0; iVar10 = iVar10 + -1) {
    *puVar17 = *puVar13;
    puVar13 = puVar13 + 1;
    puVar17 = puVar17 + 1;
  }
  iVar11 = 2;
  puVar14 = param_2;
  puVar18 = local_b4;
  for (iVar10 = 0x15; iVar10 != 0; iVar10 = iVar10 + -1) {
    *puVar18 = *puVar14;
    puVar14 = puVar14 + 1;
    puVar18 = puVar18 + 1;
  }
  if ((uVar1 & 1) == 0) {
    if ((uVar1 & 6) == 0) {
      if ((uVar1 & 8) == 0) {
        if ((uVar1 & 0x10) != 0) {
          iVar11 = 4;
        }
      }
      else {
        iVar11 = 3;
      }
    }
    else {
      iVar11 = 2;
    }
  }
  else {
    iVar11 = 1;
  }
  iVar10 = 2;
  uVar1 = param_1[3];
  if ((uVar1 & 1) == 0) {
    if ((uVar1 & 6) == 0) {
      if ((uVar1 & 8) == 0) {
        if ((uVar1 & 0x10) != 0) {
          iVar10 = 4;
        }
      }
      else {
        iVar10 = 3;
      }
    }
    else {
      iVar10 = 2;
    }
  }
  else {
    iVar10 = 1;
  }
  iVar2 = param_1[9];
  uVar1 = param_2[0xc];
  if ((*param_2 & 0x40) == 0) {
    uVar15 = param_1[0x10];
    iVar5 = param_2[0xf] * uVar15 + param_2[0xe];
  }
  else {
    iVar5 = ((param_2[0xf] - 1) + param_2[0x11]) * param_1[0x10] + param_2[0xe];
    uVar15 = uVar19;
    uVar19 = param_1[0x10];
  }
  uVar4 = param_2[0x12];
  local_d8 = (ushort *)(local_34 + (iVar11 * iVar5 >> 1) * 2);
  local_e4 = (ushort *)
             (local_50 +
             ((int)(iVar10 * ((param_2[0xb] + param_2[5]) * iVar2 + param_2[10] + param_2[4])) >> 1)
             * 2);
  if (0 < local_70) {
    local_dc = local_70;
    do {
      if (0 < local_74) {
        local_cc = local_74;
        do {
          local_ec = *local_d8;
          local_d8 = local_d8 + 1;
          if (local_ec == 0) {
LAB_004b2530:
            if ((local_b4[0] & 0x100) == 0) goto LAB_004b253f;
          }
          else {
            iVar5 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
            if (iVar5 == 0xf) {
              uVar16 = local_ec >> 10 & 0x1f;
              uVar3 = local_ec >> 5;
            }
            else {
              uVar16 = (uint)(local_ec >> 0xb);
              uVar3 = local_ec >> 6;
            }
            uVar20 = (uVar3 & 0x1f) * 8;
            uVar12 = (local_ec & 0x1f) * 8;
            if ((uVar16 * 8 < uVar20) && (uVar12 < uVar20)) {
              uVar6 = __ftol();
              uVar7 = __ftol();
              uVar8 = __ftol();
              uVar9 = __ftol();
              uVar16 = uVar16 * 8;
              switch((ushort)uVar4 / 0x3c) {
              case 0:
                uVar12 = uVar6;
                uVar16 = uVar9;
                uVar20 = uVar8;
                break;
              case 1:
                uVar12 = uVar6;
                uVar16 = uVar7;
                uVar20 = uVar9;
                break;
              case 2:
                uVar12 = uVar8;
                uVar16 = uVar6;
                uVar20 = uVar9;
                break;
              case 3:
                uVar12 = uVar9;
                uVar16 = uVar6;
                uVar20 = uVar7;
                break;
              case 4:
                uVar12 = uVar9;
                uVar16 = uVar8;
                uVar20 = uVar6;
                break;
              case 5:
                uVar12 = uVar7;
                uVar16 = uVar9;
                uVar20 = uVar6;
              }
              iVar5 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
              if (iVar5 == 0xf) {
                local_ec = (ushort)((((int)uVar16 >> 3) << 5 | (int)uVar20 >> 3) << 5);
              }
              else {
                local_ec = (ushort)((((int)uVar16 >> 3) << 5 | (int)uVar20 >> 3) << 6);
              }
              local_ec = local_ec | (ushort)((int)uVar12 >> 3);
            }
            if (local_ec == 0) goto LAB_004b2530;
LAB_004b253f:
            *local_e4 = local_ec;
          }
          local_e4 = local_e4 + 1;
          local_cc = local_cc + -1;
        } while (local_cc != 0);
      }
      local_d8 = local_d8 + ((int)(iVar11 * (uVar15 - uVar19)) >> 1);
      local_e4 = local_e4 + ((int)(iVar10 * (iVar2 - uVar1)) >> 1);
      local_dc = local_dc + -1;
    } while (local_dc != 0);
  }
  return 1;
}


