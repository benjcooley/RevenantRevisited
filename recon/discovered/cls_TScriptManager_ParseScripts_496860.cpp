// FUN_00496860_TScriptManager_ParseScripts @ 00496860 size=1466

void __thiscall FUN_00496860_TScriptManager_ParseScripts(int param_1,char *param_2,undefined4 param_3,undefined4 param_4)

{
  uint *puVar1;
  char cVar2;
  byte bVar3;
  int *piVar4;
  undefined4 uVar5;
  int iVar6;
  byte *pbVar7;
  uint uVar8;
  int iVar9;
  byte *pbVar10;
  char *pcVar11;
  bool bVar12;
  undefined **local_60;
  char *local_5c;
  char *local_58;
  char *local_54;
  char *local_50;
  undefined4 local_4c;
  int local_48;
  int local_44;
  undefined ***local_40;
  int local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_2c;
  undefined4 local_28;
  undefined1 *local_24;
  undefined1 local_20;
  undefined4 local_1c;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059da77;
  local_c = ExceptionList;
  uVar8 = 0xffffffff;
  pcVar11 = param_2;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar2 = *pcVar11;
    pcVar11 = pcVar11 + 1;
  } while (cVar2 != '\0');
  local_60 = &PTR_LAB_005a36f8;
  local_54 = param_2 + (~uVar8 - 1);
  local_58 = param_2;
  local_50 = param_2;
  local_5c = s_String_005da638;
  ExceptionList = &local_c;
  FUN_00478720();
  local_40 = &local_60;
  local_4 = 0;
  local_4c = 0;
  local_48 = 0;
  local_44 = 0;
  local_3c = 0;
  local_38 = 0;
  local_34 = 0;
  local_2c = 0;
  local_28 = 0;
  local_20 = 0;
  local_1c = 1;
  local_24 = (undefined1 *)FUN_00482fb0(0x2000);
  *local_24 = 0;
  uVar5 = DAT_0066856c;
  local_4 = 1;
  DAT_0066856c = 1;
  FUN_00478a10();
  iVar9 = 0;
  if (0 < *(int *)(param_1 + 0x8004)) {
    do {
      iVar6 = FUN_00410160(iVar9);
      if ((iVar6 != 0) && (iVar6 = FUN_00495c20(param_3), iVar6 != 0)) {
        puVar1 = (uint *)(*(int *)(*(int *)(param_1 + 0x8014) + iVar9 * 4) + 0x48);
        *puVar1 = *puVar1 | 1;
      }
      iVar9 = iVar9 + 1;
    } while (iVar9 < *(int *)(param_1 + 0x8004));
  }
  uVar8 = 0;
  if (0 < *(int *)(param_1 + 0x8018)) {
    do {
      if ((((*(int *)(param_1 + 0x8028) != 0) && (uVar8 < *(uint *)(param_1 + 0x8018))) &&
          (*(int *)(*(int *)(param_1 + 0x8028) + uVar8 * 4) != 0)) &&
         (iVar9 = FUN_004943f0(1), iVar9 != 0)) {
        FUN_00492490();
      }
      uVar8 = uVar8 + 1;
    } while ((int)uVar8 < *(int *)(param_1 + 0x8018));
  }
