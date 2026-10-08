// FUN_0051c660 @ 0051c660 size=1504

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0051c660(int *param_1)

{
  char cVar1;
  uint *puVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  int *piVar7;
  char *pcVar8;
  char *pcVar9;
  int local_114;
  undefined4 local_110;
  int local_10c;
  int local_108;
  undefined ***local_104;
  undefined4 local_100;
  undefined4 local_fc;
  undefined4 local_f8;
  undefined4 local_f0;
  undefined4 local_ec;
  undefined1 *local_e8;
  undefined1 local_e4;
  undefined4 local_e0;
  undefined4 local_d0;
  int local_cc;
  int local_c8;
  undefined ***local_c4;
  undefined4 local_c0;
  undefined4 local_bc;
  undefined4 local_b8;
  undefined4 local_b0;
  undefined4 local_ac;
  undefined1 *local_a8;
  undefined1 local_a4;
  undefined4 local_a0;
  undefined4 local_90;
  int local_8c;
  int local_88;
  undefined ***local_84;
  undefined4 local_80;
  undefined4 local_7c;
  undefined4 local_78;
  undefined4 local_70;
  undefined4 local_6c;
  undefined1 *local_68;
  undefined1 local_64;
  undefined4 local_60;
  undefined **local_50;
  char *local_4c;
  char *local_48;
  char *local_44;
  char *local_40;
  undefined **local_3c;
  char *local_38;
  int local_34;
  int local_30;
  int local_2c;
  undefined **local_28;
  char *local_24;
  char *local_20;
  char *local_1c;
  char *local_18;
  void *pvStack_14;
  undefined1 *puStack_10;
  undefined4 local_c;
  
  local_c = 0xffffffff;
  puStack_10 = &LAB_005a1226;
  pvStack_14 = ExceptionList;
  iVar5 = (int)(short)param_1[0xd3];
  ExceptionList = &pvStack_14;
  if (0 < iVar5) {
    iVar3 = param_1[0x2a];
    puVar2 = (uint *)param_1[0xd4];
    ExceptionList = &pvStack_14;
    do {
      if (*puVar2 < (uint)(int)(short)iVar3) {
        uVar6 = *(uint *)(param_1[0x2b] + *puVar2 * 4);
      }
      else {
        uVar6 = 0;
      }
      puVar2[1] = uVar6;
      puVar2 = puVar2 + 2;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
  }
  piVar7 = param_1 + 0xa8;
  local_114 = 0xb;
  do {
    iVar5 = *piVar7;
    if ((iVar5 != 0) &&
       (iVar5 = FUN_0048cb50((int)*(short *)(iVar5 + 4),(int)*(short *)(iVar5 + 6)), iVar5 != 0)) {
      if (*(short *)(*piVar7 + 4) == 1) {
        pcVar9 = *(char **)(iVar5 + 0xcc);
        if (pcVar9 != (char *)0x0) {
          uVar6 = 0xffffffff;
          pcVar8 = pcVar9;
          do {
            if (uVar6 == 0) break;
            uVar6 = uVar6 - 1;
            cVar1 = *pcVar8;
            pcVar8 = pcVar8 + 1;
          } while (cVar1 != '\0');
          local_28 = &PTR_LAB_005a36f8;
          local_1c = pcVar9 + (~uVar6 - 1);
          local_24 = s_String_005e2a34;
          local_20 = pcVar9;
          local_18 = pcVar9;
          FUN_00478720();
          local_c4 = &local_28;
          local_c = 0;
          local_d0 = 0;
          local_cc = 0;
          local_c8 = 0;
          local_c0 = 0;
          local_bc = 0;
          local_b8 = 0;
          local_b0 = 0;
          local_ac = 0;
          local_a4 = 0;
          local_a0 = 1;
          local_a8 = (undefined1 *)FUN_00482fb0(0x2000);
          *local_a8 = 0;
          local_c = 1;
          FUN_0051cc40(&local_d0,0xffffffff);
          local_c = 2;
          FUN_004830f0(local_a8);
          if (local_cc == 0) {
            if (local_c8 != 0) {
              FUN_004830f0(local_c4);
              FUN_004a1540(local_c8);
            }
          }
          else {
            FUN_004830f0(local_c4);
            FUN_004830f0(local_cc);
          }
LAB_0051c9b6:
          local_c = 0xffffffff;
          FUN_00478730();
        }
      }
      else if ((*(short *)(*piVar7 + 4) == 2) &&
              (pcVar9 = *(char **)(iVar5 + 200), pcVar9 != (char *)0x0)) {
        uVar6 = 0xffffffff;
        pcVar8 = pcVar9;
        do {
          if (uVar6 == 0) break;
          uVar6 = uVar6 - 1;
          cVar1 = *pcVar8;
          pcVar8 = pcVar8 + 1;
        } while (cVar1 != '\0');
        local_50 = &PTR_LAB_005a36f8;
        local_44 = pcVar9 + (~uVar6 - 1);
        local_4c = s_String_005e2a3c;
        local_48 = pcVar9;
        local_40 = pcVar9;
        FUN_00478720();
        local_84 = &local_50;
        local_c = 3;
        local_90 = 0;
        local_8c = 0;
        local_88 = 0;
        local_80 = 0;
        local_7c = 0;
        local_78 = 0;
        local_70 = 0;
        local_6c = 0;
        local_64 = 0;
        local_60 = 1;
        local_68 = (undefined1 *)FUN_00482fb0(0x2000);
        *local_68 = 0;
        local_c = 4;
        FUN_0051cc40(&local_90,0xffffffff);
        local_c = 5;
        FUN_004830f0(local_68);
        if (local_8c == 0) {
          if (local_88 != 0) {
            FUN_004830f0(local_84);
            FUN_004a1540(local_88);
          }
        }
        else {
          FUN_004830f0(local_84);
          FUN_004830f0(local_8c);
        }
        goto LAB_0051c9b6;
      }
    }
    piVar7 = piVar7 + 1;
    local_114 = local_114 + -1;
    if (local_114 == 0) {
      iVar5 = 0;
      if (0 < (short)param_1[0xd6]) {
        do {
          uVar6 = 0xffffffff;
          pcVar9 = *(char **)(param_1[0xd7] + iVar5 * 8);
          do {
            if (uVar6 == 0) break;
            uVar6 = uVar6 - 1;
            cVar1 = *pcVar9;
            pcVar9 = pcVar9 + 1;
          } while (cVar1 != '\0');
          local_34 = *(int *)(param_1[0xd7] + iVar5 * 8);
          local_3c = &PTR_LAB_005a36f8;
          local_30 = (~uVar6 - 1) + local_34;
          local_38 = s_String_005e2a44;
          local_2c = local_34;
          FUN_00478720();
          local_104 = &local_3c;
          local_c = 6;
          local_110 = 0;
          local_10c = 0;
          local_108 = 0;
          local_100 = 0;
          local_fc = 0;
          local_f8 = 0;
          local_f0 = 0;
          local_ec = 0;
          local_e4 = 0;
          local_e0 = 1;
          local_e8 = (undefined1 *)FUN_00482fb0(0x2000);
          *local_e8 = 0;
          local_c = 7;
          FUN_0051cc40(&local_110,iVar5);
          local_c = 8;
          FUN_004830f0(local_e8);
          if (local_10c == 0) {
            if (local_108 != 0) {
              FUN_004830f0(local_104);
              FUN_004a1540(local_108);
            }
          }
          else {
            FUN_004830f0(local_104);
            FUN_004830f0(local_10c);
          }
          local_c = 0xffffffff;
          FUN_00478730();
          iVar5 = iVar5 + 1;
        } while (iVar5 < (short)param_1[0xd6]);
      }
      iVar5 = (**(code **)(*param_1 + 0x1d8))();
      iVar3 = (**(code **)(*param_1 + 0x1c0))();
      if (iVar5 < iVar3) {
        iVar5 = *param_1;
        uVar4 = (**(code **)(iVar5 + 0x1d8))();
        (**(code **)(iVar5 + 0x1c4))(uVar4);
      }
      iVar5 = (**(code **)(*param_1 + 0x1e0))();
      iVar3 = (**(code **)(*param_1 + 0x1c8))();
      if (iVar5 < iVar3) {
        iVar5 = *param_1;
        uVar4 = (**(code **)(iVar5 + 0x1e0))();
        (**(code **)(iVar5 + 0x1cc))(uVar4);
      }
      iVar5 = (**(code **)(*param_1 + 0x1e8))();
      iVar3 = (**(code **)(*param_1 + 0x1d0))();
      if (iVar5 < iVar3) {
        iVar5 = *param_1;
        uVar4 = (**(code **)(iVar5 + 0x1e8))();
        (**(code **)(iVar5 + 0x1d4))(uVar4);
      }
      iVar5 = 0x22;
      do {
        iVar3 = (**(code **)(*param_1 + 0xdc))(iVar5);
        if ((0x1e < iVar3) && (iVar5 < (short)param_1[0xd3])) {
          *(undefined4 *)(param_1[0xd4] + 4 + iVar5 * 8) = 0x1e;
        }
        iVar5 = iVar5 + 1;
      } while (iVar5 < 0x28);
      iVar5 = 0x28;
      do {
        iVar3 = (**(code **)(*param_1 + 0xdc))(iVar5);
        if ((0x1e < iVar3) && (iVar5 < (short)param_1[0xd3])) {
          *(undefined4 *)(param_1[0xd4] + 4 + iVar5 * 8) = 0x1e;
        }
        iVar5 = iVar5 + 1;
      } while (iVar5 < 0x33);
      if ((param_1 == DAT_00667fcc) && (DAT_0065b2d8 == DAT_00667fcc)) {
        _DAT_0065b190 = 1;
      }
      FUN_00519230();
      ExceptionList = pvStack_14;
      return;
    }
  } while( true );
}


