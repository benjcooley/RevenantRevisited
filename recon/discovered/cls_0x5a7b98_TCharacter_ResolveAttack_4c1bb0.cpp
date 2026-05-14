// FUN_004c1bb0_ResolveAttack @ 004c1bb0 size=5798

void __fastcall FUN_004c1bb0_ResolveAttack(int *param_1)

{
  char cVar1;
  undefined1 uVar2;
  short sVar3;
  int iVar4;
  undefined4 uVar5;
  int *piVar6;
  char *pcVar7;
  byte *pbVar8;
  int iVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  char *pcVar14;
  char *pcVar15;
  bool bVar16;
  int iStack_48;
  int iStack_44;
  char acStack_40 [2];
  undefined2 uStack_3e;
  undefined2 uStack_36;
  int iStack_34;
  int iStack_30;
  int iStack_2c;
  undefined4 uStack_14;
  undefined1 uStack_e;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e30c;
  local_c = ExceptionList;
  if (DAT_0065d0c4 != 0) {
    return;
  }
  ExceptionList = &local_c;
  FUN_00540750();
  if ((int *)param_1[0x16] != (int *)0x0) {
    (**(code **)(*(int *)param_1[0x16] + 0x2c))();
  }
  if (param_1[0x42] != 0) {
    if (param_1[0x36] != 0) {
      *(undefined4 *)(param_1[0x36] + 0x28) = 0;
    }
    param_1[0x20] = 1;
    param_1[0x42] = 0;
  }
  if (0 < param_1[0x89]) {
    param_1[0x89] = param_1[0x89] + -1;
  }
  FUN_004db190();
  piVar6 = (int *)param_1[0x38];
  if ((((piVar6 != (int *)0x0) &&
       ((iVar4 = *piVar6, iVar4 == 3 || ((piVar6 != (int *)0x0 && (iVar4 == 0x19)))))) &&
      ((iVar4 == 3 || (iVar4 == 0x19)))) && (piVar6 = (int *)piVar6[0x11], piVar6 != (int *)0x0)) {
    (**(code **)(*piVar6 + 0x23c))(param_1,piVar6);
  }
  if (5 < *(int *)(DAT_00667fd0 + 0x48) - param_1[0x45]) {
    (**(code **)(*param_1 + 0x40))(param_1[2] & 0xffefffff);
  }
  if (0x18 < *(int *)(DAT_00667fd0 + 0x48) - param_1[0x45]) {
    param_1[0x39] = 0;
  }
  iVar4 = FUN_004dab80_TActionBlock_Is(s_decapdeath_005df72c);
  if ((((iVar4 != 0) && ((*(byte *)(param_1[0x3f] + 200) & 0x10) == 0)) &&
      (*(int *)(param_1[0x3f] + 0x164) != 0)) &&
     (((sVar3 = (short)param_1[0x17], 5 < sVar3 && (sVar3 < 0x41)) && ((int)sVar3 % 5 == 0)))) {
    pcVar7 = acStack_40;
    for (iVar4 = 0xd; iVar4 != 0; iVar4 = iVar4 + -1) {
      pcVar7[0] = '\0';
      pcVar7[1] = '\0';
      pcVar7[2] = '\0';
      pcVar7[3] = '\0';
      pcVar7 = pcVar7 + 4;
    }
    acStack_40[0] = '\x19';
    acStack_40[1] = '\0';
    uStack_3e = FUN_00475210(s_blood_005df738,0);
    iStack_34 = param_1[4];
    uStack_36 = (undefined2)DAT_00666970;
    iStack_30 = param_1[5];
    iStack_2c = param_1[6] + 0x4b;
    uStack_e = 0;
    uVar5 = FUN_00450e40_TMapPane_NewObject(acStack_40,0xffffffff);
    piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(uVar5,0);
    if (piVar6 != (int *)0x0) {
      (**(code **)(*piVar6 + 0x200))(0x4b,*(char *)((int)param_1 + 0x36) + -0x5b,0x40,0x5b,5,1);
    }
  }
  iVar4 = FUN_004dab80_TActionBlock_Is(s_combat_to_ripinhalfed_005df740);
  if (((iVar4 != 0) && ((*(byte *)(param_1[0x3f] + 200) & 0x10) == 0)) &&
     ((*(int *)(param_1[0x3f] + 0x164) != 0 &&
      ((sVar3 = (short)param_1[0x17], 0x41 < sVar3 && (sVar3 < 0x49)))))) {
    uVar13 = (int)sVar3 & 0x80000001;
    bVar16 = uVar13 == 0;
    if ((int)uVar13 < 0) {
      bVar16 = (uVar13 - 1 | 0xfffffffe) == 0xffffffff;
    }
    if (bVar16) {
      pcVar7 = acStack_40;
      for (iVar4 = 0xd; iVar4 != 0; iVar4 = iVar4 + -1) {
        pcVar7[0] = '\0';
        pcVar7[1] = '\0';
        pcVar7[2] = '\0';
        pcVar7[3] = '\0';
        pcVar7 = pcVar7 + 4;
      }
      acStack_40[0] = '\x19';
      acStack_40[1] = '\0';
      uStack_3e = FUN_00475210(s_blood_005df758,0);
      iStack_30 = param_1[5];
      uStack_36 = (undefined2)DAT_00666970;
      iStack_34 = param_1[4];
      iStack_2c = param_1[6] + 0x69;
      uStack_e = 0;
      uVar5 = FUN_00450e40_TMapPane_NewObject(acStack_40,0xffffffff);
      piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(uVar5,0);
      if (piVar6 != (int *)0x0) {
        (**(code **)(*piVar6 + 0x200))(0x69,*(char *)((int)param_1 + 0x36) + -0x5b,0x40,0x5b,5,1);
      }
    }
  }
  iVar4 = FUN_004dab80_TActionBlock_Is(s_ripkilldeath_005df760);
  if ((((iVar4 != 0) && ((*(byte *)(param_1[0x3f] + 200) & 0x10) == 0)) &&
      (*(int *)(param_1[0x3f] + 0x164) != 0)) &&
     ((0x73 < (short)param_1[0x17] && ((short)param_1[0x17] < 0x7e)))) {
    iVar4 = 2;
    do {
      pcVar7 = acStack_40;
      for (iVar10 = 0xd; iVar10 != 0; iVar10 = iVar10 + -1) {
        pcVar7[0] = '\0';
        pcVar7[1] = '\0';
        pcVar7[2] = '\0';
        pcVar7[3] = '\0';
        pcVar7 = pcVar7 + 4;
      }
      acStack_40[0] = '\x19';
      acStack_40[1] = '\0';
      uStack_3e = FUN_00475210(s_blood_005df770,0);
      iStack_34 = param_1[4];
      uStack_36 = (undefined2)DAT_00666970;
      iStack_30 = param_1[5];
      iStack_2c = param_1[6] + 0x82;
      uStack_e = 0;
      uVar5 = FUN_00450e40_TMapPane_NewObject(acStack_40,0xffffffff);
      piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(uVar5,0);
      if (piVar6 != (int *)0x0) {
        (**(code **)(*piVar6 + 0x200))(0x82,*(char *)((int)param_1 + 0x36) + -0x5b,0x40,0x5b,5,1);
      }
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  iVar4 = FUN_004dab80_TActionBlock_Is(s_atkimpale_to_walk_005df778);
  if ((iVar4 != 0) ||
     ((iVar4 = FUN_004dab80_TActionBlock_Is(s_atkimpale_005df78c), iVar4 != 0 && ((short)param_1[1] == 0xb)))) {
    param_1[0x44] = param_1[0x44] & 0xffffff7f;
  }
  if (((param_1[0xa0] == 4) && (iVar4 = FUN_004dab80_TActionBlock_Is(s_impimpale_005df798), iVar4 != 0)) &&
     (DAT_00667fcc != 0)) {
    *(uint *)(DAT_00667fcc + 0x110) = *(uint *)(DAT_00667fcc + 0x110) | 0x20000;
  }
  iVar4 = FUN_0059a530_stricmp(param_1[0xe],&DAT_005df7a4);
  if (((iVar4 == 0) && (iVar4 = FUN_004dab80_TActionBlock_Is(s_walk_to_dead_005df7ac), iVar4 != 0)) &&
     ((int)(short)param_1[0x17] % 5 == 0)) {
    pcVar7 = acStack_40;
    for (iVar4 = 0xd; iVar4 != 0; iVar4 = iVar4 + -1) {
      pcVar7[0] = '\0';
      pcVar7[1] = '\0';
      pcVar7[2] = '\0';
      pcVar7[3] = '\0';
      pcVar7 = pcVar7 + 4;
    }
    acStack_40[0] = '\x19';
    acStack_40[1] = '\0';
    uStack_3e = FUN_00475210(s_blood_005df7bc,0);
    iStack_34 = param_1[4];
    uStack_36 = (undefined2)DAT_00666970;
    iStack_30 = param_1[5];
    iStack_2c = param_1[6] + 0x28;
    uStack_e = 0;
    uVar5 = FUN_00450e40_TMapPane_NewObject(acStack_40,0xffffffff);
    piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(uVar5,0);
    if (piVar6 != (int *)0x0) {
      (**(code **)(*piVar6 + 0x200))(0x28,*(char *)((int)param_1 + 0x36) + -0x80,0x20,0x20,5,1);
    }
  }
  FUN_004d57a0_UpdateFade();
  iVar4 = (**(code **)(*param_1 + 0x1c0))();
  uVar13 = param_1[0x44];
  if (iVar4 < 1) {
    if ((uVar13 & 0x40000) == 0) {
      param_1[0x44] = uVar13 | 0x40000;
      (**(code **)(*param_1 + 0x248))(0);
    }
    (**(code **)(*param_1 + 0x40))(param_1[2] & 0xff7fffff);
  }
  else if ((uVar13 & 0x40000) != 0) {
    param_1[0x44] = uVar13 & 0xfffbffff;
    (**(code **)(*param_1 + 0x24c))(0);
  }
  iVar4 = (**(code **)(*param_1 + 0x1c0))();
  if ((iVar4 < 1) && ((*(byte *)(param_1 + 0x44) & 8) == 0)) {
    if ((((short)param_1[1] != 0xb) ||
        ((((DAT_00676828 != 0 && (DAT_0067682c == 0)) || (DAT_0067682c != 0)) &&
         (((byte)DAT_006767fc & 4) != 0)))) && (iVar4 = FUN_00470040(), 0 < iVar4)) {
      if (DAT_0066829c != 0) {
        if ((DAT_0067682c == 0) || (param_1[0x8a] != 0)) goto LAB_004c21e9;
        FUN_00587310(param_1);
      }
      FUN_004d6570();
    }
LAB_004c21e9:
    if (((param_1 == (int *)0x0) || ((int *)param_1[0x36] == (int *)0x0)) ||
       (*(int *)param_1[0x36] != 0x13)) {
      iVar4 = param_1[0x38];
      *(uint *)(param_1[0x36] + 0x60) = *(uint *)(param_1[0x36] + 0x60) & 0xffffffcf;
      uVar13 = 0xffffffff;
      pcVar7 = (char *)(iVar4 + 4);
      do {
        pcVar15 = pcVar7;
        if (uVar13 == 0) break;
        uVar13 = uVar13 - 1;
        pcVar15 = pcVar7 + 1;
        cVar1 = *pcVar7;
        pcVar7 = pcVar15;
      } while (cVar1 != '\0');
      uVar13 = ~uVar13;
      pcVar7 = pcVar15 + -uVar13;
      pcVar15 = acStack_40;
      for (uVar11 = uVar13 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
        *(undefined4 *)pcVar15 = *(undefined4 *)pcVar7;
        pcVar7 = pcVar7 + 4;
        pcVar15 = pcVar15 + 4;
      }
      for (uVar13 = uVar13 & 3; uVar13 != 0; uVar13 = uVar13 - 1) {
        *pcVar15 = *pcVar7;
        pcVar7 = pcVar7 + 1;
        pcVar15 = pcVar15 + 1;
      }
      iVar4 = -1;
      pcVar7 = acStack_40;
      do {
        pcVar15 = pcVar7;
        if (iVar4 == 0) break;
        iVar4 = iVar4 + -1;
        pcVar15 = pcVar7 + 1;
        cVar1 = *pcVar7;
        pcVar7 = pcVar15;
      } while (cVar1 != '\0');
      iVar4 = param_1[1];
      *(undefined4 *)(pcVar15 + -1) = DAT_005e00c4;
      pcVar15[3] = DAT_005e00c8;
      if ((short)iVar4 == 0xb) {
        pcVar7 = (char *)(**(code **)(*param_1 + 0x310))();
        uVar13 = 0xffffffff;
        do {
          pcVar15 = pcVar7;
          if (uVar13 == 0) break;
          uVar13 = uVar13 - 1;
          pcVar15 = pcVar7 + 1;
          cVar1 = *pcVar7;
          pcVar7 = pcVar15;
        } while (cVar1 != '\0');
        uVar13 = ~uVar13;
        iVar4 = -1;
        pcVar7 = acStack_40;
        do {
          pcVar14 = pcVar7;
          if (iVar4 == 0) break;
          iVar4 = iVar4 + -1;
          pcVar14 = pcVar7 + 1;
          cVar1 = *pcVar7;
          pcVar7 = pcVar14;
        } while (cVar1 != '\0');
        pcVar7 = pcVar15 + -uVar13;
        pcVar15 = pcVar14 + -1;
        for (uVar11 = uVar13 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
          *(undefined4 *)pcVar15 = *(undefined4 *)pcVar7;
          pcVar7 = pcVar7 + 4;
          pcVar15 = pcVar15 + 4;
        }
        for (uVar13 = uVar13 & 3; uVar13 != 0; uVar13 = uVar13 - 1) {
          *pcVar15 = *pcVar7;
          pcVar7 = pcVar7 + 1;
          pcVar15 = pcVar15 + 1;
        }
      }
      else {
        iVar4 = -1;
        pcVar7 = acStack_40;
        do {
          pcVar15 = pcVar7;
          if (iVar4 == 0) break;
          iVar4 = iVar4 + -1;
          pcVar15 = pcVar7 + 1;
          cVar1 = *pcVar7;
          pcVar7 = pcVar15;
        } while (cVar1 != '\0');
        *(undefined2 *)(pcVar15 + -1) = DAT_005e00cc;
      }
      iVar4 = -1;
      pcVar7 = acStack_40;
      do {
        pcVar15 = pcVar7;
        if (iVar4 == 0) break;
        iVar4 = iVar4 + -1;
        pcVar15 = pcVar7 + 1;
        cVar1 = *pcVar7;
        pcVar7 = pcVar15;
      } while (cVar1 != '\0');
      iVar4 = *param_1;
      *(undefined4 *)(pcVar15 + -1) = DAT_005e00d0;
      pcVar15[3] = DAT_005e00d4;
      iVar4 = (**(code **)(iVar4 + 0x1f0))(acStack_40,0);
      if (iVar4 != 0) {
        iVar4 = FUN_00482fb0(100);
        local_c = (void *)0x0;
        if (iVar4 == 0) {
          iVar4 = 0;
        }
        else {
          iVar4 = FUN_004da9f0_TActionBlock_Ctor1(&iStack_48,0x13);
        }
        iVar10 = *param_1;
        local_c = (void *)0xffffffff;
        *(uint *)(iVar4 + 0x60) = *(uint *)(iVar4 + 0x60) | 0x830;
        iVar10 = (**(code **)(iVar10 + 0x218))(iVar4,0,0);
        if ((((iVar10 != 2) && (iVar4 != param_1[0x36])) && (iVar4 != param_1[0x38])) &&
           ((iVar4 != param_1[0x37] && (iVar4 != 0)))) {
          if (*(int *)(iVar4 + 0x5c) != 0) {
            FUN_00482f80(*(int *)(iVar4 + 0x5c));
          }
          FUN_004830f0(iVar4);
        }
      }
      FUN_004ce1b0_BuildActionName(&iStack_48,&DAT_005df7c4);
      iVar4 = (**(code **)(*param_1 + 0x1f0))(&iStack_48,0);
      if (iVar4 == 0) {
        uVar13 = 0xffffffff;
        pcVar7 = &DAT_005df7cc;
        do {
          pcVar15 = pcVar7;
          if (uVar13 == 0) break;
          uVar13 = uVar13 - 1;
          pcVar15 = pcVar7 + 1;
          cVar1 = *pcVar7;
          pcVar7 = pcVar15;
        } while (cVar1 != '\0');
        uVar13 = ~uVar13;
        pcVar7 = pcVar15 + -uVar13;
        pcVar15 = &stack0xffffffb0;
        for (uVar11 = uVar13 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
          *(undefined4 *)pcVar15 = *(undefined4 *)pcVar7;
          pcVar7 = pcVar7 + 4;
          pcVar15 = pcVar15 + 4;
        }
        for (uVar13 = uVar13 & 3; uVar13 != 0; uVar13 = uVar13 - 1) {
          *pcVar15 = *pcVar7;
          pcVar7 = pcVar7 + 1;
          pcVar15 = pcVar15 + 1;
        }
      }
      iVar4 = FUN_00482fb0(100);
      uStack_14 = 1;
      if (iVar4 == 0) {
        uVar5 = 0;
      }
      else {
        uVar5 = FUN_004da9f0_TActionBlock_Ctor1(&stack0xffffffb0,0x13);
      }
      uStack_14 = 0xffffffff;
      if (((param_1 == (int *)0x0) || ((int *)param_1[0x36] == (int *)0x0)) ||
         (*(int *)param_1[0x36] != 0x13)) {
        (**(code **)(*param_1 + 0x218))(uVar5,0,0);
      }
      else {
        (**(code **)(*param_1 + 0x208))(uVar5,0);
      }
      (**(code **)(*param_1 + 0x1f8))(uVar5);
    }
    if (param_1[0x21] != 0) {
      FUN_00492640(0xb,0,0,0,0,0,0);
      param_1[0x44] = param_1[0x44] | 2;
    }
    iVar4 = param_1[0x8a];
    bVar16 = DAT_0066829c == 0;
    param_1[0x8a] = iVar4 + 1;
    if (((bVar16) || ((short)param_1[1] != 0xb)) && (0xef < iVar4 + 1)) {
      if (param_1[0x65] < 1) {
        (**(code **)(*param_1 + 0x40))(param_1[2] | 0x1000);
      }
      else if (param_1[0x68] == 0) {
        (**(code **)(*param_1 + 0x1c0))();
        param_1[0x65] = 100;
        param_1[0x66] = 1;
        param_1[0x67] = 0;
        param_1[0x68] = -1;
      }
    }
    if (DAT_00676828 == 0) {
      ExceptionList = local_c;
      return;
    }
    if (DAT_0067682c != 0) {
      ExceptionList = local_c;
      return;
    }
    sVar3 = (short)param_1[1];
    goto LAB_004c31ed;
  }
  iVar4 = param_1[0x58];
  if (((iVar4 != 0) && (iVar10 = FUN_0047e920_FrameTick(), *(int *)(iVar4 + 0xc4) + param_1[0x59] <= iVar10))
     && ((uVar13 = *(uint *)(iVar4 + 0x24), (uVar13 & 0x8000) != 0 ||
         (((uVar13 & 0x4000) != 0 && (0 < param_1[0x5b])))))) {
    if (((uVar13 & 0x4000) == 0) ||
       (iVar4 = FUN_0047e920_FrameTick(), iVar4 - param_1[0x59] < *(int *)(param_1[0x58] + 0xc0))) {
      iVar4 = 0;
      piVar6 = (int *)(param_1[0x3f] + 0xcc);
      if (0 < *piVar6) {
        do {
          iVar10 = *(int *)(piVar6[4] + iVar4 * 4);
          if (iVar10 == 0) {
            iVar10 = piVar6[5];
          }
          iVar10 = FUN_0059a530_stricmp(param_1[0x58],iVar10 + 0x98);
          if (iVar10 == 0) {
            if ((*(uint *)(param_1[0x58] + 0x24) & 0x4000) != 0) {
              param_1[0x5b] = param_1[0x5b] + -1;
            }
            FUN_004d2a60_SpecificAttack(iVar4);
            goto LAB_004c25d5;
          }
          iVar4 = iVar4 + 1;
          piVar6 = (int *)(param_1[0x3f] + 0xcc);
        } while (iVar4 < *piVar6);
      }
    }
    param_1[0x5b] = 0;
  }
LAB_004c25d5:
  if ((((short)param_1[1] == 0xb) &&
      (((int *)param_1[0x38] == (int *)0x0 ||
       ((((*(int *)param_1[0x38] != 3 ||
          ((iVar4 = FUN_004dab80_TActionBlock_Is(s_combatrun_005e0618), iVar4 == 0 &&
           (iVar4 = FUN_004dab80_TActionBlock_Is(s_handrun_005e0610), iVar4 == 0)))) &&
         (((int *)param_1[0x38] == (int *)0x0 ||
          ((*(int *)param_1[0x38] != 0x19 || (iVar4 = FUN_004dab80_TActionBlock_Is(s_bowrun_005e0608), iVar4 == 0)))
          ))) && (iVar4 = FUN_004dab80_TActionBlock_Is(&DAT_005e0604), iVar4 == 0)))))) &&
     (((param_1[0x38] == 0 || (iVar4 = FUN_004dab80_TActionBlock_Is(s_sneak_005c618c), iVar4 == 0)) &&
      (uVar13 = param_1[0x10], uVar11 = FUN_0047e920_FrameTick(), ((uVar11 ^ uVar13) & 0x1f) == 0)))) {
    iVar4 = FUN_004cd690_FindCharacters(&iStack_48,1,0xffffffff,*(undefined1 *)((int)param_1 + 0x36),0x20,7);
    if ((iVar4 < 1) || (iStack_48 == 0)) {
      if (param_1[0x3a] == 0) {
        param_1[0x3a] = 1;
      }
    }
    else {
      iVar4 = FUN_004cd990_IsValidTarget(iStack_48);
      if (((iVar4 != 0) &&
          ((piVar6 = (int *)param_1[0x38], piVar6 == (int *)0x0 ||
           ((*piVar6 != 3 &&
            ((piVar6 == (int *)0x0 ||
             ((*piVar6 != 0x19 &&
              ((piVar6 == (int *)0x0 || (iVar4 = FUN_004dab80_TActionBlock_Is(s_sneak_005c618c), iVar4 == 0)))))))))
           ))) && (((int *)param_1[0x38] == (int *)0x0 || (*(int *)param_1[0x38] != 0x19)))) {
        if (DAT_0065a784 < 0) {
          pbVar8 = (byte *)0x0;
        }
        else {
          pbVar8 = *(byte **)(DAT_0065a77c + DAT_0065a784 * 4);
        }
        if (((((*pbVar8 & 8) != 0) || (*(short *)(iStack_48 + 4) != 0xb)) && (DAT_005d7a68 != 0)) &&
           (param_1[0x3a] != 0)) {
          iVar4 = 0;
          piVar6 = param_1 + 0x70;
          do {
            if (*piVar6 == iStack_48) {
              if (param_1[iVar4 * 3 + 0x72] != 0) goto LAB_004c2867;
              break;
            }
            iVar4 = iVar4 + 1;
            piVar6 = piVar6 + 3;
          } while (iVar4 < 8);
          iVar4 = (**(code **)(*param_1 + 4))(iStack_48);
          if (iVar4 < *(int *)(param_1[0x3f] + 0x158)) {
            uVar2 = *(undefined1 *)((int)param_1 + 0x36);
            uVar5 = FUN_0046ea90_AngleTo(iStack_48);
            iVar4 = FUN_0046ded0(uVar2,uVar5);
            if (iVar4 < 0) {
              uVar2 = *(undefined1 *)((int)param_1 + 0x36);
              uVar5 = FUN_0046ea90_AngleTo(iStack_48);
              iVar4 = FUN_0046ded0(uVar2,uVar5);
              iVar4 = -iVar4;
            }
            else {
              uVar2 = *(undefined1 *)((int)param_1 + 0x36);
              uVar5 = FUN_0046ea90_AngleTo(iStack_48);
              iVar4 = FUN_0046ded0(uVar2,uVar5);
            }
            if ((iVar4 < 0x30) && ((DAT_0065d0d0 != 0 || (DAT_0065d0c8 != 0)))) {
              FUN_004d3b90_BeginFighting(iStack_48,3);
            }
          }
        }
      }
    }
  }
LAB_004c2867:
  if ((param_1[0x44] & 0x20000U) != 0) {
    if (((param_1[0x37] == 0) || (iVar4 = FUN_004dab80_TActionBlock_Is(&DAT_005df7d4), iVar4 == 0)) ||
       ((param_1[0x38] == 0 || (iVar4 = FUN_004dab80_TActionBlock_Is(&DAT_005df7dc), iVar4 == 0)))) {
      *(uint *)(param_1[0x38] + 0x60) = *(uint *)(param_1[0x38] + 0x60) & 0xffffffef;
      iVar4 = param_1[0x36];
      if (((iVar4 == 0) || ((*(byte *)(iVar4 + 0x60) & 0x10) == 0)) || (param_1[0x37] == iVar4)) {
        iStack_44 = FUN_00482fb0(100);
        uStack_4 = 2;
        if (iStack_44 == 0) {
          iVar4 = 0;
        }
        else {
          iVar4 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005df7e4,1);
        }
        *(uint *)(iVar4 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
        uStack_4 = 0xffffffff;
        (**(code **)(*param_1 + 0x218))(iVar4,0,0);
      }
      *(uint *)(param_1[0x37] + 0x60) = *(uint *)(param_1[0x37] + 0x60) & 0xffffffef;
      iStack_44 = FUN_00482fb0(100);
      uStack_4 = 3;
      if (iStack_44 == 0) {
        iVar4 = 0;
      }
      else {
        iVar4 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005df7ec,1);
      }
      iVar10 = *param_1;
      uStack_4 = 0xffffffff;
      *(uint *)(iVar4 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
      iVar10 = (**(code **)(iVar10 + 0x208))(iVar4,0);
      if ((iVar10 == 0) && (iVar4 != 0)) {
        if (*(int *)(iVar4 + 0x5c) != 0) {
          FUN_00482f80(*(int *)(iVar4 + 0x5c));
        }
        FUN_004830f0(iVar4);
      }
    }
    param_1[0x44] = param_1[0x44] & 0xfffdffff;
  }
  if ((param_1[0x44] & 0x100000U) == 0) {
    if ((*(byte *)(param_1 + 2) & 0x20) != 0) {
      (**(code **)(*param_1 + 0x230))();
    }
  }
  else {
    (**(code **)(*param_1 + 0x234))();
    (**(code **)(*param_1 + 0x230))();
  }
  iVar10 = (**(code **)(*param_1 + 0x314))();
  iVar4 = param_1[0x4e];
  if (iVar10 < iVar4) {
LAB_004c2a01:
    param_1[0x4e] = iVar10;
  }
  else {
    iVar12 = iVar10 - iVar4;
    if (iVar12 < 0) {
      iVar12 = iVar4 - iVar10;
    }
    if (50000 < iVar12) goto LAB_004c2a01;
  }
  iVar4 = param_1[0x4f];
  if (iVar10 < iVar4) {
LAB_004c2a24:
    param_1[0x4f] = iVar10;
  }
  else {
    iVar12 = iVar10 - iVar4;
    if (iVar12 < 0) {
      iVar12 = iVar4 - iVar10;
    }
    if (50000 < iVar12) goto LAB_004c2a24;
  }
  iVar4 = param_1[0x50];
  if (iVar10 < iVar4) {
LAB_004c2a47:
    param_1[0x50] = iVar10;
  }
  else {
    iVar12 = iVar10 - iVar4;
    if (iVar12 < 0) {
      iVar12 = iVar4 - iVar10;
    }
    if (50000 < iVar12) goto LAB_004c2a47;
  }
  iVar4 = param_1[0x51];
  if (iVar10 < iVar4) {
LAB_004c2a6a:
    param_1[0x51] = iVar10;
  }
  else {
    iVar12 = iVar10 - iVar4;
    if (iVar12 < 0) {
      iVar12 = iVar4 - iVar10;
    }
    if (50000 < iVar12) goto LAB_004c2a6a;
  }
  iVar4 = param_1[0x57];
  if (iVar10 < iVar4) {
LAB_004c2a8d:
    param_1[0x57] = iVar10;
  }
  else {
    iVar12 = iVar10 - iVar4;
    if (iVar12 < 0) {
      iVar12 = iVar4 - iVar10;
    }
    if (50000 < iVar12) goto LAB_004c2a8d;
  }
  if ((DAT_00668104 == 0) || ((short)param_1[1] != 0xb)) {
    iVar4 = (**(code **)(*param_1 + 0x1d8))();
    iVar12 = (**(code **)(*param_1 + 0x1c0))();
    if (iVar12 < iVar4) {
      iVar4 = 0;
      param_1[0x52] =
           param_1[0x52] + ((iVar10 - param_1[0x4e]) * DAT_0065d81c * 10000) / DAT_0065d828;
      if ((short)param_1[1] == 0xb) {
        iVar12 = (**(code **)(*param_1 + 0xdc))(0x15);
        param_1[0x52] = param_1[0x52] + (iVar12 * 10000) / 0x5a0;
      }
      if (10000 < param_1[0x52]) {
        iVar12 = param_1[0x52];
        do {
          iVar12 = iVar12 + -10000;
          iVar4 = iVar4 + 1;
        } while (10000 < iVar12);
        param_1[0x52] = iVar12;
        if (0 < iVar4) {
          iVar12 = (**(code **)(*param_1 + 0x1c0))();
          iVar9 = (**(code **)(*param_1 + 0x1d8))();
          if (iVar12 + iVar4 < iVar9) {
            iVar12 = (**(code **)(*param_1 + 0x1c0))();
            iVar12 = iVar12 + iVar4;
          }
          else {
            iVar12 = (**(code **)(*param_1 + 0x1d8))();
          }
          (**(code **)(*param_1 + 0x1c4))(iVar12);
        }
      }
    }
    param_1[0x4e] = iVar10;
    iVar4 = (**(code **)(*param_1 + 0x1e8))();
    iVar12 = (**(code **)(*param_1 + 0x1d0))();
    if (iVar12 < iVar4) {
      iVar4 = 0;
      param_1[0x54] =
           param_1[0x54] + ((iVar10 - param_1[0x50]) * DAT_0065d824 * 10000) / DAT_0065d830;
      if ((short)param_1[1] == 0xb) {
        iVar12 = (**(code **)(*param_1 + 0xdc))(0x16);
        param_1[0x54] = param_1[0x54] + (iVar12 * 10000) / 0x5a0;
      }
      if (10000 < param_1[0x54]) {
        iVar12 = param_1[0x54];
        do {
          iVar12 = iVar12 + -10000;
          iVar4 = iVar4 + 1;
        } while (10000 < iVar12);
        param_1[0x54] = iVar12;
        if (0 < iVar4) {
          iVar12 = (**(code **)(*param_1 + 0x1d0))();
          iVar9 = (**(code **)(*param_1 + 0x1e8))();
          if (iVar12 + iVar4 < iVar9) {
            iVar12 = (**(code **)(*param_1 + 0x1d0))();
            iVar12 = iVar12 + iVar4;
          }
          else {
            iVar12 = (**(code **)(*param_1 + 0x1e8))();
          }
          (**(code **)(*param_1 + 0x1d4))(iVar12);
        }
      }
    }
    param_1[0x50] = iVar10;
    if ((((int *)param_1[0x38] == (int *)0x0) ||
        ((((*(int *)param_1[0x38] != 3 ||
           ((iVar4 = FUN_004dab80_TActionBlock_Is(s_combatrun_005e0618), iVar4 == 0 &&
            (iVar4 = FUN_004dab80_TActionBlock_Is(s_handrun_005e0610), iVar4 == 0)))) &&
          (((int *)param_1[0x38] == (int *)0x0 ||
           ((*(int *)param_1[0x38] != 0x19 || (iVar4 = FUN_004dab80_TActionBlock_Is(s_bowrun_005e0608), iVar4 == 0))
           )))) && (iVar4 = FUN_004dab80_TActionBlock_Is(&DAT_005e0604), iVar4 == 0)))) ||
       (param_1[0x36] == param_1[0x38])) {
      param_1[0x51] = -1;
      iVar4 = (**(code **)(*param_1 + 0x1e0))();
      iVar12 = (**(code **)(*param_1 + 0x1c8))();
      if (iVar12 < iVar4) {
        iVar4 = 0;
        param_1[0x53] =
             param_1[0x53] + ((iVar10 - param_1[0x4f]) * DAT_0065d820 * 10000) / DAT_0065d82c;
        if ((short)param_1[1] == 0xb) {
          iVar12 = (**(code **)(*param_1 + 0xdc))(0x17);
          param_1[0x53] = param_1[0x53] + (iVar12 * 10000) / 0x5a0;
        }
        if (10000 < param_1[0x53]) {
          iVar12 = param_1[0x53];
          do {
            iVar12 = iVar12 + -10000;
            iVar4 = iVar4 + 1;
          } while (10000 < iVar12);
          param_1[0x53] = iVar12;
          if (0 < iVar4) {
            iVar12 = (**(code **)(*param_1 + 0x1c8))();
            iVar9 = (**(code **)(*param_1 + 0x1e0))();
            if (iVar12 + iVar4 < iVar9) {
              iVar12 = (**(code **)(*param_1 + 0x1c8))();
              iVar12 = iVar12 + iVar4;
            }
            else {
              iVar12 = (**(code **)(*param_1 + 0x1e0))();
            }
            (**(code **)(*param_1 + 0x1cc))(iVar12);
          }
        }
      }
    }
    else {
      if ((param_1[0x51] != -1) && (iVar4 = (**(code **)(*param_1 + 0x1c8))(), 0 < iVar4)) {
        iVar12 = 0;
        iVar4 = param_1[0x55] +
                ((iVar10 - param_1[0x51]) * *(int *)(param_1[0x3f] + 0x44c) * 10000) /
                *(int *)(param_1[0x3f] + 0x450);
        param_1[0x55] = iVar4;
        if (10000 < iVar4) {
          do {
            iVar4 = iVar4 + -10000;
            iVar12 = iVar12 + 1;
          } while (10000 < iVar4);
          param_1[0x55] = iVar4;
          if (0 < iVar12) {
            iVar4 = (**(code **)(*param_1 + 0x1c8))();
            if (iVar4 - iVar12 < 0) {
              iVar4 = 0;
            }
            else {
              iVar4 = (**(code **)(*param_1 + 0x1c8))();
              iVar4 = iVar4 - iVar12;
            }
            (**(code **)(*param_1 + 0x1cc))(iVar4);
          }
        }
      }
      param_1[0x51] = iVar10;
    }
    param_1[0x4f] = iVar10;
    iVar4 = (**(code **)(*param_1 + 0x1b0))();
    if (iVar4 == 0) {
      param_1[0x57] = -1;
    }
    else {
      if (param_1[0x57] != -1) {
        iVar4 = (**(code **)(*param_1 + 0x1c0))();
        if (iVar4 < 2) {
          (**(code **)(*param_1 + 0x1b4))(0);
        }
        else {
          iVar4 = (iVar10 - param_1[0x57]) * DAT_0065d834;
          iVar9 = DAT_0065d838 * 100;
          iVar12 = (**(code **)(*param_1 + 0x2c8))(9);
          iVar12 = param_1[0x56] + ((100 - iVar12) * ((iVar4 * 240000) / iVar9)) / 100;
          param_1[0x56] = iVar12;
          iVar4 = 0;
          while (10000 < iVar12) {
            iVar4 = iVar4 + 1;
            iVar12 = param_1[0x56] + -10000;
            param_1[0x56] = iVar12;
          }
          iVar12 = (**(code **)(*param_1 + 0x1c0))();
          if (iVar12 <= iVar4) {
            iVar4 = (**(code **)(*param_1 + 0x1c0))();
            iVar4 = iVar4 + -1;
          }
          if (0 < iVar4) {
            (**(code **)(*param_1 + 0x228))(iVar4,9,0,0,0);
            param_1[0x57] = iVar10;
            goto LAB_004c30dd;
          }
        }
      }
      param_1[0x57] = iVar10;
    }
LAB_004c30dd:
    if (param_1 != (int *)0x0) {
      if (((((int *)param_1[0x36] != (int *)0x0) && (*(int *)param_1[0x36] == 0xb)) &&
          (0 < param_1[0x62])) && (iVar4 = param_1[0x62] + -1, param_1[0x62] = iVar4, iVar4 == 0)) {
        iVar4 = param_1[0x38];
        iVar10 = *param_1;
        *(uint *)(iVar4 + 0x60) = *(uint *)(iVar4 + 0x60) | 0x20;
        (**(code **)(iVar10 + 0x208))(iVar4,0);
      }
      goto LAB_004c3131;
    }
  }
  else {
    iVar4 = (**(code **)(*param_1 + 0x1d8))();
    iVar10 = (**(code **)(*param_1 + 0x1c0))();
    if (iVar10 < iVar4) {
      iVar4 = *param_1;
      uVar5 = (**(code **)(iVar4 + 0x1d8))();
      (**(code **)(iVar4 + 0x1c4))(uVar5);
    }
    iVar4 = (**(code **)(*param_1 + 0x1e8))();
    iVar10 = (**(code **)(*param_1 + 0x1d0))();
    if (iVar10 < iVar4) {
      iVar4 = *param_1;
      uVar5 = (**(code **)(iVar4 + 0x1e8))();
      (**(code **)(iVar4 + 0x1d4))(uVar5);
    }
    iVar4 = (**(code **)(*param_1 + 0x1e0))();
    iVar10 = (**(code **)(*param_1 + 0x1c8))();
    if (iVar10 < iVar4) {
      iVar4 = *param_1;
      uVar5 = (**(code **)(iVar4 + 0x1e0))();
      (**(code **)(iVar4 + 0x1cc))(uVar5);
    }
    (**(code **)(*param_1 + 0x1b4))(0);
LAB_004c3131:
    if (param_1 != (int *)0x0) {
      if ((((int *)param_1[0x36] != (int *)0x0) && (*(int *)param_1[0x36] == 0x15)) &&
         ((0 < param_1[0x6f] && (iVar4 = param_1[0x6f] + -1, param_1[0x6f] = iVar4, iVar4 == 0)))) {
        iVar4 = param_1[0x38];
        iVar10 = *param_1;
        *(uint *)(iVar4 + 0x60) = *(uint *)(iVar4 + 0x60) | 0x20;
        (**(code **)(iVar10 + 0x208))(iVar4,0);
      }
      if ((((int *)param_1[0x36] != (int *)0x0) && (*(int *)param_1[0x36] == 0x16)) &&
         (iVar4 = (**(code **)(*param_1 + 0x154))(), iVar4 != 0)) {
        iVar4 = param_1[0x38];
        iVar10 = *param_1;
        *(uint *)(iVar4 + 0x60) = *(uint *)(iVar4 + 0x60) | 0x20;
        (**(code **)(iVar10 + 0x208))(iVar4,0);
        FUN_004d3b90_BeginFighting(0,3);
      }
    }
  }
  if (DAT_00676828 == 0) {
    ExceptionList = local_c;
    return;
  }
  if (DAT_0067682c != 0) {
    ExceptionList = local_c;
    return;
  }
  sVar3 = (short)param_1[1];
LAB_004c31ed:
  if (((sVar3 != 0xb) && (iVar4 = (**(code **)(*param_1 + 0x1a8))(), iVar4 != 0)) &&
     (iVar4 = FUN_0047e920_FrameTick(), param_1[0x9b] + 0x78 < iVar4)) {
    if (param_1 == (int *)0x0) {
      iVar4 = 0;
    }
    else {
      iVar4 = param_1[0x10];
    }
    FUN_00583f60(0,0x4c,iVar4,3,0x11);
    iVar4 = FUN_0047e920_FrameTick();
    param_1[0x9b] = iVar4;
  }
  ExceptionList = local_c;
  return;
}


