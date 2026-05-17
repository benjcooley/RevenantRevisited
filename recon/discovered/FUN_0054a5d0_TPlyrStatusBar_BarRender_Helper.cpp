// FUN_0054a5d0 @ 0054a5d0 size=1446

void __thiscall
FUN_0054a5d0(int param_1,int param_2,uint param_3,int param_4,undefined4 param_5,int param_6,
            int param_7,int param_8,int param_9,int param_10,undefined4 param_11,int param_12,
            int param_13,int param_14)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int unaff_EDI;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  int local_3c;
  undefined4 local_38;
  uint local_34;
  undefined4 local_30;
  int local_2c;
  uint local_28;
  uint local_24;
  undefined4 local_20;
  int local_1c;
  int local_18;
  int local_14;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  int local_4;
  
  iVar1 = param_13;
  if ((int)param_3 < 1) {
    param_3 = 1;
  }
  if ((int)param_3 < param_2) {
    param_2 = param_3;
  }
  if (param_2 < 0) {
    param_2 = 0;
  }
  if (param_14 < 1) {
    iVar3 = *(int *)(param_1 + 0xdc);
  }
  else {
    iVar3 = *(int *)(param_1 + 0xd4);
  }
  iVar2 = (param_2 * param_12) / (int)param_3;
  iVar4 = param_13 >> 1;
  if (param_14 < 1) {
    iVar5 = param_9;
    if (param_12 - param_13 <= iVar2) {
      iVar5 = param_7;
    }
    param_14 = param_7;
    uVar7 = iVar2 - param_13;
    uVar6 = param_12 + param_13 * -2;
    param_3 = uVar6;
    if ((int)(((int)uVar7 < 0) - 1 & uVar7) < (int)uVar6) {
      param_3 = ((int)uVar7 < 0) - 1 & uVar7;
    }
    uVar6 = uVar6 - param_3;
    param_2 = param_9;
    param_13 = iVar5;
    if (iVar2 < iVar4) {
      param_7 = param_9;
    }
  }
  else {
    iVar5 = param_9;
    if (iVar4 <= iVar2) {
      iVar5 = param_7;
    }
    param_2 = param_7;
    uVar7 = iVar2 - param_13;
    param_3 = param_12 + param_13 * -2;
    uVar6 = param_3;
    if ((int)(((int)uVar7 < 0) - 1 & uVar7) < (int)param_3) {
      uVar6 = ((int)uVar7 < 0) - 1 & uVar7;
    }
    iVar4 = param_12 - param_13;
    param_3 = param_3 - uVar6;
    param_14 = param_9;
    param_13 = iVar5;
    if (iVar2 < iVar4) {
      param_7 = param_9;
    }
  }
  if (DAT_006680c8 == 0) {
    uVar7 = (iVar3 * 0xff) / 6 << 0x18 | 0xffffff;
    FUN_00414d70(param_4,param_5,1,*(undefined4 *)(param_1 + 0x68),0,iVar1,param_11,uVar7,param_6,
                 param_13,iVar1,param_11,0,4);
    if (0 < (int)uVar6) {
      FUN_00414d70(param_4 + iVar1,param_5,1,*(undefined4 *)(param_1 + 0x68),0,uVar6,param_11,uVar7,
                   param_6 + iVar1,param_2,uVar6,param_11,0,4);
    }
    if (0 < (int)param_3) {
      FUN_00414d70(uVar6 + param_4 + iVar1,param_5,1,*(undefined4 *)(param_1 + 0x68),0,param_3,
                   param_11,uVar7,uVar6 + param_6 + iVar1,param_14,param_3,param_11,0,4);
    }
    iVar3 = (param_10 - param_12) + iVar1;
    FUN_00414d70((param_4 - iVar1) + param_12,param_5,1,*(undefined4 *)(param_1 + 0x68),0,iVar3,
                 param_11,uVar7,(param_6 - iVar1) + param_12,param_7,iVar3,param_11,0,4);
    return;
  }
  local_2c = param_4;
  local_1c = param_6;
  local_28 = param_5;
  local_18 = param_13;
  local_54 = 0x100;
  local_50 = 0;
  local_4c = 0;
  local_48 = 0;
  local_24 = iVar1;
  local_20 = param_11;
  local_14 = iVar1;
  local_10 = param_11;
  local_4 = 0;
  local_40 = 0;
  local_44 = 0;
  local_30 = 0;
  local_34 = 0;
  local_38 = 0;
  local_3c = 0;
  local_8 = 0x1f;
  local_c = 0;
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))(&local_54,*(undefined4 *)(param_1 + 0x68),0,0);
  if (0 < (int)uVar6) {
    local_3c = local_4 + iVar1;
    local_28 = local_c;
    local_2c = param_2 + iVar1;
    local_30 = param_11;
    local_20 = param_11;
    local_14 = 0;
    local_50 = 0;
    local_54 = 0;
    local_40 = 0;
    local_44 = 0;
    local_48 = 0;
    local_4c = 0;
    local_18 = 0x1f;
    local_1c = 0;
    local_34 = uVar6;
    local_24 = uVar6;
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
              (&stack0xffffff9c,*(undefined4 *)(unaff_EDI + 0x68),0,0);
  }
  if (0 < local_8) {
    local_3c = local_4 + uVar6 + iVar1;
    local_28 = param_10;
    local_34 = local_8;
    local_24 = local_8;
    local_2c = uVar6 + param_2 + iVar1;
    local_30 = param_11;
    local_20 = param_11;
    local_14 = 0;
    local_50 = 0;
    local_54 = 0;
    local_40 = 0;
    local_44 = 0;
    local_48 = 0;
    local_4c = 0;
    local_18 = 0x1f;
    local_1c = 0;
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
              (&stack0xffffff9c,*(undefined4 *)(unaff_EDI + 0x68),0,0);
  }
  local_3c = (local_4 - iVar1) + param_8;
  local_2c = (param_2 - iVar1) + param_8;
  local_28 = param_3;
  local_34 = (param_6 - param_8) + iVar1;
  local_30 = param_11;
  local_20 = param_11;
  local_14 = 0;
  local_50 = 0;
  local_54 = 0;
  local_40 = 0;
  local_44 = 0;
  local_48 = 0;
  local_4c = 0;
  local_18 = 0x1f;
  local_1c = 0;
  local_24 = local_34;
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x5c))
            (&stack0xffffff9c,*(undefined4 *)(unaff_EDI + 0x68),0,0);
  return;
}


