// FUN_004998b0_TSector_LoadPreloadSectors @ 004998b0 size=802

undefined4 FUN_004998b0_TSector_LoadPreloadSectors(int param_1,int param_2,int param_3,code *param_4)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int *piVar9;
  int iVar10;
  int *local_34;
  int local_28;
  int local_20;
  int local_1c;
  int local_14;
  int local_8;
  
  iVar8 = param_3;
  FUN_00499be0_TSector_ClearPreloadSectors(param_1,param_2,param_3,0);
  DAT_005da754 = param_1;
  if (0x20 < param_2) {
    param_2 = 0x20;
  }
  iVar6 = 0;
  DAT_006687a8 = param_2;
  param_3 = 0;
  if (param_2 < 1) {
    return 1;
  }
  piVar9 = (int *)(iVar8 + 8);
  local_34 = &DAT_00668590;
  local_28 = param_2;
  do {
    iVar8 = *piVar9;
    local_20 = piVar9[-2];
    *local_34 = piVar9[-2];
    local_34[1] = piVar9[-1];
    local_34[2] = *piVar9;
    local_1c = piVar9[-1];
    local_34[3] = piVar9[1];
    local_20 = local_20 >> 10;
    local_1c = local_1c >> 10;
    local_8 = iVar8 + 0x3ff >> 10;
    local_14 = piVar9[1] + 0x3ff >> 10;
    if (local_20 < 0x1f) {
      if (local_20 < 0) {
        local_20 = 0;
      }
    }
    else {
      local_20 = 0x1f;
    }
    if (local_1c < 0x1f) {
      if (local_1c < 0) {
        local_1c = 0;
      }
    }
    else {
      local_1c = 0x1f;
    }
    if (local_8 < 0x1f) {
      if (local_8 < 0) {
        local_8 = 0;
      }
    }
    else {
      local_8 = 0x1f;
    }
    if (local_14 < 0x1f) {
      if (local_14 < 0) {
        local_14 = 0;
      }
    }
    else {
      local_14 = 0x1f;
    }
    iVar8 = (local_8 - local_20) + 1;
    iVar3 = ((local_14 - local_1c) + 1) * iVar8 * 2;
    for (iVar5 = local_1c; iVar5 <= local_14; iVar5 = iVar5 + 1) {
      if (local_20 <= local_8) {
        iVar7 = iVar6 * 1000;
        iVar6 = param_3 + iVar8;
        iVar10 = local_20;
        do {
          iVar4 = 0;
          piVar2 = DAT_006687a0;
          if (0 < DAT_00668790) {
            do {
              iVar1 = *piVar2;
              if (((*(int *)(iVar1 + 4) == param_1) && (*(int *)(iVar1 + 8) == iVar10)) &&
                 (*(int *)(iVar1 + 0xc) == iVar5)) goto LAB_00499a60;
              iVar4 = iVar4 + 1;
              piVar2 = piVar2 + 1;
            } while (iVar4 < DAT_00668790);
          }
          iVar4 = FUN_004982b0_TSector_LoadSector(param_1,iVar10,iVar5,2);
          if (iVar4 != 0) {
            FUN_0041c840(iVar4);
            *(undefined4 *)(iVar4 + 0x10) = 1;
          }
LAB_00499a60:
          if (param_4 != (code *)0x0) {
            (*param_4)(iVar7 / iVar3);
          }
          iVar7 = iVar7 + 1000;
          iVar10 = iVar10 + 1;
          param_3 = iVar6;
        } while (iVar10 <= local_8);
      }
    }
    if (local_20 < local_8) {
      do {
        if (local_14 <= local_1c) break;
        iVar8 = 0;
        if (0 < DAT_00668790) {
          iVar5 = iVar6 * 1000;
          do {
            iVar6 = DAT_006687a0[iVar8];
            if (((*(int *)(iVar6 + 8) == local_20) || (*(int *)(iVar6 + 8) == local_8)) ||
               ((*(int *)(iVar6 + 0xc) == local_1c || (*(int *)(iVar6 + 0xc) == local_14)))) {
              iVar10 = 0;
              if (0 < *(int *)(iVar6 + 0xb8)) {
                do {
                  piVar2 = *(int **)(*(int *)(iVar6 + 200) + iVar10 * 4);
                  if (piVar2 != (int *)0x0) {
                    (**(code **)(*piVar2 + 0x1c))();
                  }
                  iVar10 = iVar10 + 1;
                } while (iVar10 < *(int *)(iVar6 + 0xb8));
              }
              if (param_4 != (code *)0x0) {
                (*param_4)(iVar5 / iVar3);
              }
              param_3 = param_3 + 1;
              iVar5 = iVar5 + 1000;
            }
            iVar8 = iVar8 + 1;
            iVar6 = param_3;
          } while (iVar8 < DAT_00668790);
        }
        local_20 = local_20 + 1;
        local_8 = local_8 + -1;
        local_1c = local_1c + 1;
        local_14 = local_14 + -1;
      } while (local_20 < local_8);
    }
    local_34 = local_34 + 4;
    piVar9 = piVar9 + 4;
    local_28 = local_28 + -1;
    if (local_28 == 0) {
      return 1;
    }
  } while( true );
}


