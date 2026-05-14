// FUN_004ce350_Walk @ 004ce350 size=2466

undefined4 __thiscall FUN_004ce350_Walk(int *param_1,uint param_2)

{
  int *piVar1;
  bool bVar2;
  bool bVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  undefined *puVar10;
  uint uStack_3c;
  int iStack_38;
  int iStack_34;
  int iStack_30;
  undefined1 auStack_2c [32];
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e7e3;
  pvStack_c = ExceptionList;
  iVar8 = 0;
  bVar3 = false;
  switch(*(undefined4 *)param_1[0x38]) {
  case 1:
  case 2:
    iVar6 = 2;
    break;
  case 3:
  case 4:
    iVar6 = 4;
    break;
  default:
    iVar6 = 0;
    break;
  case 0x19:
  case 0x1a:
    iVar6 = 0x1a;
  }
  ExceptionList = &pvStack_c;
  if (((short)param_1[1] == 0xb) && (ExceptionList = &pvStack_c, (param_1[0xdb] & 2U) != 0)) {
    ExceptionList = &pvStack_c;
    FUN_0051d680_SetPlayerState(param_1[0xdb] & 0xfffffffd);
  }
  if ((param_1[0x44] & 0x80000U) == 0) {
    if ((((param_1 == (int *)0x0) || (piVar1 = (int *)param_1[0x36], piVar1 == (int *)0x0)) ||
        ((*piVar1 != *(int *)param_1[0x38] && ((piVar1 == (int *)0x0 || (*piVar1 != iVar6)))))) ||
       (iVar4 = (**(code **)(*param_1 + 0x1c0))(), iVar4 < 1)) {
      ExceptionList = pvStack_c;
      return 0;
    }
    if ((param_1[0x44] & 0x80000U) == 0) {
      iVar4 = *(int *)(param_1[0x36] + 0x48);
      if ((iVar4 != 0) && ((*(uint *)(iVar4 + 0x24) & 0x2000000) != 0)) {
        ExceptionList = pvStack_c;
        return 0;
      }
      iVar4 = *(int *)(param_1[0x36] + 0x4c);
      if ((iVar4 != 0) && ((*(byte *)(iVar4 + 0x24) & 0x80) != 0)) {
        ExceptionList = pvStack_c;
        return 0;
      }
    }
  }
  if (((DAT_0066829c != 0) && (iVar4 = (**(code **)(*param_1 + 0x178))(), iVar4 < 1)) &&
     (DAT_00676e5c == '\0')) {
    ExceptionList = pvStack_c;
    return 0;
  }
  piVar1 = (int *)param_1[0x38];
  if ((((piVar1 == (int *)0x0) || (iVar4 = *piVar1, iVar4 != 3)) &&
      ((piVar1 == (int *)0x0 || (iVar4 = *piVar1, iVar4 != 0x19)))) ||
     (((iVar4 == 3 &&
       ((iVar4 = FUN_004dab80_TActionBlock_Is(s_combatrun_005e0618), iVar4 != 0 ||
        (iVar4 = FUN_004dab80_TActionBlock_Is(s_handrun_005e0610), iVar4 != 0)))) ||
      ((((int *)param_1[0x38] != (int *)0x0 &&
        ((*(int *)param_1[0x38] == 0x19 && (iVar4 = FUN_004dab80_TActionBlock_Is(s_bowrun_005e0608), iVar4 != 0))))
       || (iVar4 = FUN_004dab80_TActionBlock_Is(&DAT_005e0604), iVar4 != 0)))))) {
    bVar2 = false;
LAB_004ce523:
    iVar4 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),param_2);
    if (iVar4 < 0) {
      iVar4 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),param_2);
      iVar4 = -iVar4;
    }
    else {
      iVar4 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),param_2);
    }
    if (iVar4 < 0x10) goto LAB_004ce560;
  }
  else {
    bVar2 = true;
    if (DAT_005d7a64 == 0) goto LAB_004ce523;
LAB_004ce560:
    FUN_0046db20_ConvertToVector(param_2,4,&iStack_38,0);
    iStack_30 = iStack_30 + param_1[6];
    iStack_34 = iStack_34 + param_1[5];
    iStack_38 = iStack_38 + param_1[4];
    iVar4 = FUN_004c39d0_FindClearPath(param_1 + 4,&iStack_38,0,0,0);
    if (iVar4 != 0) {
      ExceptionList = pvStack_c;
      return 0;
    }
  }
  iVar4 = FUN_004cd690_FindCharacters(&uStack_3c,1,0xffffffff,param_2,0x20,7);
  uVar7 = (iVar4 < 1) - 1 & uStack_3c;
  if (param_1[0x95] != 0) {
    iVar4 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),param_2);
    if (iVar4 < 0) {
      iVar4 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),param_2);
      uStack_3c = -iVar4;
    }
    else {
      uStack_3c = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),param_2);
    }
    if (((int)uStack_3c < 0x20) ||
       (((int *)param_1[0x36] != (int *)0x0 &&
        (((iVar4 = *(int *)param_1[0x36], iVar4 == 2 || (iVar4 == 4)) || (iVar4 == 0x1a)))))) {
      piVar1 = (int *)param_1[0x36];
      if ((piVar1 != (int *)0x0) &&
         (((iVar6 = *piVar1, iVar6 == 2 || (iVar6 == 4)) || (iVar6 == 0x1a)))) {
        piVar1[0xc] = param_2;
        piVar1[0xb] = param_2;
        iVar8 = __ftol();
        piVar1[0xd] = iVar8;
        ExceptionList = pvStack_c;
        return 1;
      }
    }
    else {
      iVar8 = FUN_00482fb0(100);
      uStack_4 = 6;
      if (iVar8 == 0) {
        iVar8 = 0;
      }
      else {
        iVar8 = FUN_004da9f0_TActionBlock_Ctor1(param_1[0x38] + 4,iVar6);
      }
      uVar7 = *(uint *)(iVar8 + 0x60);
      uStack_4 = 0xffffffff;
      *(uint *)(iVar8 + 0x60) = uVar7 | 0x200;
      uVar5 = __ftol();
      *(undefined4 *)(iVar8 + 0x34) = uVar5;
      *(uint *)(iVar8 + 0x2c) = param_2;
      *(uint *)(iVar8 + 0x30) = param_2;
      *(uint *)(iVar8 + 0x60) = uVar7 | 0x620;
      bVar3 = true;
    }
    goto LAB_004cec2d;
  }
  piVar1 = (int *)param_1[0x36];
  if (bVar2) {
    if ((piVar1 != (int *)0x0) &&
       ((((iVar8 = *piVar1, iVar8 == 2 || (iVar8 == 4)) || (iVar8 == 0x1a)) &&
        (piVar1[0xc] == param_2)))) {
      ExceptionList = pvStack_c;
      return 1;
    }
    uVar9 = piVar1[0x11];
    if (((uVar7 != 0) && (uVar7 != uVar9)) && (iVar8 = FUN_004cd990_IsValidTarget(uVar9), iVar8 != 0)) {
      if ((uVar9 != 0) && (iVar8 = (**(code **)(*param_1 + 4))(uVar7), 0x2f < iVar8)) {
        iVar8 = (**(code **)(*param_1 + 4))(uVar7);
        iVar4 = (**(code **)(*param_1 + 4))(uVar9);
        if (iVar4 <= iVar8) goto LAB_004ce8e9;
      }
      FUN_004d4790_SetFighting(uVar7);
      uVar9 = uVar7;
    }
LAB_004ce8e9:
    uStack_3c = param_2;
    if ((((uVar9 != 0) && (iVar8 = FUN_004cd990_IsValidTarget(uVar9), iVar8 != 0)) &&
        ((DAT_005d7a64 != 0 || ((short)param_1[1] == 0xc)))) && (param_1[0x95] == 0)) {
      uStack_3c = FUN_0046ea90_AngleTo(*(undefined4 *)(param_1[0x36] + 0x44));
    }
    uVar7 = uStack_3c + 0xf & 0xe0;
    if ((*(int *)param_1[0x36] != *(int *)param_1[0x38]) && (*(int *)param_1[0x36] != iVar6)) {
      ExceptionList = pvStack_c;
      return 0;
    }
    FUN_004d39f0_GetAngleMoveAnim(param_2,uStack_3c,(int *)param_1[0x38] + 1,auStack_2c,0x20);
    iVar8 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),uVar7);
    if (iVar8 < 0) {
      iVar4 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),uVar7);
      iVar4 = -iVar4;
    }
    else {
      iVar4 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),uVar7);
    }
    if (((short)param_1[1] == 0xb) ||
       ((iVar4 < 0x41 &&
        ((((param_1 != (int *)0x0 && ((int *)param_1[0x36] != (int *)0x0)) &&
          (*(int *)param_1[0x36] == iVar6)) || (iVar4 < 0x21)))))) {
      iVar8 = param_1[0x36];
      if (iVar8 == param_1[0x38]) {
        iVar8 = FUN_00482fb0(100);
        uStack_4 = 5;
        if (iVar8 == 0) {
          iVar8 = 0;
        }
        else {
          iVar8 = FUN_004da9f0_TActionBlock_Ctor1(auStack_2c,iVar6);
        }
        uVar7 = *(uint *)(iVar8 + 0x60) & 0xfffffdff;
        goto LAB_004cea67;
      }
    }
    else {
      iVar8 = FUN_00482fb0(100);
      uStack_4 = 4;
      if (iVar8 == 0) {
        iVar8 = 0;
      }
      else {
        iVar8 = FUN_004da9f0_TActionBlock_Ctor1(param_1[0x38] + 4,iVar6);
      }
      uVar7 = *(uint *)(iVar8 + 0x60) | 0x200;
LAB_004cea67:
      uStack_4 = 0xffffffff;
      *(uint *)(iVar8 + 0x60) = uVar7;
      bVar3 = true;
    }
    if ((iVar8 != 0) && (param_1[0x36] != 0)) {
      *(undefined4 *)(iVar8 + 0x44) = *(undefined4 *)(param_1[0x36] + 0x44);
    }
    uVar7 = iVar4 - 0x20U & ((int)(iVar4 - 0x20U) < 0) - 1;
    *(uint *)(iVar8 + 0x30) = param_2;
    *(int *)(iVar8 + 0x34) = ((int)(uVar7 + ((int)uVar7 >> 0x1f & 0x1fU)) >> 5) * 4 + 8;
    *(uint *)(iVar8 + 0x2c) = uStack_3c;
  }
  else {
    if (((*piVar1 == iVar6) && (piVar1[0xc] == param_2)) && (piVar1[0xb] == param_2)) {
      ExceptionList = pvStack_c;
      return 1;
    }
    iVar8 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),param_2);
    if (iVar8 < 0) {
      iVar4 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),param_2);
      iVar4 = -iVar4;
    }
    else {
      iVar4 = FUN_0046ded0(*(undefined1 *)((int)param_1 + 0x36),param_2);
    }
    piVar1 = (int *)param_1[0x36];
    if (((piVar1 != (int *)0x0) &&
        (((iVar8 = *piVar1, iVar8 == 2 || (iVar8 == 4)) || (iVar8 == 0x1a)))) && (iVar4 < 0x41)) {
      piVar1[0xc] = param_2;
      uVar7 = iVar4 - 0x20U & ((int)(iVar4 - 0x20U) < 0) - 1;
      piVar1[0xb] = param_2;
      piVar1[0xd] = ((int)(uVar7 + ((int)uVar7 >> 0x1f & 0x1fU)) >> 5) * 4 + 8;
      ExceptionList = pvStack_c;
      return 1;
    }
    if (((short)param_1[1] == 0xc) ||
       ((iVar4 < 0x41 &&
        ((((param_1 != (int *)0x0 && (piVar1 != (int *)0x0)) && (*piVar1 == iVar6)) ||
         (iVar4 < 0x21)))))) {
      iVar8 = *param_1;
      uVar5 = FUN_004dadd0_StName(param_1[0x38] + 4,&DAT_005e00d8,0);
      iVar8 = (**(code **)(iVar8 + 0x1f0))(uVar5);
      if (iVar8 == 0) {
        iVar8 = *param_1;
        uVar5 = FUN_004dadd0_StName(param_1[0x38] + 4,&DAT_005e00e0,0);
        iVar8 = (**(code **)(iVar8 + 0x1f0))(uVar5);
        if (iVar8 == 0) {
          iVar8 = *param_1;
          uVar5 = FUN_004dadd0_StName(param_1[0x38] + 4,&DAT_005e00e8,0);
          iVar8 = (**(code **)(iVar8 + 0x1f0))(uVar5);
          if (iVar8 == 0) {
            ExceptionList = pvStack_c;
            return 0;
          }
          uStack_3c = FUN_00482fb0(100);
          uStack_4 = 3;
          if (uStack_3c == 0) {
LAB_004ce843:
            iVar8 = 0;
            goto LAB_004ce84f;
          }
          puVar10 = &DAT_005e00ec;
        }
        else {
          uStack_3c = FUN_00482fb0(100);
          uStack_4 = 2;
          if (uStack_3c == 0) goto LAB_004ce843;
          puVar10 = &DAT_005e00e4;
        }
      }
      else {
        uStack_3c = FUN_00482fb0(100);
        uStack_4 = 1;
        if (uStack_3c == 0) goto LAB_004ce843;
        puVar10 = &DAT_005e00dc;
      }
      uVar5 = FUN_004dadd0_StName(param_1[0x38] + 4,puVar10,iVar6);
      iVar8 = FUN_004da9f0_TActionBlock_Ctor1(uVar5,iVar6);
    }
    else {
      uStack_3c = FUN_00482fb0(100);
      uStack_4 = 0;
      if (uStack_3c == 0) {
        iVar8 = 0;
      }
      else {
        iVar8 = FUN_004da9f0_TActionBlock_Ctor1(param_1[0x38] + 4,iVar6);
      }
      *(uint *)(iVar8 + 0x60) = *(uint *)(iVar8 + 0x60) | 0x200;
      uVar7 = iVar4 - 0x20U & ((int)(iVar4 - 0x20U) < 0) - 1;
      *(int *)(iVar8 + 0x34) = ((int)(uVar7 + ((int)uVar7 >> 0x1f & 0x1fU)) >> 5) * 4 + 8;
    }
