// FUN_00518aa0_Animate @ 00518aa0 size=842

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00518aa0_Animate(int *param_1)

{
  short sVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 *puVar8;
  int iVar9;
  uint uVar10;
  undefined8 uVar11;
  char *pcVar12;
  undefined1 *puVar13;
  int iStack_50;
  int iStack_4c;
  undefined4 uStack_48;
  undefined1 auStack_44 [68];
  
  FUN_004c1bb0_ResolveAttack();
  if (((param_1[0xdb] & 1U) != 0) && ((param_1[0xdb] & 2U) == 0)) {
    iVar2 = param_1[0xdd] + 1;
    param_1[0xdd] = iVar2;
    uVar11 = __allmul(iVar2,iVar2 >> 0x1f,100,0);
    iVar2 = __alldiv(uVar11,0x18,0);
    param_1[0xdc] = iVar2;
  }
  iVar3 = (**(code **)(*param_1 + 0x314))();
  iVar2 = param_1[0xd5];
  if (iVar2 != -1) {
    iVar9 = iVar3 - iVar2;
    if (iVar9 < 0) {
      iVar9 = iVar2 - iVar3;
    }
    if (50000 < iVar9) {
      param_1[0xd5] = iVar3 + -1;
    }
    if (param_1[0xd5] < iVar3) {
      FUN_0051c660_TPlayer_RefreshStats();
    }
  }
  (**(code **)(*param_1 + 0x40))(param_1[2] & 0xfbffffff);
  uVar10 = 0;
  if (DAT_00668154 == 0) {
    if ((param_1[0x44] & 0x1000U) != 0) {
      iVar2 = param_1[0xe7];
      if (iVar2 < 2) {
        iVar2 = 1;
      }
      param_1[0xe7] = iVar2;
    }
    iVar2 = (**(code **)(*param_1 + 0x1c0))();
    if (iVar2 < 1) {
      iVar2 = param_1[0xe7];
      if (iVar2 == 0) {
        if (DAT_0066829c != 0) {
          FUN_0047e3c0();
          param_1[0xe7] = param_1[0xe7] + -1;
          goto LAB_00518bcc;
        }
        DAT_0065abec = 0;
        _DAT_0065cb08 = &DAT_0066f680;
        FUN_0048ea40();
        iVar2 = param_1[0xe7];
      }
      param_1[0xe7] = iVar2 + -1;
    }
    else {
      param_1[0xe7] = 0xc0;
    }
  }
LAB_00518bcc:
  iVar2 = (**(code **)(*param_1 + 0x250))();
  if ((iVar2 == 0) || (param_1[0x16] == 0)) {
    if (*(short *)((int)param_1 + 0x8a) < 1) {
      FUN_004713c0(0);
    }
    else {
      uVar10 = (int)*(short *)((int)param_1 + 0x8a) - 0x19;
      FUN_004713c0(1);
      FUN_004715e0(((int)uVar10 < 0) - 1 & uVar10);
    }
  }
  else {
    uVar4 = (**(code **)(*(int *)param_1[0xae] + 0x1f8))();
    uVar5 = (**(code **)(*(int *)param_1[0xae] + 0x200))();
    uVar6 = (**(code **)(*(int *)param_1[0xae] + 0x208))();
    puVar13 = auStack_44;
    pcVar12 = s_lhand_005e2718;
    (**(code **)(*param_1 + 0x24))(s_lhand_005e2718,puVar13);
    uVar7 = FUN_0040edc0(pcVar12);
    (**(code **)(*param_1 + 0x24))(uVar7);
    FUN_0040ef80(uVar7,puVar13);
    iVar2 = FUN_00483300_RandomRange(0xfffffffb,5);
    iStack_50 = __ftol();
    iStack_50 = iVar2 + iStack_50;
    iVar2 = FUN_00483300_RandomRange(0xfffffffb,5);
    iVar3 = __ftol();
    iStack_4c = iVar2 + iVar3;
    uStack_48 = __ftol();
    if (DAT_006671f0 == 0) {
      iStack_50 = iStack_50 + 0x28;
      iStack_4c = iVar2 + iVar3 + 0x28;
    }
    sVar1 = *(short *)((int)param_1 + 0x8a);
    iVar2 = (**(code **)(*(int *)param_1[0xae] + 0x1f0))();
    if (sVar1 < iVar2) {
      iVar2 = iVar2 - sVar1;
      uVar10 = (int)(iVar2 + (iVar2 >> 0x1f & 3U)) >> 2;
    }
    else {
      iVar2 = FUN_00483300_RandomRange(0,2);
      if (iVar2 == 0) {
        iVar2 = FUN_00483300_RandomRange(8,0x1e);
        uVar10 = -iVar2;
      }
    }
    uVar10 = ((int)uVar10 < 0) - 1 & uVar10;
    if (99 < (int)uVar10) {
      uVar10 = 100;
    }
    FUN_004713c0(1);
    uVar7 = (**(code **)(*(int *)param_1[0xae] + 0x1f0))();
    FUN_004714e0(uVar7);
    FUN_004715e0(uVar10);
    puVar8 = (undefined4 *)FUN_00429950(uVar4,uVar5,uVar6);
    FUN_00471820(*puVar8);
    FUN_004716c0(&iStack_50);
  }
  iVar2 = FUN_0047e920_FrameTick();
  if (param_1[0xe6] < iVar2) {
    iVar2 = FUN_0047e920_FrameTick();
    param_1[0xe6] = iVar2 + 0x78;
    if (param_1 == DAT_00667fcc) {
      FUN_00585fb0(param_1);
    }
    if (DAT_0067682c != 0) {
      FUN_00583e30(param_1);
    }
  }
  return;
}