joined_r0x004969cb:
  if (local_3c == 10) {
    uVar8 = 0;
    if (0 < *(int *)(param_1 + 0x8018)) {
      do {
        if (((*(int *)(param_1 + 0x8028) != 0) && (uVar8 < *(uint *)(param_1 + 0x8018))) &&
           (*(int *)(*(int *)(param_1 + 0x8028) + uVar8 * 4) != 0)) {
          iVar9 = *(int *)(*(int *)(param_1 + 0x8028) + uVar8 * 4);
          for (iVar6 = *(int *)(iVar9 + 4); iVar6 != 0; iVar6 = *(int *)(iVar6 + 8)) {
            if ((*(byte *)(iVar6 + 0x48) & 2) != 0) {
              if (*(int *)(iVar9 + 0xc) != 0) {
                *(undefined4 *)(*(int *)(iVar9 + 0xc) + 0x84) = 0;
                FUN_00471150(iVar9);
              }
              break;
            }
          }
        }
        uVar8 = uVar8 + 1;
      } while ((int)uVar8 < *(int *)(param_1 + 0x8018));
    }
    uVar8 = 0;
    if (0 < *(int *)(param_1 + 0x8004)) {
      do {
        if (((*(int *)(param_1 + 0x8014) != 0) && (uVar8 < *(uint *)(param_1 + 0x8004))) &&
           ((*(int *)(*(int *)(param_1 + 0x8014) + uVar8 * 4) != 0 &&
            ((*(byte *)(*(int *)(*(int *)(param_1 + 0x8014) + uVar8 * 4) + 0x48) & 1) != 0)))) {
          FUN_00496f20(uVar8,0);
        }
        uVar8 = uVar8 + 1;
      } while ((int)uVar8 < *(int *)(param_1 + 0x8004));
    }
    local_4 = 4;
    DAT_0066856c = uVar5;
    FUN_004830f0(local_24);
    if (local_48 == 0) {
joined_r0x00496ddb:
      if (local_44 != 0) {
        FUN_004830f0(local_40);
        FUN_004a1540(local_44);
      }
    }
    else {
      FUN_004830f0(local_40);
      FUN_004830f0(local_48);
    }
  }
  else {
    iVar9 = FUN_00482fb0(0x4c);
    local_4._0_1_ = 2;
    if (iVar9 == 0) {
      param_2 = (char *)0x0;
    }
    else {
      param_2 = (char *)FUN_004946f0_TScriptProto_Ctor(0,param_4,param_3,0);
    }
    local_4 = CONCAT31(local_4._1_3_,1);
    iVar9 = FUN_00494e20_TScriptProto_ParseScript(&local_4c);
    if (iVar9 != -1) {
      uVar8 = 0;
      if (0 < *(int *)(param_1 + 0x8004)) {
        do {
          if ((((*(int *)(param_1 + 0x8014) != 0) && (uVar8 < *(uint *)(param_1 + 0x8004))) &&
              (*(int *)(*(int *)(param_1 + 0x8014) + uVar8 * 4) != 0)) &&
             ((pbVar7 = (byte *)**(undefined4 **)(*(int *)(param_1 + 0x8014) + uVar8 * 4),
              pbVar7 != (byte *)0x0 && (pbVar10 = *(byte **)param_2, pbVar10 != (byte *)0x0)))) {
            do {
              bVar3 = *pbVar7;
              bVar12 = bVar3 < *pbVar10;
              if (bVar3 != *pbVar10) {
LAB_00496a9c:
                iVar9 = (1 - (uint)bVar12) - (uint)(bVar12 != 0);
                goto LAB_00496aa1;
              }
              if (bVar3 == 0) break;
              bVar3 = pbVar7[1];
              bVar12 = bVar3 < pbVar10[1];
              if (bVar3 != pbVar10[1]) goto LAB_00496a9c;
              pbVar7 = pbVar7 + 2;
              pbVar10 = pbVar10 + 2;
            } while (bVar3 != 0);
            iVar9 = 0;
LAB_00496aa1:
            if (iVar9 == 0) {
              piVar4 = *(int **)(*(int *)(param_1 + 0x8014) + uVar8 * 4);
              if (*piVar4 == 0) goto LAB_00496baf;
              if (param_2 == (char *)0x0) goto LAB_00496bc3;
              pbVar7 = *(byte **)((int)param_2 + 4);
              pbVar10 = (byte *)piVar4[1];
              goto LAB_00496add;
            }
          }
          uVar8 = uVar8 + 1;
        } while ((int)uVar8 < *(int *)(param_1 + 0x8004));
      }
      goto LAB_00496be3;
    }
    if (param_2 != (char *)0x0) {
      FUN_004948e0_TScriptProto_Dtor();
      FUN_004830f0(param_2);
    }
    local_4 = 3;
    FUN_004830f0(local_24);
    if (local_48 == 0) goto joined_r0x00496ddb;
    FUN_004830f0(local_40);
    FUN_004830f0(local_48);
  }
  local_4 = 0xffffffff;
  FUN_00478730();
  ExceptionList = local_c;
  return;
  while( true ) {
    bVar3 = pbVar10[1];
    bVar12 = bVar3 < pbVar7[1];
    if (bVar3 != pbVar7[1]) goto LAB_00496b01;
    pbVar10 = pbVar10 + 2;
    pbVar7 = pbVar7 + 2;
    if (bVar3 == 0) break;
LAB_00496add:
    bVar3 = *pbVar10;
    bVar12 = bVar3 < *pbVar7;
    if (bVar3 != *pbVar7) {
LAB_00496b01:
      iVar9 = (1 - (uint)bVar12) - (uint)(bVar12 != 0);
      goto LAB_00496b06;
    }
    if (bVar3 == 0) break;
  }
  iVar9 = 0;
LAB_00496b06:
  if (iVar9 == 0) {
LAB_00496baf:
    if (param_2 != (char *)0x0) {
      FUN_004948e0_TScriptProto_Dtor();
      FUN_004830f0(param_2);
    }
LAB_00496bc3:
    param_2 = (char *)0x0;
    puVar1 = (uint *)(*(int *)(*(int *)(param_1 + 0x8014) + uVar8 * 4) + 0x48);
    *puVar1 = *puVar1 & 0xfffffffe;
  }
  else {
    FUN_00496f20(uVar8,param_2);
    FUN_0041cb40(uVar8);
    if (piVar4 != (int *)0x0) {
      FUN_004948e0_TScriptProto_Dtor();
      FUN_004830f0(piVar4);
    }
    FUN_0041c910(param_2,uVar8);
    puVar1 = (uint *)(*(int *)(*(int *)(param_1 + 0x8014) + uVar8 * 4) + 0x48);
    *puVar1 = *puVar1 | 2;
    FUN_0045a680(8,param_2);
    if (DAT_00668154 == 0) {
      FUN_0054d170(&DAT_0065c5d0,s_Updated___s__script_005da658,
                   **(undefined4 **)(*(int *)(param_1 + 0x8014) + uVar8 * 4));
    }
    else {
      FUN_0041ee50(s_Updated___s__script_005da640,
                   **(undefined4 **)(*(int *)(param_1 + 0x8014) + uVar8 * 4));
    }
  }
LAB_00496be3:
  iVar9 = *(int *)(param_1 + 0x8004);
  if (iVar9 <= (int)uVar8) {
    uVar8 = 0;
    if (0 < iVar9) {
      do {
        if (((*(int *)(param_1 + 0x8014) == 0) || (*(uint *)(param_1 + 0x8004) <= uVar8)) ||
           (*(int *)(*(int *)(param_1 + 0x8014) + uVar8 * 4) == 0)) {
          FUN_0045a680(5,*(undefined4 *)(*(int *)(param_1 + 0x8014) + uVar8 * 4));
          FUN_0041c910(param_2,uVar8);
          break;
        }
        uVar8 = uVar8 + 1;
      } while ((int)uVar8 < iVar9);
    }
    if ((int)*(uint *)(param_1 + 0x8004) <= (int)uVar8) {
      FUN_0041c840(param_2);
    }
    FUN_0045a680(8,param_2);
  }
  goto joined_r0x004969cb;
}