LAB_004ce84f:
    uStack_4 = 0xffffffff;
    bVar3 = true;
    uVar5 = *(undefined4 *)(param_1[0x36] + 0x44);
    param_1[0x2c] = (uint)*(byte *)((int)param_1 + 0x36);
    *(undefined4 *)(iVar8 + 0x44) = uVar5;
    *(uint *)(iVar8 + 0x30) = param_2;
    *(uint *)(iVar8 + 0x2c) = param_2;
  }
  iVar6 = *param_1;
  *(uint *)(iVar8 + 0x60) = *(uint *)(iVar8 + 0x60) | 0x420;
  iVar6 = (**(code **)(iVar6 + 0x1f0))(iVar8 + 4,0);
  if (iVar6 == 0) {
    if (((iVar8 != param_1[0x36]) && (iVar8 != param_1[0x37])) &&
       ((iVar8 != param_1[0x38] && (iVar8 != 0)))) {
      if (*(int *)(iVar8 + 0x5c) != 0) {
        FUN_00482f80(*(int *)(iVar8 + 0x5c));
      }
      FUN_004830f0(iVar8);
    }
    ExceptionList = pvStack_c;
    return 0;
  }
LAB_004cec2d:
  iVar6 = (**(code **)(*param_1 + 0x208))(iVar8,0);
  if (((iVar6 == 0) && (bVar3)) && (iVar8 != 0)) {
    if (*(int *)(iVar8 + 0x5c) != 0) {
      FUN_00482f80(*(int *)(iVar8 + 0x5c));
    }
    FUN_004830f0(iVar8);
  }
  iVar8 = param_1[0x38];
  *(uint *)(iVar8 + 0x30) = param_2;
  *(uint *)(iVar8 + 0x2c) = param_2;
  ExceptionList = pvStack_c;
  return 1;
}


