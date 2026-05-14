// FUN_004c8b60_AI @ 004c8b60 size=2291

void __fastcall FUN_004c8b60_AI(int *param_1)

{
  undefined2 uVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  int iVar6;
  int *piVar7;
  uint uVar8;
  int iVar9;
  int *piVar10;
  undefined4 *puVar11;
  uint *puVar12;
  int iStack_74;
  int iStack_70;
  uint uStack_6c;
  int iStack_68;
  int iStack_64;
  int iStack_60;
  undefined4 uStack_5c;
  undefined4 uStack_58;
  undefined4 uStack_54;
  uint auStack_50 [10];
  undefined4 auStack_28 [10];
  
  iVar3 = (**(code **)(*param_1 + 0x178))();
  if (iVar3 < 1) {
    return;
  }
  if (((short)param_1[1] == 0xb) && ((param_1[0x44] & 0x100000U) == 0)) {
    return;
  }
  if ((((*(byte *)(param_1 + 2) & 0x40) != 0) ||
      (iVar3 = (**(code **)(*param_1 + 0x1c0))(), iVar3 < 1)) || (DAT_00668110 != 0)) {
    if ((int *)param_1[0x36] == (int *)0x0) {
      return;
    }
    iVar3 = *(int *)param_1[0x36];
    if (((iVar3 != 2) && (iVar3 != 4)) && (iVar3 != 0x1a)) {
      return;
    }
    FUN_004cee70_Stop(0);
    return;
  }
  if (DAT_00668154 != 0) {
    return;
  }
  iVar3 = (**(code **)(*param_1 + 0x1a8))();
  if (iVar3 != 0) {
    FUN_004c9b70_AI_PerMonster();
  }
  piVar10 = (int *)param_1[0x38];
  if ((piVar10 == (int *)0x0) ||
     ((*piVar10 != 3 && ((piVar10 == (int *)0x0 || (*piVar10 != 0x19)))))) {
    iVar3 = 0;
  }
  else {
    iVar3 = piVar10[0x11];
  }
  iVar3 = FUN_004cd990_IsValidTarget(iVar3);
  if (((iVar3 == 0) || (piVar10 = (int *)param_1[0x38], piVar10 == (int *)0x0)) ||
     (((*piVar10 != 3 && ((piVar10 == (int *)0x0 || (*piVar10 != 0x19)))) ||
      (piVar10 = (int *)piVar10[0x11], piVar10 == (int *)0x0)))) {
    if ((((DAT_00668154 != 0) || (iVar3 = (**(code **)(*param_1 + 0x1a8))(), iVar3 == 0)) ||
        ((*(byte *)(param_1 + 0x44) & 4) != 0)) ||
       (uVar8 = param_1[0x10], uVar4 = FUN_0047e920_FrameTick(), ((uVar4 ^ uVar8) & 0x1f) != 0))
    goto LAB_004c93ce;
    iVar3 = FUN_004cd690_FindCharacters(&uStack_6c,1,0xffffffff,0xffffffff,0x20,7);
    piVar10 = (int *)((iVar3 < 1) - 1 & uStack_6c);
    if (piVar10 == (int *)0x0) goto LAB_004c93ce;
    if (param_1[0x95] == 0) {
      FUN_004d3b90_BeginFighting(piVar10,3);
    }
  }
  if (param_1[0x36] == 0) goto LAB_004c93ce;
  if (((param_1 == (int *)0x0) || (iVar3 = FUN_004dab80_TActionBlock_Is(&DAT_005df9f4), iVar3 == 0)) ||
     ((param_1[0x95] != 0 || ((param_1[0x44] & 0x4000U) == 0)))) {
    iVar3 = *(int *)param_1[0x36];
    if (((iVar3 != 3) || (param_1[0x95] != 0)) &&
       (((param_1[0x4a] & 1U) == 0 || ((param_1[0x4a] & 6U) == 0)))) {
      if ((param_1[0x95] != 0) && ((*(byte *)(param_1 + 0x44) & 4) == 0)) {
        if (param_1[0x8d] != 0) {
          param_1[0x8d] = 0;
        }
        cVar2 = FUN_0046ea90_AngleTo(piVar10);
        iVar3 = FUN_004ce350_Walk(cVar2 + '\x7f');
        if (iVar3 != 0) {
          FUN_00583e80_LogActionEvent(param_1,0x1c,cVar2 + '\x7f',1,0);
        }
        goto LAB_004c93ce;
      }
      if (((int *)param_1[0x36] == (int *)0x0) ||
         (((iVar3 != 2 && (iVar3 != 4)) && (iVar3 != 0x1a)))) {
        if ((((param_1[10] == 0) && (param_1[0xb] == 0)) &&
            (iVar3 = FUN_004c39d0_FindClearPath(param_1 + 4,param_1 + 4,8,0,&uStack_6c), iVar3 != 0)) &&
           (uStack_6c != 0)) {
          iVar3 = FUN_0046ead0_AngleDiffFromFacing(uStack_6c);
          cVar2 = ((-1 < iVar3) - 1U & 0xfa) + 0x72;
          FUN_004d6220_SideStep(cVar2);
          piVar10 = (int *)param_1[0x38];
          if ((piVar10 == (int *)0x0) ||
             ((*piVar10 != 3 && ((piVar10 == (int *)0x0 || (*piVar10 != 0x19)))))) {
            uVar8 = 0;
          }
          else {
            uVar8 = piVar10[0x11];
          }
          if (uVar8 != uStack_6c) {
            FUN_004d6220_SideStep(((cVar2 != 'l') - 1U & 6) + 0x6c);
          }
        }
        goto LAB_004c93ce;
      }
      if (piVar10 == (int *)0x0) goto LAB_004c93ce;
      iVar3 = (**(code **)(*param_1 + 4))(piVar10);
      if ((iVar3 < *(int *)(param_1[0x3f] + 0x160)) && (param_1[0x8d] == 0)) {
        FUN_004cee70_Stop(0);
        goto LAB_004c93ce;
      }
      if ((param_1[0x58] != 0) &&
         ((iVar3 = FUN_004dab80_TActionBlock_Is(param_1[0x58]), iVar3 != 0 &&
          ((*(uint *)(param_1[0x58] + 0x24) & 0x1000000) != 0)))) goto LAB_004c93ce;
      iVar3 = FUN_004cd540_CanSeeCharacter(piVar10,0xffffffff);
      if (iVar3 == 0) {
        if ((*(byte *)(piVar10 + 2) & 0x80) == 0) {
          iVar3 = (**(code **)(*param_1 + 4))(piVar10);
          iVar6 = (**(code **)(*piVar10 + 0x2fc))();
          if ((iVar3 <= *(int *)(param_1[0x3f] + 0x1b8)) &&
             (iVar3 = (**(code **)(*param_1 + 0x2e0))(iVar3), 100 - iVar3 < iVar6))
          goto LAB_004c92c2;
        }
        iStack_64 = param_1[0x90];
        iStack_68 = param_1[0x8f];
        iStack_60 = param_1[0x91];
        piVar7 = (int *)piVar10[0x38];
        if ((piVar7 != (int *)0x0) &&
           ((((*piVar7 == 3 || ((piVar7 != (int *)0x0 && (*piVar7 == 0x19)))) &&
             (piVar7 = (int *)piVar7[0x11], piVar7 != (int *)0x0)) &&
            ((piVar7 != param_1 && (iVar3 = FUN_004cd540_CanSeeCharacter(piVar7,0xffffffff), iVar3 != 0)))))) {
          iVar3 = FUN_00483300_RandomRange(0,1);
          cVar2 = (-(iVar3 != 0) & 0xfaU) + 0x72;
          FUN_004d6220_SideStep(cVar2);
          FUN_004d6220_SideStep(((cVar2 != 'l') - 1U & 6) + 0x6c);
        }
        if (param_1[0x8d] == 0) {
LAB_004c9158:
          iStack_74 = 0;
          iStack_70 = FUN_00452060_TMapPane_FindObjectsInRange(&iStack_68,*(undefined2 *)((int)param_1 + 0xe),auStack_28,0xfa,0,
                                   0xf,10,0);
          if (10 < iStack_70) {
            iStack_70 = 10;
          }
          if (0 < iStack_70) {
            puVar12 = auStack_50;
            puVar11 = auStack_28;
            do {
              uVar8 = FUN_00452690_TMapPane_GetInstance(*puVar11,0);
              if (((uVar8 != 0) &&
                  (iVar3 = FUN_0059a530_stricmp(**(undefined4 **)(uVar8 + 0x4c),s_waypoint_005df9fc),
                  iVar3 == 0)) && (iVar3 = FUN_00528850_WaypointReachable(param_1), iVar3 != 0)) {
                *puVar12 = uVar8;
                iStack_74 = iStack_74 + 1;
                puVar12 = puVar12 + 1;
              }
              puVar11 = puVar11 + 1;
              iStack_70 = iStack_70 + -1;
            } while (iStack_70 != 0);
          }
          iVar6 = 0;
          uStack_6c = 0;
          iVar3 = 1000;
          iStack_70 = 0;
          uVar8 = uStack_6c;
          if (0 < iStack_74) {
            puVar12 = auStack_50;
            do {
              uVar8 = *puVar12;
              uStack_5c = *(undefined4 *)(uVar8 + 0x10);
              uStack_58 = *(undefined4 *)(uVar8 + 0x14);
              uStack_54 = *(undefined4 *)(uVar8 + 0x18);
              iVar9 = FUN_0046de60_Distance2D(&iStack_68,&uStack_5c);
              if (iVar9 < iVar3) {
                FUN_00470bc0_StoreLastTargetVec(&uStack_5c);
                iVar3 = iVar9;
                iStack_70 = iVar6;
              }
              iVar6 = iVar6 + 1;
              puVar12 = puVar12 + 1;
            } while (iVar6 < iStack_74);
            uVar8 = uStack_6c;
            if (0 < iStack_74) {
              uVar8 = auStack_50[iStack_70];
            }
          }
          if (param_1[0x8d] == uVar8) {
            param_1[0x8d] = 0;
          }
          else {
            param_1[0x92] = 6;
            param_1[0x8d] = uVar8;
          }
        }
        else {
          iVar3 = (**(code **)(*param_1 + 4))(param_1[0x8d]);
          if (iVar3 < 5) {
            iStack_68 = piVar10[4];
            iStack_64 = piVar10[5];
            iStack_60 = piVar10[6];
            goto LAB_004c9158;
          }
        }
        piVar10 = (int *)param_1[0x8d];
        if (piVar10 == (int *)0x0) {
          uVar5 = FUN_0046dc60_AngleToPP(param_1 + 4,&iStack_68);
          *(undefined4 *)(param_1[0x36] + 0x2c) = uVar5;
          goto LAB_004c93ce;
        }
      }
      else {
LAB_004c92c2:
        if (param_1[0x8d] != 0) {
          if (param_1[0x92] == 0) {
            param_1[0x8d] = 0;
          }
          else {
            param_1[0x92] = param_1[0x92] + -1;
          }
        }
        iVar3 = piVar10[5];
        iVar6 = piVar10[6];
        param_1[0x8f] = piVar10[4];
        param_1[0x90] = iVar3;
        param_1[0x91] = iVar6;
        if (((int *)param_1[0x8d] != (int *)0x0) && (param_1[0x92] != 0)) {
          piVar10 = (int *)param_1[0x8d];
        }
      }
      uVar5 = FUN_0046ea90_AngleTo(piVar10);
      *(undefined4 *)(param_1[0x36] + 0x2c) = uVar5;
      goto LAB_004c93ce;
    }
  }
  param_1[0x8d] = 0;
  if (0 < param_1[0x48]) {
    param_1[0x48] = param_1[0x48] + -1;
  }
  if (0 < param_1[0x49]) {
    param_1[0x49] = param_1[0x49] + -1;
  }
  if (((param_1[0x48] == 0) || (param_1[0x49] == 0)) || ((*(byte *)(param_1 + 0x4a) & 1) != 0)) {
    param_1[0x4a] = param_1[0x4a] & 0xfffffffe;
    if ((param_1[0x58] == 0) || (iVar3 = FUN_004dab80_TActionBlock_Is(param_1[0x58]), iVar3 == 0)) {
      iVar3 = (**(code **)(*param_1 + 4))(piVar10);
      if (*(int *)(param_1[0x3f] + 0x160) < iVar3) {
        uVar5 = FUN_0046ea90_AngleTo(piVar10);
        iVar3 = FUN_004ce350_Walk(uVar5);
        if (iVar3 != 0) {
          FUN_00583e80_LogActionEvent(param_1,0x1c,uVar5,1,0);
          param_1[0x58] = 0;
          param_1[0x5a] = 0;
          goto LAB_004c8e88;
        }
LAB_004c8e32:
        param_1[0x58] = 0;
        param_1[0x5a] = 0;
      }
      else {
        uVar5 = FUN_00483300_RandomRange(1,100);
        iVar3 = FUN_004d2900_RandomAttack(uVar5);
        uVar1 = *(undefined2 *)((int)param_1 + 0xe);
        iVar6 = (**(code **)(*param_1 + 600))();
        piVar7 = (int *)FUN_004d4db0_FindCharInLine(param_1,param_1 + 4,uVar1,iVar6 << 1);
        if ((piVar7 == (int *)0x0) || (piVar7 == piVar10)) {
LAB_004c8e2e:
          if (iVar3 == 0) goto LAB_004c8e32;
        }
        else if (iVar3 == 0) {
          iVar6 = FUN_0046ead0_AngleDiffFromFacing(piVar7);
          if ((iVar6 < 0x21) || (0x5f < iVar6)) {
            if ((-0x21 < iVar6) || (iVar6 < -0x5f)) goto LAB_004c8e2e;
            iStack_74 = 0x72;
          }
          else {
            iStack_74 = 0x6c;
          }
          FUN_004d6220_SideStep(iStack_74);
          goto LAB_004c8e2e;
        }
        param_1[0x4b] = param_1[0x4b] + -1;
      }
    }
LAB_004c8e88:
    if ((param_1[0x58] == 0) || ((~*(uint *)(param_1[0x58] + 0x24) >> 0x18 & 1) != 0)) {
      param_1[0x48] = param_1[0x48] + -1;
    }
  }
  if (param_1[0x48] < 0) {
    iVar3 = FUN_00483300_RandomRange((*(int *)(param_1[0x3f] + 0x1d0) * 0x18) / 100,
                         (*(int *)(param_1[0x3f] + 0x1d4) * 0x18) / 100);
    param_1[0x48] = iVar3;
  }
  if (param_1[0x49] < 0) {
    iVar3 = FUN_00483300_RandomRange((*(int *)(param_1[0x3f] + 0x1d8) * 0x18) / 100,
                         (*(int *)(param_1[0x3f] + 0x1dc) * 0x18) / 100);
    param_1[0x49] = iVar3;
  }
LAB_004c93ce:
  iVar3 = param_1[0x3f];
  iVar6 = (**(code **)(*param_1 + 0x1c0))();
  if (((*(int *)(iVar3 + 0x440) < iVar6) && (param_1[0x96] == 0)) || (param_1[0x97] == 0)) {
    iVar3 = 0;
  }
  else {
    iVar3 = 1;
  }
  param_1[0x96] = iVar3;
  param_1[0x95] = iVar3;
  if (param_1[0x97] < 1) {
    return;
  }
  param_1[0x97] = param_1[0x97] + -1;
  return;
}


