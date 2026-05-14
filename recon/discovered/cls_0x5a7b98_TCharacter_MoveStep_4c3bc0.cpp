// FUN_004c3bc0_MoveStep @ 004c3bc0 size=2762

/* WARNING: Removing unreachable block (ram,0x004c46a1) */

uint __fastcall FUN_004c3bc0_MoveStep(int *param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  uint local_94;
  int local_90;
  int local_8c;
  int local_88;
  int *local_84;
  int *local_80;
  int local_7c;
  int local_78;
  int local_74;
  int local_70;
  int local_6c;
  int *local_68;
  int local_64;
  int local_60;
  int local_5c;
  int local_58;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20 [5];
  int local_c;
  int local_8;
  undefined1 local_4 [4];
  
  if ((((param_1[2] & 1U) != 0) || (-1 < (short)param_1[0x1f])) || ((param_1[2] & 0x800000U) != 0))
  {
    return 0;
  }
  piVar1 = param_1 + 4;
  local_94 = 0;
  iVar3 = FUN_00452e10_GetWalkHeight(piVar1,*(undefined2 *)((int)param_1 + 0xe),0);
  iVar9 = param_1[6] - iVar3;
  if (iVar9 < -0x10) {
    param_1[6] = iVar3;
  }
  else if (0x10 < iVar9) {
    iVar6 = param_1[9] + -0x60000;
    if (iVar6 < -0x31ffff) {
      iVar6 = -0x320000;
    }
    local_38 = param_1[5];
    param_1[9] = iVar6;
    local_3c = *piVar1;
    local_94 = 4;
    local_34 = iVar3;
    (**(code **)(*param_1 + 8))(&local_3c,*(undefined2 *)((int)param_1 + 0xe),0);
  }
  if (iVar9 < 1) {
    param_1[9] = 0;
  }
  if (param_1[0x43] != 0) {
    param_1[0x43] = 0;
    return 8;
  }
  if (param_1[0x3b] == 0) {
    iVar3 = *(int *)(param_1[0x3f] + 0x1f8);
    if (((iVar3 < 1) || (param_1 == (int *)0x0)) ||
       ((piVar2 = (int *)param_1[0x36], piVar2 == (int *)0x0 ||
        ((*piVar2 != 4 || ((piVar2[0x18] & 0x200U) != 0)))))) {
      if ((0 < *(int *)(param_1[0x3f] + 0x1ec)) &&
         ((((param_1 != (int *)0x0 && (piVar2 = (int *)param_1[0x36], piVar2 != (int *)0x0)) &&
           (*piVar2 == 2)) && ((piVar2[0x18] & 0x200U) == 0)))) {
        uVar4 = (**(code **)(*param_1 + 0x30c))(0);
        iVar3 = FUN_004dab80_TActionBlock_Is(uVar4);
        if (iVar3 != 0) {
          FUN_0046db20_ConvertToVector(param_1[0x2c],*(int *)(param_1[0x3f] + 0x1ec) << 0x10,&local_48,0);
          goto LAB_004c3eaf;
        }
      }
      if (((*(int *)(param_1[0x3f] + 0x1ec) < 1) || (param_1 == (int *)0x0)) ||
         (((piVar2 = (int *)param_1[0x36], piVar2 == (int *)0x0 ||
           ((*piVar2 != 2 || ((piVar2[0x18] & 0x200U) != 0)))) ||
          (iVar3 = FUN_004dab80_TActionBlock_Is(s_sneak_005df804), iVar3 == 0)))) {
        if ((((*(int *)(param_1[0x3f] + 0x1f0) < 1) || (param_1 == (int *)0x0)) ||
            (piVar2 = (int *)param_1[0x36], piVar2 == (int *)0x0)) ||
           (((*piVar2 != 2 || ((piVar2[0x18] & 0x200U) != 0)) ||
            (iVar3 = FUN_004dab80_TActionBlock_Is(&DAT_005df80c), iVar3 == 0)))) {
          FUN_00470c30(&local_48);
        }
        else {
          FUN_0046db20_ConvertToVector(param_1[0x2c],*(int *)(param_1[0x3f] + 0x1f0) << 0x10,&local_48,0);
        }
      }
      else {
        FUN_0046db20_ConvertToVector(param_1[0x2c],*(int *)(param_1[0x3f] + 500) << 0x10,&local_48,0);
      }
    }
    else {
      FUN_0046db20_ConvertToVector(param_1[0x2c],iVar3 << 0x10,&local_48,0);
    }
LAB_004c3eaf:
    if (((local_48 == 0) && (local_44 == 0)) &&
       ((local_40 == 0 && (((param_1[7] == 0 && (param_1[8] == 0)) && (param_1[9] == 0)))))) {
      param_1[0xc] = 0;
      param_1[0xb] = 0;
      param_1[10] = 0;
      param_1[0x47] = -1;
      return local_94 | 8;
    }
    local_78 = param_1[0xb] + local_44 + param_1[8];
    local_50 = param_1[0xc] + local_40 + param_1[9];
    local_7c = param_1[10] + local_48 + param_1[7];
  }
  else {
    local_7c = (param_1[0x3c] - *piVar1) * 0x10000;
    local_78 = (param_1[0x3d] - param_1[5]) * 0x10000;
    local_50 = (param_1[0x3e] - param_1[6]) * 0x10000;
    param_1[0xc] = 0;
    param_1[0xb] = 0;
    param_1[10] = 0;
    if (((local_7c == 0) && (local_78 == 0)) && (local_50 == 0)) {
      return local_94 | 8;
    }
    param_1[0x47] = -1;
    local_48 = local_7c;
    local_44 = local_78;
    local_40 = local_50;
  }
  local_68 = param_1 + 10;
  iVar9 = *piVar1 + ((int)(local_7c + (local_7c >> 0x1f & 0xffffU)) >> 0x10);
  iVar7 = param_1[5] + ((int)(local_78 + (local_78 >> 0x1f & 0xffffU)) >> 0x10);
  iVar6 = param_1[6] + ((int)(local_50 + (local_50 >> 0x1f & 0xffffU)) >> 0x10);
  iVar3 = local_7c;
  if (local_7c < 0) {
    iVar3 = -local_7c;
  }
  iVar5 = local_78;
  if (local_78 < 0) {
    iVar5 = -local_78;
  }
  if (iVar3 < iVar5) {
LAB_004c3fdf:
    iVar3 = local_78;
    if (local_78 < 0) {
      iVar3 = -local_78;
    }
    iVar5 = local_7c;
    if (local_7c < 0) {
      iVar5 = -local_7c;
    }
    if (iVar5 < iVar3) {
      iVar3 = local_78;
      if (local_78 < 0) {
        iVar3 = -local_78;
      }
      if (0x7ffff < iVar3) {
        iVar3 = local_78;
        if (local_78 < 0) {
          local_6c = (int)((0x7ffff - local_78) + (0x7ffff - local_78 >> 0x1f & 0x7ffffU)) >> 0x13;
          local_7c = local_7c / local_6c;
          local_78 = local_78 / local_6c;
          local_74 = local_50 / local_6c;
          goto LAB_004c405d;
        }
        goto LAB_004c3fa5;
      }
    }
    local_74 = local_50;
    local_6c = 1;
  }
  else {
    iVar3 = local_7c;
    if (local_7c < 0) {
      iVar3 = -local_7c;
    }
    if (iVar3 < 0x80000) goto LAB_004c3fdf;
    iVar3 = local_7c;
    if (local_7c < 0) {
      iVar3 = -local_7c;
    }
LAB_004c3fa5:
    local_6c = (int)(iVar3 + 0x7ffff + (iVar3 + 0x7ffff >> 0x1f & 0x7ffffU)) >> 0x13;
    local_7c = local_7c / local_6c;
    local_78 = local_78 / local_6c;
    local_74 = local_50 / local_6c;
  }
LAB_004c405d:
  local_84 = (int *)(local_6c * local_7c);
  if (((int)((int)local_84 + ((int)local_84 >> 0x1f & 0xffffU)) >> 0x10) + *piVar1 < iVar9) {
    local_70 = local_6c * 100;
    do {
      local_84 = (int *)((int)local_84 + local_70);
      local_7c = local_7c + 100;
    } while (((int)((int)local_84 + ((int)local_84 >> 0x1f & 0xffffU)) >> 0x10) + *piVar1 < iVar9);
  }
  local_80 = (int *)(local_6c * local_7c);
  if (iVar9 < ((int)((int)local_80 + ((int)local_80 >> 0x1f & 0xffffU)) >> 0x10) + *piVar1) {
    local_70 = local_6c * -100;
    local_84 = local_80;
    do {
      local_84 = local_84 + local_6c * -0x19;
      local_7c = local_7c + -100;
    } while (iVar9 < ((int)((int)local_84 + ((int)local_84 >> 0x1f & 0xffffU)) >> 0x10) + *piVar1);
  }
  iVar3 = ((int)(local_78 * local_6c + (local_78 * local_6c >> 0x1f & 0xffffU)) >> 0x10) +
          param_1[5];
  while (iVar3 < iVar7) {
    local_78 = local_78 + 100;
    iVar3 = ((int)(local_78 * local_6c + (local_78 * local_6c >> 0x1f & 0xffffU)) >> 0x10) +
            param_1[5];
  }
  iVar3 = ((int)(local_78 * local_6c + (local_78 * local_6c >> 0x1f & 0xffffU)) >> 0x10) +
          param_1[5];
  while (iVar7 < iVar3) {
    local_78 = local_78 + -100;
    iVar3 = ((int)(local_78 * local_6c + (local_78 * local_6c >> 0x1f & 0xffffU)) >> 0x10) +
            param_1[5];
  }
  iVar3 = ((int)(local_74 * local_6c + (local_74 * local_6c >> 0x1f & 0xffffU)) >> 0x10) +
          param_1[6];
  while (iVar3 < iVar6) {
    local_74 = local_74 + 100;
    iVar3 = ((int)(local_74 * local_6c + (local_74 * local_6c >> 0x1f & 0xffffU)) >> 0x10) +
            param_1[6];
  }
  iVar3 = ((int)(local_74 * local_6c + (local_74 * local_6c >> 0x1f & 0xffffU)) >> 0x10) +
          param_1[6];
  while (iVar6 < iVar3) {
    local_74 = local_74 + -100;
    iVar3 = ((int)(local_74 * local_6c + (local_74 * local_6c >> 0x1f & 0xffffU)) >> 0x10) +
            param_1[6];
  }
  local_84 = (int *)0x0;
  if (0 < local_6c) {
    do {
      local_90 = *piVar1;
      local_8c = param_1[5];
      local_88 = param_1[6];
      local_5c = local_74;
      iVar3 = local_7c;
      if (local_7c < 1) {
        iVar3 = -local_7c;
      }
      local_64 = local_7c;
      if (0xffff < iVar3) {
        iVar3 = (int)(local_7c + (local_7c >> 0x1f & 0xffffU)) >> 0x10;
        local_90 = local_90 + iVar3;
        local_64 = local_7c + iVar3 * -0x10000;
      }
      iVar3 = local_78;
      if (local_78 < 1) {
        iVar3 = -local_78;
      }
      local_60 = local_78;
      if (0xffff < iVar3) {
        iVar3 = (int)(local_78 + (local_78 >> 0x1f & 0xffffU)) >> 0x10;
        local_8c = local_8c + iVar3;
        local_60 = local_78 + iVar3 * -0x10000;
      }
      iVar3 = local_74;
      if (local_74 < 1) {
        iVar3 = -local_74;
      }
      if (0xffff < iVar3) {
        iVar3 = (int)(local_74 + (local_74 >> 0x1f & 0xffffU)) >> 0x10;
        local_88 = local_88 + iVar3;
        local_5c = local_74 + iVar3 * -0x10000;
      }
      iVar3 = FUN_004c39d0_FindClearPath(piVar1,&local_90,local_94,&local_24,&local_2c);
      if (iVar3 == 0) {
        param_1[0x47] = -1;
        param_1[9] = 0;
        param_1[8] = 0;
        param_1[7] = 0;
        uVar8 = local_94;
        if (local_88 != local_24) {
          local_88 = local_88 + (local_24 - local_88) / 2;
        }
      }
      else {
        uVar8 = local_94 | 2;
        FUN_004c39d0_FindClearPath(piVar1,piVar1,uVar8,local_4,&local_28);
        if ((local_2c != 0) && (local_28 != 0)) {
          uVar8 = local_94 & 0xfffffffd;
        }
      }
      local_94 = uVar8;
      local_70 = 0;
      if ((local_94 & 2) != 0) {
        piVar2 = (int *)param_1[0x38];
        if ((((piVar2 == (int *)0x0) ||
             ((*piVar2 != 3 && ((piVar2 == (int *)0x0 || (*piVar2 != 0x19)))))) ||
            (piVar2[0x11] == 0)) || (local_2c == 0)) {
          local_20[0] = param_1[0x47];
          local_4c = 0;
          if (local_20[0] == -1) {
            local_20[0] = -0x20;
            local_20[1] = 0x20;
            local_20[2] = 0xffffffc0;
            local_20[3] = 0x40;
            local_30 = 4;
          }
          else {
            local_30 = 1;
          }
          local_80 = local_20;
          do {
            iVar9 = 2;
            iVar3 = *local_80;
            do {
              FUN_0046db20_ConvertToVector(iVar3 + param_1[0x2c] & 0xff,iVar9,local_20 + 4,0);
              local_50 = param_1[6] + local_8;
              local_54 = param_1[5] + local_c;
              local_58 = *piVar1 + local_20[4];
              iVar6 = FUN_004c39d0_FindClearPath(piVar1,&local_58,0,0,0);
              if (iVar6 != 0) break;
              if (local_4c < iVar9) {
                param_1[0x47] = iVar3;
                local_3c = local_58;
                local_38 = local_54;
                local_34 = local_50;
                local_4c = iVar9;
              }
              iVar9 = iVar9 + 2;
            } while (iVar9 < 8);
            local_80 = local_80 + 1;
            local_30 = local_30 + -1;
          } while (local_30 != 0);
          if (local_4c < 1) {
            local_94 = local_94 | 2;
          }
          else {
            local_90 = local_3c;
            local_8c = local_38;
            local_88 = local_34;
            iVar3 = FUN_004c39d0_FindClearPath(piVar1,&local_90,0,0,0);
            if (iVar3 == 0) {
              local_94 = local_94 & 0xfffffffd;
              local_70 = 1;
              local_5c = 0;
              local_64 = 0;
              local_60 = 0;
              local_84 = (int *)local_6c;
              *local_68 = 0;
              local_68[1] = 0;
              local_68[2] = 0;
            }
            else {
              local_90 = *piVar1;
              local_8c = param_1[5];
              local_88 = param_1[6];
            }
          }
        }
        if ((local_94 & 2) != 0) {
          local_8c = param_1[5];
          local_90 = *piVar1;
          local_88 = param_1[6];
          if (param_1[0x95] != 0) {
            param_1[0x97] = 0;
            param_1[0x95] = 0;
            param_1[0x96] = 0;
          }
        }
      }
      *local_68 = local_64;
      local_68[1] = local_60;
      local_68[2] = local_5c;
      iVar3 = FUN_00452e10_GetWalkHeight(piVar1,*(undefined2 *)((int)param_1 + 0xe),0);
      if (local_88 < iVar3) {
        local_88 = FUN_00452e10_GetWalkHeight(piVar1,*(undefined2 *)((int)param_1 + 0xe),0);
      }
      if (((local_90 != *piVar1) || (local_8c != param_1[5])) || (local_88 != param_1[6])) {
        (**(code **)(*param_1 + 8))(&local_90,*(undefined2 *)((int)param_1 + 0xe),0);
      }
      local_94 = local_94 | 1;
    } while (((local_70 == 0) || (param_1[0x3b] == 0)) &&
            (local_84 = (int *)((int)local_84 + 1), (int)local_84 < local_6c));
  }
  return local_94;
}


