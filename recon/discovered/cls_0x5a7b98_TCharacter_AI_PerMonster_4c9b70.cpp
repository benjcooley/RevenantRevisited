// FUN_004c9b70_AI_PerMonster @ 004c9b70 size=14652

/* WARNING: Removing unreachable block (ram,0x004cc0bc) */
/* WARNING: Removing unreachable block (ram,0x004ca445) */
/* WARNING: Removing unreachable block (ram,0x004cc031) */
/* WARNING: Removing unreachable block (ram,0x004cc230) */
/* WARNING: Type propagation algorithm not settling */

undefined4 __fastcall FUN_004c9b70_AI_PerMonster(int *param_1)

{
  uint *puVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  char cVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  undefined4 uVar8;
  int iVar9;
  undefined4 *puVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  int iVar14;
  char *pcVar15;
  int iVar16;
  int iVar17;
  bool bVar18;
  char *pcVar19;
  int iStack_158;
  int *piStack_154;
  int local_150;
  int aiStack_14c [5];
  int local_138;
  int local_134;
  int local_130;
  int iStack_12c;
  int iStack_128;
  undefined4 uStack_124;
  int iStack_120;
  int iStack_11c;
  undefined4 uStack_118;
  int aiStack_114 [5];
  undefined4 local_100;
  int iStack_fc;
  undefined2 uStack_f6;
  int local_f4;
  int iStack_f0;
  int iStack_ec;
  undefined4 uStack_b8;
  undefined4 uStack_b4;
  undefined4 uStack_b0;
  int iStack_ac;
  int iStack_a8;
  int iStack_a4;
  int iStack_a0;
  int iStack_90;
  int *piStack_48;
  undefined4 uStack_10;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e78c;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  if (param_1[0xa0] == 0) {
    uVar8 = *(undefined4 *)param_1[0x13];
    ExceptionList = &local_c;
    iVar5 = FUN_0059a530_stricmp(uVar8,&s_Baez_005dfa14);
    if (iVar5 == 0) {
      param_1[0xa0] = 1;
      if (param_1[0xa1] == 0) {
        iVar5 = FUN_00482fb0(0x58);
        param_1[0xa1] = iVar5;
      }
      puVar10 = (undefined4 *)param_1[0xa1];
      for (iVar5 = 0x16; iVar5 != 0; iVar5 = iVar5 + -1) {
        *puVar10 = 0;
        puVar10 = puVar10 + 1;
      }
    }
    else {
      iVar5 = FUN_0059a530_stricmp(uVar8,s_Solifuge_005dfa1c);
      if (iVar5 == 0) {
        param_1[0xa0] = 2;
        if (param_1[0xa1] == 0) {
          iVar5 = FUN_00482fb0(0x34);
          param_1[0xa1] = iVar5;
        }
        puVar10 = (undefined4 *)param_1[0xa1];
        for (iVar5 = 0xd; iVar5 != 0; iVar5 = iVar5 + -1) {
          *puVar10 = 0;
          puVar10 = puVar10 + 1;
        }
      }
      else {
        iVar5 = FUN_0059a530_stricmp(uVar8,s_Jhaga_005dfa28);
        if (iVar5 == 0) {
          param_1[0xa0] = 3;
          if (param_1[0xa1] == 0) {
            iVar5 = FUN_00482fb0(0x5c);
            param_1[0xa1] = iVar5;
          }
          puVar10 = (undefined4 *)param_1[0xa1];
          for (iVar5 = 0x17; iVar5 != 0; iVar5 = iVar5 + -1) {
            *puVar10 = 0;
            puVar10 = puVar10 + 1;
          }
        }
        else {
          iVar5 = FUN_0059a530_stricmp(uVar8,s_Yhagoro_005dfa30);
          if (iVar5 == 0) {
            param_1[0xa0] = 4;
            if (param_1[0xa1] == 0) {
              iVar5 = FUN_00482fb0(0xc);
              param_1[0xa1] = iVar5;
            }
            puVar10 = (undefined4 *)param_1[0xa1];
            *puVar10 = 0;
            puVar10[1] = 0;
            puVar10[2] = 0;
          }
          else {
            param_1[0xa0] = -1;
          }
        }
      }
    }
  }
  switch(param_1[0xa0]) {
  case 1:
    break;
  case 2:
    param_1[0x44] = param_1[0x44] | 0x80;
    if (DAT_00667fcc != (int *)0x0) {
      iVar5 = (**(code **)(*DAT_00667fcc + 0x1c0))();
      if ((iVar5 < 1) &&
         (((param_1 == (int *)0x0 || (param_1[0x36] == 0)) ||
          (iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_005dfd90), iVar5 == 0)))) {
        DAT_00667fcc[0x44] = DAT_00667fcc[0x44] | 0x1000;
        ExceptionList = local_c;
        return 1;
      }
      piVar6 = DAT_00667fcc;
      bVar18 = DAT_00667fcc != (int *)0x0;
      DAT_00667fcc[0x44] = DAT_00667fcc[0x44] & 0xffffefff;
      if (bVar18) {
        aiStack_14c[0] = piVar6[4];
        aiStack_14c[1] = piVar6[5];
        aiStack_14c[2] = piVar6[6];
      }
    }
    local_138 = param_1[5];
    aiStack_14c[4] = param_1[4];
    local_134 = param_1[6];
    uVar11 = param_1[2];
    iVar5 = (**(code **)(*param_1 + 0x1c0))();
    iVar9 = (**(code **)(*param_1 + 0x1d8))();
    iVar9 = (iVar5 * 100) / iVar9;
    if ((iVar9 < 0x43) || (100 < iVar9)) {
      if ((iVar9 < 0x22) || (0x42 < iVar9)) {
        iVar5 = param_1[0xa1];
        if (*(int *)(iVar5 + 0xc) == 1) {
          *(undefined4 *)(iVar5 + 0x2c) = 1;
        }
        *(undefined4 *)(iVar5 + 0xc) = 2;
      }
      else {
        iVar5 = param_1[0xa1];
        if (*(int *)(iVar5 + 0xc) == 0) {
          *(undefined4 *)(iVar5 + 0x2c) = 1;
        }
        *(undefined4 *)(iVar5 + 0xc) = 1;
      }
    }
    else {
      *(undefined4 *)(param_1[0xa1] + 0xc) = 0;
    }
    iVar5 = (**(code **)(*param_1 + 0x1c0))();
    if (iVar5 < 1) {
      piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(*(undefined4 *)(param_1[0xa1] + 0x1c),0);
      if (piVar6 != (int *)0x0) {
        (**(code **)(*piVar6 + 0x1c4))(0);
      }
      piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(*(undefined4 *)(param_1[0xa1] + 0x20),0);
      if (piVar6 != (int *)0x0) {
        (**(code **)(*piVar6 + 0x1c4))(0);
      }
      piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(*(undefined4 *)(param_1[0xa1] + 0x24),0);
      if (piVar6 != (int *)0x0) {
        (**(code **)(*piVar6 + 0x1c4))(0);
      }
    }
    if ((uVar11 & 0x800000) == 0) {
      iVar5 = param_1[0xa1];
      if (*(int *)(iVar5 + 0x18) != 0) {
        FUN_004d6b40_RestoreMaterials();
        iVar5 = param_1[0xa1];
        *(undefined4 *)(iVar5 + 0x18) = 0;
      }
      if (*(int *)(iVar5 + 0x14) == 0) {
        if (*(int *)(iVar5 + 0x30) == 0) {
          param_1[0x44] = param_1[0x44] & 0xffff7ffb;
        }
        iVar5 = *(int *)(iVar5 + 0x2c);
        param_1[0x44] = param_1[0x44] | 0x10;
        if (iVar5 != 0) {
          if (((param_1 == (int *)0x0) || (param_1[0x36] == 0)) ||
             (iVar5 = FUN_004dab80_TActionBlock_Is(s_pissedoff_005dfd98), iVar5 == 0)) {
            iStack_158 = FUN_00482fb0(100);
            uStack_4 = 0x29;
            if (iStack_158 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_pissedoff_005dfda4,1);
            }
            iVar9 = *param_1;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              if (*(int *)(iVar5 + 0x5c) != 0) {
                FUN_00482f80(*(int *)(iVar5 + 0x5c));
              }
              FUN_004830f0(iVar5);
            }
          }
          else {
            *(undefined4 *)(param_1[0xa1] + 0x2c) = 0;
          }
        }
        if (((param_1 == (int *)0x0) || (param_1[0x36] == 0)) ||
           ((iVar5 = FUN_004dab80_TActionBlock_Is(s_pissedoff_005dfdb0), iVar5 == 0 ||
            (*(int *)(param_1[0xa1] + 0x30) != 0)))) {
          uVar11 = param_1[0x44] & 0xffff7fff;
        }
        else {
          uVar11 = param_1[0x44] | 0x8000;
        }
        param_1[0x44] = uVar11;
        if (((*(int *)(param_1[0xa1] + 0x30) != 0) && (param_1 != (int *)0x0)) &&
           (((param_1[0x36] != 0 && (iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_005dfdbc), iVar5 != 0)) ||
            ((param_1[0x36] != 0 && (iVar5 = FUN_004dab80_TActionBlock_Is(s_combatf_005dfdc4), iVar5 != 0)))))) {
          iVar5 = param_1[0xa1];
          *(undefined4 *)(iVar5 + 0x30) = 0;
          *(undefined4 *)(iVar5 + 0x14) = 1;
          param_1[0x44] = param_1[0x44] & 0xffff7ff7U | 4;
          iVar5 = aiStack_14c[4] - aiStack_14c[0];
          if (iVar5 < 0) {
            iVar5 = aiStack_14c[0] - aiStack_14c[4];
          }
          iVar9 = local_138 - aiStack_14c[1];
          if (iVar9 < 0) {
            iVar9 = aiStack_14c[1] - local_138;
          }
          iVar14 = iVar5;
          if (iVar9 <= iVar5) {
            iVar14 = iVar9;
          }
          iVar16 = (**(code **)(*param_1 + 600))();
          if (((iVar9 - (iVar14 >> 1)) + iVar5 < iVar16 * 3) && (DAT_00667fcc != (int *)0x0)) {
            FUN_004ce1b0_BuildActionName(aiStack_114,&DAT_005dfdcc);
            piVar6 = DAT_00667fcc;
            iStack_158 = FUN_00482fb0(100);
            uStack_4 = 0x2a;
            if (iStack_158 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(aiStack_114,1);
            }
            iVar9 = *piVar6;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar6 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              if (*(int *)(iVar5 + 0x5c) != 0) {
                FUN_00482f80(*(int *)(iVar5 + 0x5c));
              }
              FUN_004830f0(iVar5);
            }
            (**(code **)(*DAT_00667fcc + 0x48))(*(int *)(param_1[0x3f] + 0x1c0) << 1,0);
          }
        }
      }
      else {
        param_1[0x44] = param_1[0x44] & 0xffff7fefU | 4;
        if ((param_1 == (int *)0x0) ||
           ((((param_1[0x36] == 0 || (iVar5 = FUN_004dab80_TActionBlock_Is(s_stunned_005dfdd4), iVar5 == 0)) &&
             ((param_1[0x36] == 0 ||
              (iVar5 = FUN_004dab80_TActionBlock_Is(s_skywalk_to_combat_005dfddc), iVar5 == 0)))) &&
            ((param_1[0x36] == 0 || (iVar5 = FUN_004dab80_TActionBlock_Is(s_skywalk_005dfdf0), iVar5 == 0)))))) {
          *(uint *)(param_1[0x36] + 0x60) = *(uint *)(param_1[0x36] + 0x60) & 0xffffffef;
          iStack_158 = FUN_00482fb0(100);
          uStack_4 = 0x2b;
          if (iStack_158 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_stunned_005dfdf8,1);
          }
          *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
          uStack_4 = 0xffffffff;
          (**(code **)(*param_1 + 0x218))(iVar5,0,0);
        }
        iVar5 = param_1[0xa1];
        iVar9 = *(int *)(iVar5 + 0x10) + -1;
        *(int *)(iVar5 + 0x10) = iVar9;
        if (iVar9 < 1) {
          *(undefined4 *)(iVar5 + 0x14) = 0;
          *(undefined4 *)(iVar5 + 0x10) = 0x48;
          *(uint *)(param_1[0x36] + 0x60) = *(uint *)(param_1[0x36] + 0x60) & 0xffffffef;
          iStack_158 = FUN_00482fb0(100);
          uStack_4 = 0x2c;
          if (iStack_158 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_005dfe00,1);
          }
          iVar9 = *param_1;
          *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
          uStack_4 = 0xffffffff;
          (**(code **)(iVar9 + 0x218))(iVar5,0,0);
        }
      }
      piVar6 = (int *)param_1[0xa1];
      if ((*piVar6 != 0) && (piVar6[1] == 0)) {
        *piVar6 = *piVar6 + -1;
        if ((param_1 != (int *)0x0) &&
           (((param_1[0x36] != 0 && (iVar5 = FUN_004dab80_TActionBlock_Is(s_skywalk_005dfe08), iVar5 != 0)) ||
            ((param_1[0x36] != 0 && (iVar5 = FUN_004dab80_TActionBlock_Is(s_skywalkf_005dfe10), iVar5 != 0)))))) {
          iStack_158 = FUN_00482fb0(100);
          uStack_4 = 0x2d;
          if (iStack_158 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_skywalk_to_combat_005dfe1c,1);
          }
          iVar9 = *param_1;
          uStack_4 = 0xffffffff;
          *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
          iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
          if ((iVar9 == 0) && (iVar5 != 0)) {
            if (*(int *)(iVar5 + 0x5c) != 0) {
              FUN_00482f80(*(int *)(iVar5 + 0x5c));
            }
            FUN_004830f0(iVar5);
          }
        }
        piVar6 = (int *)param_1[0xa1];
        if (((*piVar6 == 0) && (piVar6[1] == 0)) && (piVar6[1] = 1, 1 < piVar6[3])) {
          *piVar6 = 0x1e0;
        }
      }
      piVar7 = piVar6 + 1;
      if (piVar6[1] == 1) {
        *(uint *)(param_1[0x36] + 0x60) = *(uint *)(param_1[0x36] + 0x60) & 0xffffffef;
        iVar5 = FUN_00482fb0(100);
        uStack_4 = 0x2e;
        if (iVar5 == 0) {
          iVar5 = 0;
        }
        else {
          iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_to_skywalk_005dfe30,1);
        }
        iVar9 = *param_1;
        *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
        uStack_4 = 0xffffffff;
        (**(code **)(iVar9 + 0x218))(iVar5,0,0);
        iStack_158 = FUN_00482fb0(100);
        uStack_4 = 0x2f;
        if (iStack_158 == 0) {
          iVar5 = 0;
        }
        else {
          iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_skywalk_005dfe44,1);
        }
        iVar9 = *param_1;
        uStack_4 = 0xffffffff;
        *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
        iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
        if ((iVar9 == 0) && (iVar5 != 0)) {
          if (*(int *)(iVar5 + 0x5c) != 0) {
            FUN_00482f80(*(int *)(iVar5 + 0x5c));
          }
          FUN_004830f0(iVar5);
        }
        iVar5 = param_1[0xa1];
        piVar7 = (int *)(iVar5 + 4);
        *piVar7 = 2;
        if (*(int *)(iVar5 + 0xc) < 2) {
          *(undefined4 *)(iVar5 + 8) = 0x78;
        }
        else {
          *(undefined4 *)(iVar5 + 8) = 0xf0;
        }
        *(undefined4 *)(iVar5 + 0x28) = 0;
      }
      if (*piVar7 == 2) {
        param_1[0x44] = param_1[0x44] | 0x800c;
        if (((param_1 == (int *)0x0) || (param_1[0x36] == 0)) ||
           (iVar5 = FUN_004dab80_TActionBlock_Is(s_skywalkf_005dfe4c), iVar5 == 0)) {
          uVar11 = param_1[0x44] & 0xffffffbf;
        }
        else {
          uVar11 = param_1[0x44] | 0x40;
        }
        param_1[0x44] = uVar11;
        iVar5 = param_1[0xa1];
        iVar9 = *(int *)(iVar5 + 8) + -1;
        *(int *)(iVar5 + 8) = iVar9;
        if ((iVar9 == 0xc0) && (1 < *(int *)(iVar5 + 0xc))) {
          local_150 = 0;
          do {
            if (local_150 == 0) {
              iStack_158 = *(int *)(param_1[0xa1] + 0x1c);
            }
            else if (local_150 == 1) {
              iStack_158 = *(int *)(param_1[0xa1] + 0x20);
            }
            else if (local_150 == 2) {
              iStack_158 = *(int *)(param_1[0xa1] + 0x24);
            }
            piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(iStack_158,0);
            if ((piVar6 == (int *)0x0) || (iVar5 = (**(code **)(*piVar6 + 0x1c0))(), iVar5 < 1)) {
              iStack_158 = 0;
              piStack_154 = (int *)0x0;
              if (local_150 == 0) {
                iVar5 = FUN_00483300_RandomRange(0,1);
                if (iVar5 == 0) {
                  pcVar19 = s_Recluse_005dfe58;
                }
                else {
                  pcVar19 = &DAT_005dfe60;
                }
              }
              else {
                iVar5 = FUN_00483300_RandomRange(0,1);
                pcVar19 = s_Araknid_005dfe74;
                if (iVar5 == 0) {
                  pcVar19 = s_Artikulid_005dfe68;
                }
              }
              uVar11 = 0xffffffff;
              do {
                pcVar15 = pcVar19;
                if (uVar11 == 0) break;
                uVar11 = uVar11 - 1;
                pcVar15 = pcVar19 + 1;
                cVar4 = *pcVar19;
                pcVar19 = pcVar15;
              } while (cVar4 != '\0');
              uVar11 = ~uVar11;
              piVar6 = (int *)(pcVar15 + -uVar11);
              piVar7 = aiStack_114;
              for (uVar12 = uVar11 >> 2; uVar12 != 0; uVar12 = uVar12 - 1) {
                *piVar7 = *piVar6;
                piVar6 = piVar6 + 1;
                piVar7 = piVar7 + 1;
              }
              for (uVar11 = uVar11 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
                *(char *)piVar7 = (char)*piVar6;
                piVar6 = (int *)((int)piVar6 + 1);
                piVar7 = (int *)((int)piVar7 + 1);
              }
              puVar10 = &local_100;
              for (iVar5 = 0xd; iVar5 != 0; iVar5 = iVar5 + -1) {
                *puVar10 = 0;
                puVar10 = puVar10 + 1;
              }
              local_100._2_2_ = FUN_00475210(aiStack_114,0);
              if (-1 < local_100._2_2_) {
                local_f4 = param_1[4];
                iStack_ec = param_1[6];
                uStack_f6 = (undefined2)DAT_00666970;
                iStack_f0 = param_1[5];
                local_100._0_2_ = 0xc;
                iVar5 = (**(code **)(*param_1 + 600))();
                iVar5 = FUN_00483300_RandomRange(0,iVar5 << 3);
                iVar9 = (**(code **)(*param_1 + 600))();
                local_f4 = local_f4 + iVar5 + iVar9 * -4;
                iVar5 = (**(code **)(*param_1 + 600))();
                iVar5 = FUN_00483300_RandomRange(0,iVar5 << 3);
                iVar9 = (**(code **)(*param_1 + 600))();
                iStack_f0 = iStack_f0 + iVar5 + iVar9 * -4;
                iStack_158 = FUN_00450e40_TMapPane_NewObject(&local_100,0xffffffff);
                piStack_154 = (int *)FUN_00452690_TMapPane_GetInstance(iStack_158,0);
              }
              if (local_150 == 0) {
                *(int *)(param_1[0xa1] + 0x1c) = iStack_158;
              }
              else if (local_150 == 1) {
                *(int *)(param_1[0xa1] + 0x20) = iStack_158;
              }
              else if (local_150 == 2) {
                *(int *)(param_1[0xa1] + 0x24) = iStack_158;
              }
              if (piStack_154 != (int *)0x0) {
                iVar5 = FUN_00482fb0(100);
                uStack_4 = 0x30;
                if (iVar5 == 0) {
                  iVar5 = 0;
                }
                else {
                  iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_sleeping_005dfe7c,1);
                }
                iVar9 = *piStack_154;
                uStack_4 = 0xffffffff;
                *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piStack_154 + 0x36);
                iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
                if ((iVar9 == 0) && (iVar5 != 0)) {
                  if (*(int *)(iVar5 + 0x5c) != 0) {
                    FUN_00482f80(*(int *)(iVar5 + 0x5c));
                  }
                  FUN_004830f0(iVar5);
                }
              }
            }
            local_150 = local_150 + 1;
          } while (local_150 < 3);
        }
        iVar5 = param_1[0xa1];
        if ((*(int *)(iVar5 + 8) == 0xa8) && (1 < *(int *)(iVar5 + 0xc))) {
          piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(*(undefined4 *)(iVar5 + 0x1c),0);
          if ((piVar6 != (int *)0x0) &&
             ((piVar6[0x36] != 0 && (iVar5 = FUN_004dab80_TActionBlock_Is(s_sleeping_005dfe88), iVar5 != 0)))) {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0x31;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_sleeping_to_combat_005dfe94,1);
            }
            iVar9 = *piVar6;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar6 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              if (*(int *)(iVar5 + 0x5c) != 0) {
                FUN_00482f80(*(int *)(iVar5 + 0x5c));
              }
              FUN_004830f0(iVar5);
            }
          }
          piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(*(undefined4 *)(param_1[0xa1] + 0x20),0);
          if (((piVar6 != (int *)0x0) && (piVar6[0x36] != 0)) &&
             (iVar5 = FUN_004dab80_TActionBlock_Is(s_sleeping_005dfea8), iVar5 != 0)) {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0x32;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_sleeping_to_combat_005dfeb4,1);
            }
            iVar9 = *piVar6;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar6 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              if (*(int *)(iVar5 + 0x5c) != 0) {
                FUN_00482f80(*(int *)(iVar5 + 0x5c));
              }
              FUN_004830f0(iVar5);
            }
          }
          piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(*(undefined4 *)(param_1[0xa1] + 0x24),0);
          if (((piVar6 != (int *)0x0) && (piVar6[0x36] != 0)) &&
             (iVar5 = FUN_004dab80_TActionBlock_Is(s_sleeping_005dfec8), iVar5 != 0)) {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0x33;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_sleeping_to_combat_005dfed4,1);
            }
            iVar9 = *piVar6;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar6 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              if (*(int *)(iVar5 + 0x5c) != 0) {
                FUN_00482f80(*(int *)(iVar5 + 0x5c));
              }
              FUN_004830f0(iVar5);
            }
          }
        }
        if (*(int *)(param_1[0xa1] + 8) < 1) {
          iVar5 = param_1[0x36];
          if (((iVar5 == 0) || ((*(byte *)(iVar5 + 0x60) & 0x10) == 0)) || (param_1[0x37] == iVar5))
          {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0x34;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_skywalk_to_combat_005dfee8,1);
            }
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            uStack_4 = 0xffffffff;
            (**(code **)(*param_1 + 0x218))(iVar5,0,0);
          }
          iVar5 = FUN_00482fb0(100);
          uStack_4 = 0x35;
          if (iVar5 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_005dfefc,1);
          }
          iVar9 = *param_1;
          uStack_4 = 0xffffffff;
          *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
          iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
          if ((iVar9 == 0) && (iVar5 != 0)) {
            if (*(int *)(iVar5 + 0x5c) != 0) {
              FUN_00482f80(*(int *)(iVar5 + 0x5c));
            }
            FUN_004830f0(iVar5);
          }
          if ((param_1[0x37] != 0) && (iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_005dff04), iVar5 != 0)) {
            (**(code **)(*param_1 + 0x1f8))(param_1[0x37]);
          }
          puVar10 = (undefined4 *)param_1[0xa1];
          puVar10[1] = 0;
          puVar10[0xc] = 1;
          puVar10[5] = 0;
          puVar10[4] = 0x30;
          *puVar10 = 0xf0;
        }
        else {
          iVar5 = FUN_0046dc60_AngleToPP(aiStack_14c + 4,aiStack_14c);
          *(char *)((int)param_1 + 0x36) = (char)iVar5;
          param_1[0x2c] = iVar5;
          iVar5 = param_1[0xa1];
          if (*(int *)(iVar5 + 8) == 0x10) {
            *(undefined4 *)(iVar5 + 0x28) = 1;
          }
          if (*(int *)(iVar5 + 0x28) == 0) {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0x37;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_skywalkf_005dff14,1);
            }
            iVar9 = *param_1;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              if (*(int *)(iVar5 + 0x5c) != 0) {
                FUN_00482f80(*(int *)(iVar5 + 0x5c));
              }
              FUN_004830f0(iVar5);
            }
          }
          else {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0x36;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_skywalk_005dff0c,1);
            }
            iVar9 = *param_1;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            uStack_4 = 0xffffffff;
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              if (*(int *)(iVar5 + 0x5c) != 0) {
                FUN_00482f80(*(int *)(iVar5 + 0x5c));
              }
              FUN_004830f0(iVar5);
            }
            param_1[0x44] = param_1[0x44] & 0xffffffbf;
          }
          if (param_1 == (int *)0x0) {
            ExceptionList = local_c;
            return 1;
          }
          if (param_1[0x36] == 0) {
            ExceptionList = local_c;
            return 1;
          }
          iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_to_skywalk_005dff20);
          if (iVar5 == 0) {
            ExceptionList = local_c;
            return 1;
          }
        }
        param_1[0x44] = param_1[0x44] & 0xffffffbf;
      }
    }
    else {
      puVar10 = (undefined4 *)param_1[0xa1];
      iVar5 = puVar10[3];
      param_1[0x44] = param_1[0x44] & 0xffffffef;
      if (0 < iVar5) {
        *puVar10 = 0xf0;
      }
      puVar10[4] = 0x48;
      puVar10[5] = 1;
      if (puVar10[6] == 0) {
        FUN_004d6a30_SaveMaterials();
        *(undefined4 *)(param_1[0xa1] + 0x18) = 1;
        ExceptionList = local_c;
        return 1;
      }
    }
    ExceptionList = local_c;
    return 1;
  case 3:
    iVar5 = (**(code **)(*param_1 + 0x24))();
    if (iVar5 == 0) {
      (**(code **)(*param_1 + 0x28))();
    }
    piVar6 = (int *)param_1[0xa1];
    if (*piVar6 != 0) {
LAB_004cccfb:
      if ((*piVar6 == 1) && (iVar5 = piVar6[0x10], piVar6[0x10] = iVar5 + -1, iVar5 + -1 == 0)) {
        piVar6[0x10] = 0xf0;
        *piVar6 = 2;
        piVar6 = (int *)param_1[0xa1];
        iVar5 = FUN_00483300_RandomRange(0,2);
        piVar6[0x11] = iVar5;
        uVar11 = param_1[0x44];
        piVar6[0x12] = 0;
        piVar6[0x16] = 0;
        param_1[0x44] = uVar11 | 4;
      }
      if (*piVar6 == 2) {
        piVar6[0x11] = 0;
        switch(piVar6[0x12]) {
        case 0:
          piVar6[0x13] = piVar6[1];
          piVar6[0x14] = piVar6[6];
          piVar6[0x15] = piVar6[0xb];
          break;
        case 1:
          piVar6[0x13] = piVar6[4];
          piVar6[0x14] = piVar6[9];
          piVar6[0x15] = piVar6[0xe];
          break;
        case 2:
          piVar6[0x13] = piVar6[2];
          piVar6[0x14] = piVar6[7];
          piVar6[0x15] = piVar6[0xc];
          break;
        case 3:
          piVar6[0x13] = piVar6[3];
          piVar6[0x14] = piVar6[8];
          piVar6[0x15] = piVar6[0xd];
          break;
        case 4:
          piVar6[0x13] = piVar6[5];
          piVar6[0x14] = piVar6[10];
          piVar6[0x15] = piVar6[0xf];
          break;
        case 5:
          *piVar6 = 1;
          piVar6[0x16] = -1;
        }
        if (piVar6[0x16] == 0) {
          aiStack_14c[2] = param_1[6];
          iVar5 = param_1[4];
          iVar9 = param_1[5];
          puVar10 = &local_100;
          for (iVar14 = 0xd; iVar14 != 0; iVar14 = iVar14 + -1) {
            *puVar10 = 0;
            puVar10 = puVar10 + 1;
          }
          local_100._2_2_ = FUN_00475210(s_jtele_005dff40,0);
          if (-1 < local_100._2_2_) {
            uStack_f6 = (undefined2)DAT_00666970;
            local_100._0_2_ = 0x19;
            iStack_ec = aiStack_14c[2];
            local_f4 = iVar5;
            iStack_f0 = iVar9;
            uVar8 = FUN_00450e40_TMapPane_NewObject(&local_100,0xffffffff);
            FUN_00452690_TMapPane_GetInstance(uVar8,0);
          }
          iVar5 = FUN_00482fb0(100);
          uStack_4 = 0x38;
          if (iVar5 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_cinv2_005dff48,1);
          }
          iVar9 = *param_1;
          *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
          uStack_4 = 0xffffffff;
          iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
          if ((iVar9 == 0) && (iVar5 != 0)) {
            FUN_004d7030();
            FUN_004830f0(iVar5);
          }
        }
        if (*(int *)(param_1[0xa1] + 0x58) == 6) {
          FUN_004111a0(s_cinv2_to_combat_005dff50);
          FUN_004d6ff0();
          iVar5 = param_1[0xa1];
          aiStack_14c[0] = *(int *)(iVar5 + 0x4c);
          aiStack_14c[1] = *(int *)(iVar5 + 0x50);
          aiStack_14c[2] = *(int *)(iVar5 + 0x54);
          (**(code **)(*param_1 + 8))(aiStack_14c,0xffffffff,0);
          FUN_004c9af0(s_jteled_005dff60,aiStack_14c);
        }
        if (*(int *)(param_1[0xa1] + 0x58) == 0x18) {
          FUN_004111a0(s_cinv2_005dff68);
        }
        if (*(int *)(param_1[0xa1] + 0x58) == 0x1e) {
          if (*(int *)(param_1[0xa1] + 0x48) == 4) {
            pcVar19 = s_Priest_Fire_Wind_005dff70;
          }
          else {
            FUN_004d6ff0();
            FUN_004d6ff0();
            FUN_004d7000_GetPos(&iStack_ac);
            if (DAT_00667fcc == (int *)0x0) {
              iVar5 = param_1[0xa1];
              uStack_b8 = *(undefined4 *)(iVar5 + 0x14);
              uStack_b4 = *(undefined4 *)(iVar5 + 0x28);
              uStack_b0 = *(undefined4 *)(iVar5 + 0x3c);
            }
            else {
              FUN_004d7000_GetPos(&uStack_b8);
            }
            uVar8 = FUN_0046dc60_AngleToPP(&iStack_ac,&uStack_b8);
            FUN_004d7020_Face(uVar8);
            pcVar19 = s_Priest_Fireball_005dff84;
          }
          FUN_004d5b90(pcVar19,0,0,0);
        }
        if (*(int *)(param_1[0xa1] + 0x58) == 0x24) {
          FUN_004111a0(s_cinv2_to_combat_005dff94);
        }
        iVar5 = param_1[0xa1];
        if (*(int *)(iVar5 + 0x58) == 0x48) {
          *(int *)(iVar5 + 0x48) = *(int *)(iVar5 + 0x48) + 1;
          *(undefined4 *)(iVar5 + 0x58) = 0;
          iVar5 = FUN_004d7040(s_cinv2_005dffa4);
          if (iVar5 != 0) {
            FUN_004111a0(s_cinv2_to_combat_005dffac);
            ExceptionList = local_c;
            return 1;
          }
        }
        else {
          *(int *)(iVar5 + 0x58) = *(int *)(iVar5 + 0x58) + 1;
        }
      }
      ExceptionList = local_c;
      return 1;
    }
    iVar5 = 0;
    aiStack_14c[0] = 0;
    aiStack_14c[1] = 0;
    aiStack_14c[2] = 0;
    aiStack_14c[3] = 0;
    FUN_0044cf80_TMapIterator_Initialize(0,0,0,0,0xffffffff);
    if (iStack_90 != 0) {
      piVar6 = aiStack_14c;
      do {
        iVar9 = FUN_0059a530_stricmp(**(undefined4 **)(iStack_90 + 0x4c),s_waypoint_005dff34);
        if (iVar9 == 0) {
          iVar5 = iVar5 + 1;
          *piVar6 = iStack_90;
          piVar6 = piVar6 + 1;
          if (iVar5 == 4) break;
        }
        FUN_0044d080_TMapIterator_NextItem();
      } while (iStack_90 != 0);
      if ((((aiStack_14c[0] != 0) && (aiStack_14c[1] != 0)) && (aiStack_14c[2] != 0)) &&
         (aiStack_14c[3] != 0)) {
        piVar6 = aiStack_14c;
        iVar5 = 4;
        puVar10 = (undefined4 *)&stack0xffffff08;
        do {
          iVar9 = *piVar6;
          piVar6 = piVar6 + 1;
          iVar5 = iVar5 + -1;
          puVar10[-2] = *(undefined4 *)(iVar9 + 0x10);
          uVar8 = *(undefined4 *)(iVar9 + 0x18);
          puVar10[-1] = *(undefined4 *)(iVar9 + 0x14);
          *puVar10 = uVar8;
          puVar10 = puVar10 + 3;
        } while (iVar5 != 0);
        iVar5 = 0;
        iVar14 = 0;
        iVar16 = 0;
        iVar9 = 4;
        piVar6 = &iStack_fc;
        do {
          iVar5 = iVar5 + piVar6[-1];
          iVar14 = iVar14 + *piVar6;
          iVar16 = iVar16 + piVar6[1];
          iVar9 = iVar9 + -1;
          piVar6 = piVar6 + 3;
        } while (iVar9 != 0);
        piVar6 = &iStack_fc;
        iVar14 = (int)(iVar14 + (iVar14 >> 0x1f & 3U)) >> 2;
        local_130 = (int)(iVar16 + (iVar16 >> 0x1f & 3U)) >> 2;
        iVar9 = (int)(iVar5 + (iVar5 >> 0x1f & 3U)) >> 2;
        iVar5 = 0;
        do {
          if ((piVar6[-1] < iVar9) && (iVar16 = iVar5, *piVar6 < iVar14)) break;
          iVar5 = iVar5 + 1;
          piVar6 = piVar6 + 3;
          iVar16 = iStack_ac;
        } while (iVar5 < 4);
        iVar5 = 0;
        piVar6 = &iStack_fc;
        do {
          if ((piVar6[-1] < iVar9) && (iVar14 < *piVar6)) {
            iStack_a8 = iVar5;
            break;
          }
          iVar5 = iVar5 + 1;
          piVar6 = piVar6 + 3;
        } while (iVar5 < 4);
        iVar5 = 0;
        piVar6 = &iStack_fc;
        do {
          if ((iVar9 < piVar6[-1]) && (iVar17 = iVar5, *piVar6 < iVar14)) break;
          iVar5 = iVar5 + 1;
          piVar6 = piVar6 + 3;
          iVar17 = iStack_a4;
        } while (iVar5 < 4);
        iVar5 = 0;
        piVar6 = &iStack_fc;
        do {
          if ((iVar9 < piVar6[-1]) && (iVar13 = iVar5, iVar14 < *piVar6)) break;
          iVar5 = iVar5 + 1;
          piVar6 = piVar6 + 3;
          iVar13 = iStack_a0;
        } while (iVar5 < 4);
        piVar6 = (int *)param_1[0xa1];
        piVar6[1] = (&local_100)[iVar16 * 3];
        iVar5 = *(int *)(&stack0xffffff08 + iVar16 * 0xc);
        piVar6[6] = (&iStack_fc)[iVar16 * 3];
        piVar6[0xb] = iVar5;
        piVar6[2] = (&local_100)[iStack_a8 * 3];
        iVar5 = *(int *)(&stack0xffffff08 + iStack_a8 * 0xc);
        piVar6[7] = (&iStack_fc)[iStack_a8 * 3];
        piVar6[0xc] = iVar5;
        piVar6[3] = (&local_100)[iVar17 * 3];
        iVar5 = *(int *)(&stack0xffffff08 + iVar17 * 0xc);
        piVar6[8] = (&iStack_fc)[iVar17 * 3];
        piVar6[0xd] = iVar5;
        iVar5 = (&iStack_fc)[iVar13 * 3];
        iVar16 = *(int *)(&stack0xffffff08 + iVar13 * 0xc);
        piVar6[4] = (&local_100)[iVar13 * 3];
        piVar6[9] = iVar5;
        piVar6[0xe] = iVar16;
        piVar6[5] = iVar9;
        piVar6[10] = iVar14;
        piVar6[0xf] = local_130;
        *piVar6 = 1;
        piVar6[0x10] = 0xf0;
        goto LAB_004cccfb;
      }
    }
    param_1[0xa0] = -1;
    param_1[0x44] = param_1[0x44] & 0xfffffffb;
    ExceptionList = local_c;
    return 1;
  case 4:
    if ((DAT_00667fcc != (int *)0x0) && (iVar5 = (**(code **)(*DAT_00667fcc + 0x2cc))(), iVar5 == 2)
       ) {
      FUN_004d7400_SetFlags(0x80);
    }
    FUN_004d7400_SetFlags(0x10000);
    iVar5 = (**(code **)(*param_1 + 0x24))();
    if (iVar5 == 0) {
      (**(code **)(*param_1 + 0x28))();
    }
    iVar5 = (**(code **)(*param_1 + 0x1c0))();
    iVar9 = (**(code **)(*param_1 + 0x1d8))();
    iVar9 = (iVar5 * 100) / iVar9;
    if (iVar9 < 0x4c) {
      if (iVar9 < 0x1a) goto LAB_004cd10b;
      if (iVar9 < 0x33) goto LAB_004cd0f9;
LAB_004cd110:
      if (0x4b < iVar9) goto LAB_004cd115;
    }
    else {
LAB_004cd0f9:
      if (*(int *)param_1[0xa1] == 1) {
        ((int *)param_1[0xa1])[1] = 1;
      }
LAB_004cd10b:
      if (0x32 < iVar9) goto LAB_004cd110;
LAB_004cd115:
      if (0x19 < iVar9) goto LAB_004cd12b;
    }
    if (*(int *)param_1[0xa1] == 0) {
      ((int *)param_1[0xa1])[2] = 1;
    }
LAB_004cd12b:
    if ((*(int *)(param_1[0xa1] + 4) != 0) && (iVar5 = FUN_004d7040(s_combat_005dffbc), iVar5 != 0))
    {
      FUN_00429990(s_combat_to_walk_005dffc4);
      FUN_004111a0(&DAT_005dffd4);
      iVar5 = FUN_00482fb0(100);
      uStack_4 = 0x39;
      if (iVar5 == 0) {
        puVar10 = (undefined4 *)0x0;
      }
      else {
        puVar10 = (undefined4 *)FUN_004daae0_TActionBlock_CopyCtor(param_1[0x38],&DAT_005dffdc,0);
      }
      iVar5 = *param_1;
      uStack_4 = 0xffffffff;
      *puVar10 = 3;
      (**(code **)(iVar5 + 0x1f8))(puVar10);
      *(undefined4 *)param_1[0xa1] = 0xffffffff;
      FUN_004d7400_SetFlags(4);
      FUN_004d7420_ClearFlags(0x8000);
    }
    if ((*(int *)(param_1[0xa1] + 8) != 0) && (iVar5 = FUN_004d7040(&DAT_005dffe4), iVar5 != 0)) {
      *(uint *)(param_1[0x36] + 0x60) = *(uint *)(param_1[0x36] + 0x60) & 0xffffffef;
      FUN_00429990(s_walk_to_combat_005dffec);
      FUN_004111a0(s_combat_005dfffc);
      puVar10 = (undefined4 *)param_1[0xa1];
      puVar10[2] = 0;
      *puVar10 = 1;
    }
    iVar5 = FUN_004d7040(s_walk_to_combat_005e0004);
    if ((iVar5 == 0) && (iVar5 = FUN_004d7040(s_combat_to_walk_005e0014), iVar5 == 0)) {
      FUN_004d7420_ClearFlags(0x8000);
    }
    else {
      FUN_004d7400_SetFlags(0x8000);
    }
    if (((*(int *)param_1[0xa1] == 1) && (((int *)param_1[0xa1])[2] == 0)) &&
       (iVar5 = FUN_004d7040(s_walk_to_combat_005e0024), iVar5 == 0)) {
      FUN_004d7420_ClearFlags(0x100);
      FUN_004d7400_SetFlags(0x800);
      FUN_004d7420_ClearFlags(0x2000);
      FUN_004d7420_ClearFlags(0x4000);
      FUN_004d7420_ClearFlags(4);
      FUN_004d7400_SetFlags(0x200);
      FUN_004d7420_ClearFlags(0x400);
      iVar5 = FUN_004d7080(s_combat_005e0034);
      if (iVar5 == 0) {
        FUN_004111a0(s_combat_005e003c);
      }
      else {
        FUN_004d7420_ClearFlags(0x8000);
      }
    }
    if (((*(int *)param_1[0xa1] == 0) && (((int *)param_1[0xa1])[1] == 0)) &&
       (iVar5 = FUN_004d7040(s_combat_to_walk_005e0044), iVar5 == 0)) {
      FUN_004d7420_ClearFlags(0x200);
      FUN_004d7400_SetFlags(0x400);
      FUN_004d7420_ClearFlags(4);
      FUN_004d7420_ClearFlags(0x2000);
      FUN_004d7420_ClearFlags(0x4000);
      FUN_004d7400_SetFlags(0x2000);
      FUN_004d7400_SetFlags(0x4000);
      FUN_004d7400_SetFlags(0x100);
      FUN_004d7420_ClearFlags(0x800);
      iVar5 = FUN_004d7080(&DAT_005e0054);
      if (iVar5 == 0) {
        FUN_00429990(&DAT_005e005c);
        iVar5 = FUN_00482fb0(100);
        uStack_4 = 0x3a;
        if (iVar5 == 0) {
          puVar10 = (undefined4 *)0x0;
        }
        else {
          puVar10 = (undefined4 *)FUN_004daae0_TActionBlock_CopyCtor(param_1[0x38],&DAT_005e0064,0);
        }
        iVar5 = *param_1;
        uStack_4 = 0xffffffff;
        *puVar10 = 3;
        (**(code **)(iVar5 + 0x1f8))(puVar10);
        if (param_1[0x37] != 0) {
          puVar1 = (uint *)(param_1[0x37] + 0x60);
          *puVar1 = *puVar1 & 0xffffffef;
        }
        FUN_004111a0(&DAT_005e006c);
      }
      else {
        FUN_004d7420_ClearFlags(0x8000);
      }
    }
    if (((*(int *)param_1[0xa1] == -1) && (iVar5 = FUN_004d7040(&DAT_005e0074), iVar5 != 0)) &&
       ((iVar5 = FUN_004d7060(&DAT_005e007c), iVar5 != 0 &&
        (iVar5 = FUN_004d7080(&DAT_005e0084), iVar5 != 0)))) {
      puVar10 = (undefined4 *)param_1[0xa1];
      puVar10[1] = 0;
      *puVar10 = 0;
      FUN_004d7420_ClearFlags(0x8000);
      FUN_004d7420_ClearFlags(4);
    }
    ExceptionList = local_c;
    return 1;
  default:
    ExceptionList = local_c;
    return 0;
  }
  iVar5 = *(int *)(param_1[0xa1] + 0x34);
  param_1[0x44] = param_1[0x44] | 0x88;
  if (iVar5 == 0) {
    iVar5 = 0;
    FUN_0044cf80_TMapIterator_Initialize(0,0,0,0,0xffffffff);
    while (local_f4 != 0) {
      iVar9 = 0;
      do {
        iVar14 = FUN_0059a530_stricmp(*(undefined4 *)(local_f4 + 0x38),
                              *(undefined4 *)((int)&PTR_DAT_005df54c + iVar9));
        if (iVar14 == 0) {
          iVar5 = iVar5 + 1;
          *(int *)((int)&DAT_0066caac + iVar9) = local_f4;
          if (iVar5 == 0xb) break;
        }
        iVar9 = iVar9 + 4;
      } while (iVar9 < 0x2c);
      FUN_0044d080_TMapIterator_NextItem();
    }
    iVar9 = param_1[0xa1];
    iVar5 = *param_1;
    *(undefined4 *)(iVar9 + 0x34) = 1;
    uVar8 = (**(code **)(iVar5 + 0x1c0))();
    piVar6 = DAT_00667fcc;
    *(undefined4 *)(iVar9 + 0x3c) = uVar8;
    *(undefined4 *)(iVar9 + 0x38) = 0;
    if (piVar6 != (int *)0x0) {
      iVar9 = param_1[0xa1];
      uVar8 = (**(code **)(*piVar6 + 0x1c0))();
      *(undefined4 *)(iVar9 + 0x38) = uVar8;
    }
    if (*(int *)(iVar9 + 0x34) != 0) goto LAB_004c9da7;
  }
  else {
LAB_004c9da7:
    iVar9 = 0;
    local_130 = 0;
    bVar18 = false;
    iVar5 = (**(code **)(*param_1 + 0x1c0))();
    if (DAT_00667fcc != (int *)0x0) {
      iVar9 = (**(code **)(*DAT_00667fcc + 0x1c0))();
    }
    iVar14 = *(int *)(param_1[0xa1] + 0x38);
    if (iVar9 < iVar14) {
      local_130 = 1;
    }
    else if (iVar14 < iVar9) {
      bVar18 = true;
    }
    if (iVar5 < *(int *)(param_1[0xa1] + 0x3c)) {
      bVar18 = true;
    }
    if (DAT_00667fcc == (int *)0x0) {
      iVar5 = 0;
      iVar9 = 0;
      iVar14 = 0;
    }
    else {
      iVar5 = DAT_00667fcc[4];
      iVar9 = DAT_00667fcc[5];
      iVar14 = DAT_00667fcc[6];
    }
    piStack_154 = &DAT_0066caac;
    aiStack_14c[0] = (iVar5 + param_1[4]) / 2;
    aiStack_14c[1] = (param_1[5] + iVar9) / 2;
    aiStack_14c[2] = (param_1[6] + iVar14) / 2;
    do {
      iVar5 = local_130;
      piVar6 = (int *)*piStack_154;
      if (piVar6 != (int *)0x0) {
        iVar9 = (**(code **)(*piVar6 + 0x24))();
        if (iVar9 == 0) {
          (**(code **)(*piVar6 + 0x28))();
        }
        aiStack_14c[4] = piVar6[4];
        local_138 = piVar6[5];
        local_134 = piVar6[6];
        iVar9 = FUN_0046dc60_AngleToPP(aiStack_14c + 4,aiStack_14c);
        *(char *)((int)piVar6 + 0x36) = (char)iVar9;
        piVar6[0x2c] = iVar9;
        if ((piVar6[0x38] == 0) || (iVar9 = FUN_004dab80_TActionBlock_Is(s_watcha_005dfa38), iVar9 == 0)) {
          if ((piVar6[0x38] != 0) && (iVar9 = FUN_004dab80_TActionBlock_Is(s_watchb_005dfa50), iVar9 != 0)) {
            if (iVar5 == 0) {
              if (!bVar18) goto LAB_004ca12d;
              iVar5 = FUN_00482fb0(100);
              uStack_4 = 3;
              if (iVar5 == 0) goto LAB_004ca07c;
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005dfa60,1);
            }
            else {
              iVar5 = FUN_00482fb0(100);
              uStack_4 = 2;
              if (iVar5 == 0) {
LAB_004ca07c:
                iVar5 = 0;
              }
              else {
                iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_cheerb_005dfa58,1);
              }
            }
LAB_004ca07e:
            iVar9 = *piVar6;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar6 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            goto LAB_004ca09d;
          }
          if ((piVar6[0x38] != 0) && (iVar9 = FUN_004dab80_TActionBlock_Is(s_watchc_005dfa68), iVar9 != 0)) {
            if (iVar5 != 0) {
              iVar5 = FUN_00482fb0(100);
              uStack_4 = 4;
              if (iVar5 == 0) goto LAB_004ca07c;
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_cheerc_005dfa70,1);
              goto LAB_004ca07e;
            }
            if (bVar18) {
              iVar5 = FUN_00482fb0(100);
              uStack_4 = 5;
              if (iVar5 == 0) {
                iVar5 = 0;
              }
              else {
                iVar5 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005dfa78,1);
              }
              iVar9 = *piVar6;
              uStack_4 = 0xffffffff;
              *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar6 + 0x36);
              iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
              if ((iVar9 == 0) && (iVar5 != 0)) {
                iVar9 = *(int *)(iVar5 + 0x5c);
                goto joined_r0x004ca119;
              }
            }
          }
        }
        else if (iVar5 == 0) {
          if (bVar18) {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 1;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005dfa48,1);
            }
            iVar9 = *piVar6;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar6 + 0x36);
            uStack_4 = 0xffffffff;
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              iVar9 = *(int *)(iVar5 + 0x5c);
              goto joined_r0x004ca119;
            }
          }
        }
        else {
          iVar5 = FUN_00482fb0(100);
          uStack_4 = 0;
          if (iVar5 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_cheera_005dfa40,1);
          }
          iVar9 = *piVar6;
          uStack_4 = 0xffffffff;
          *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar6 + 0x36);
          iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
LAB_004ca09d:
          if ((iVar9 == 0) && (iVar5 != 0)) {
            iVar9 = *(int *)(iVar5 + 0x5c);
joined_r0x004ca119:
            if (iVar9 != 0) {
              FUN_00482f80(iVar9);
            }
            FUN_004830f0(iVar5);
          }
        }
      }
LAB_004ca12d:
      piStack_154 = piStack_154 + 1;
    } while ((int)piStack_154 < 0x66cad8);
    iVar9 = param_1[0xa1];
    uVar8 = (**(code **)(*param_1 + 0x1c0))();
    piVar6 = DAT_00667fcc;
    *(undefined4 *)(iVar9 + 0x3c) = uVar8;
    if (piVar6 != (int *)0x0) {
      iVar9 = param_1[0xa1];
      uVar8 = (**(code **)(*piVar6 + 0x1c0))();
      *(undefined4 *)(iVar9 + 0x38) = uVar8;
    }
  }
  if (*(int *)(iVar9 + 0xc) != 0) {
    (**(code **)(*param_1 + 0x1c4))(1);
    iVar5 = FUN_00482fb0(100);
    puStack_8 = (undefined1 *)0x6;
    if (iVar5 == 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005dfa80,1);
    }
    iVar9 = *param_1;
    *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
    puStack_8 = (undefined1 *)0xffffffff;
    iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
    if ((iVar9 == 0) && (iVar5 != 0)) {
      if (*(int *)(iVar5 + 0x5c) != 0) {
        FUN_00482f80(*(int *)(iVar5 + 0x5c));
      }
      FUN_004830f0(iVar5);
    }
  }
  iVar5 = (**(code **)(*param_1 + 0x1c0))();
  if (iVar5 < 2) {
    aiStack_114[2] = param_1[6];
    aiStack_114[0] = param_1[4];
    param_1[0x44] = param_1[0x44] | 4;
    aiStack_114[1] = param_1[5];
    iVar5 = param_1[0xa1];
    if (*(int *)(iVar5 + 0xc) == 0) {
      *(undefined4 *)(iVar5 + 0x44) = 0;
      *(undefined4 *)(iVar5 + 0x48) = 0;
      *(undefined4 *)(iVar5 + 0x4c) = 0;
      FUN_0044cf80_TMapIterator_Initialize(0,0,0,0,0xffffffff);
      while (piStack_48 != (int *)0x0) {
        iVar5 = FUN_0059a530_stricmp(piStack_48[0xe],s_ringwall_005dfa88);
        if (iVar5 == 0) {
          if (piStack_48 != (int *)0x0) {
            (**(code **)(*piStack_48 + 0x40))(piStack_48[2] | 0x400000);
          }
          break;
        }
        FUN_0044d080_TMapIterator_NextItem();
      }
      piVar6 = &DAT_0066caac;
      do {
        piVar7 = (int *)*piVar6;
        if (piVar7 != (int *)0x0) {
          iVar5 = FUN_00482fb0(100);
          uStack_4 = 7;
          if (iVar5 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005dfa94,1);
          }
          iVar9 = *piVar7;
          *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar7 + 0x36);
          uStack_4 = 0xffffffff;
          iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
          if ((iVar9 == 0) && (iVar5 != 0)) {
            if (*(int *)(iVar5 + 0x5c) != 0) {
              FUN_00482f80(*(int *)(iVar5 + 0x5c));
            }
            FUN_004830f0(iVar5);
          }
        }
        piVar6 = piVar6 + 1;
      } while ((int)piVar6 < 0x66cad8);
      *(undefined4 *)(param_1[0xa1] + 0xc) = 1;
      FUN_0044cf80_TMapIterator_Initialize(0,0,0,0,0xffffffff);
      do {
        if (iStack_90 == 0) {
LAB_004ca3a3:
          param_1[0xa0] = -1;
          param_1[0x44] = param_1[0x44] & 0xfffffff3;
          ExceptionList = local_c;
          return 1;
        }
        iVar5 = FUN_0059a530_stricmp(**(undefined4 **)(iStack_90 + 0x4c),s_ForBreakTable_005dfa9c);
        if (iVar5 == 0) {
          if (iStack_90 != 0) {
            iVar5 = param_1[0xa1];
            if (((*(int *)(iVar5 + 0x44) == 0) && (*(int *)(iVar5 + 0x48) == 0)) &&
               (*(int *)(iVar5 + 0x4c) == 0)) {
              iVar9 = *(int *)(iStack_90 + 0x14);
              iVar14 = *(int *)(iStack_90 + 0x18);
              *(int *)(iVar5 + 0x44) = *(int *)(iStack_90 + 0x10) + 0x20;
              *(int *)(iVar5 + 0x48) = iVar9 + 0x1b0;
              *(int *)(iVar5 + 0x4c) = iVar14 + 0x58;
            }
            iVar9 = *(int *)(iStack_90 + 0x14);
            uVar8 = *(undefined4 *)(iStack_90 + 0x10);
            uVar2 = *(undefined4 *)(iStack_90 + 0x18);
            uVar3 = *(undefined4 *)(iStack_90 + 0x40);
            *(undefined4 *)(iVar5 + 0x1c) = uVar8;
            *(int *)(iVar5 + 0x20) = iVar9 + 0xa0;
            *(undefined4 *)(iVar5 + 0x24) = uVar2;
            *(undefined4 *)(iVar5 + 0x10) = uVar8;
            *(int *)(iVar5 + 0x14) = iVar9 + 0x15e;
            *(undefined4 *)(iVar5 + 0x18) = uVar2;
            *(undefined4 *)(iVar5 + 0x28) = uVar3;
            *(uint *)(param_1[0x36] + 0x60) = *(uint *)(param_1[0x36] + 0x60) & 0xffffffef;
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 8;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_to_fakedead_005dfaac,1);
            }
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            uStack_4 = 0xffffffff;
            (**(code **)(*param_1 + 0x218))(iVar5,0,0);
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 9;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_fakedead_005dfac0,1);
            }
            iVar9 = *param_1;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              if (*(int *)(iVar5 + 0x5c) != 0) {
                FUN_00482f80(*(int *)(iVar5 + 0x5c));
              }
              FUN_004830f0(iVar5);
            }
            FUN_004538d0(param_1,4);
            iVar5 = param_1[0xa1];
            *(undefined4 *)(iVar5 + 0x40) = 1;
            break;
          }
          goto LAB_004ca3a3;
        }
        FUN_0044d080_TMapIterator_NextItem();
      } while( true );
    }
    if (*(int *)(iVar5 + 0xc) != 1) goto LAB_004cb3f2;
    if (DAT_00667fcc != (int *)0x0) {
      FUN_004d3fd0_EndFighting();
    }
    FUN_0047c580(0);
    iVar5 = param_1[0xa1];
    iStack_12c = *(int *)(iVar5 + 0x10);
    iStack_128 = *(int *)(iVar5 + 0x14);
    uStack_124 = *(undefined4 *)(iVar5 + 0x18);
    iVar5 = (**(code **)(*param_1 + 0x24))();
    if (iVar5 == 0) {
      (**(code **)(*param_1 + 0x28))();
    }
    iVar9 = FUN_0046dc60_AngleToPP(aiStack_114,&iStack_12c);
    iVar5 = param_1[0xa1];
    *(char *)((int)param_1 + 0x36) = (char)iVar9;
    param_1[0x2c] = iVar9;
    if (*(int *)(iVar5 + 0x50) == 0) {
      iStack_120 = *(int *)(iVar5 + 0x44);
      iStack_11c = *(int *)(iVar5 + 0x48);
      uStack_118 = *(undefined4 *)(iVar5 + 0x4c);
    }
    else {
      iStack_120 = *(int *)(iVar5 + 0x1c);
      iStack_11c = *(int *)(iVar5 + 0x20);
      uStack_118 = *(undefined4 *)(iVar5 + 0x24);
    }
    if (DAT_00667fcc != (int *)0x0) {
      aiStack_14c[4] = DAT_00667fcc[4];
      local_138 = DAT_00667fcc[5];
      local_134 = DAT_00667fcc[6];
    }
    if (*(int *)(iVar5 + 0x50) == 0) {
      iVar9 = aiStack_14c[4] - iStack_120;
      if (iVar9 < 0) {
        iVar9 = iStack_120 - aiStack_14c[4];
      }
      iVar14 = local_138 - iStack_11c;
      if (iVar14 < 0) {
        iVar14 = iStack_11c - local_138;
      }
      iVar16 = iVar9;
      if (iVar14 <= iVar9) {
        iVar16 = iVar14;
      }
      if ((iVar14 - (iVar16 >> 1)) + iVar9 < 0x20) {
        *(undefined4 *)(iVar5 + 0x50) = 1;
        iStack_120 = 0;
        iStack_11c = 0;
        uStack_118 = 0;
      }
    }
    iVar9 = aiStack_14c[4] - iStack_120;
    if (iVar9 < 0) {
      iVar9 = iStack_120 - aiStack_14c[4];
    }
    iVar14 = local_138 - iStack_11c;
    if (iVar14 < 0) {
      iVar14 = iStack_11c - local_138;
    }
    iVar16 = iVar9;
    if (iVar14 <= iVar9) {
      iVar16 = iVar14;
    }
    if ((iVar14 - (iVar16 >> 1)) + iVar9 < 0x20) {
      if (DAT_00667fcc != (int *)0x0) {
        (**(code **)(*DAT_00667fcc + 0xc))(&iStack_120);
        FUN_004cee70_Stop(0);
        piVar6 = DAT_00667fcc;
        iVar5 = FUN_00482fb0(100);
        puStack_8 = (undefined1 *)0xa;
        if (iVar5 == 0) {
          iVar5 = 0;
        }
        else {
          iVar5 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005dfacc,1);
        }
        iVar9 = *piVar6;
        *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar6 + 0x36);
        puStack_8 = (undefined1 *)0xffffffff;
        iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
        if ((iVar9 == 0) && (iVar5 != 0)) {
          if (*(int *)(iVar5 + 0x5c) != 0) {
            FUN_00482f80(*(int *)(iVar5 + 0x5c));
          }
          FUN_004830f0(iVar5);
        }
        cVar4 = FUN_0046dc60_AngleToPP(aiStack_14c + 4,aiStack_114);
        piVar6 = DAT_00667fcc;
        *(byte *)((int)DAT_00667fcc + 0x36) = cVar4 + 0x80U;
        piVar6[0x2c] = (uint)(byte)(cVar4 + 0x80U);
      }
      iVar5 = aiStack_114[0] - iStack_12c;
      if (iVar5 < 0) {
        iVar5 = iStack_12c - aiStack_114[0];
      }
      iVar9 = aiStack_114[1] - iStack_128;
      if (iVar9 < 0) {
        iVar9 = iStack_128 - aiStack_114[1];
      }
      iVar14 = iVar5;
      if (iVar9 <= iVar5) {
        iVar14 = iVar9;
      }
      iVar5 = (iVar9 - (iVar14 >> 1)) + iVar5;
      if (iVar5 < 0x80) {
        *(undefined4 *)(param_1[0xa1] + 0x54) = 1000;
      }
      if (iVar5 < 0x20) {
        iVar5 = FUN_00452690_TMapPane_GetInstance(*(undefined4 *)(param_1[0xa1] + 0x28),0);
        if (iVar5 == 0) {
          aiStack_14c[0] = 0;
          aiStack_14c[1] = 0;
          aiStack_14c[2] = 0;
        }
        else {
          aiStack_14c[0] = *(int *)(iVar5 + 0x10);
          aiStack_14c[1] = *(int *)(iVar5 + 0x14);
          aiStack_14c[2] = *(int *)(iVar5 + 0x18);
        }
        iVar5 = FUN_0046dc60_AngleToPP(aiStack_114,aiStack_14c);
        *(char *)((int)param_1 + 0x36) = (char)iVar5;
        param_1[0x2c] = iVar5;
        (**(code **)(*param_1 + 0xc))(&iStack_12c);
        iVar5 = FUN_00482fb0(100);
        puStack_8 = (undefined1 *)0xb;
        if (iVar5 == 0) {
          iVar9 = 0;
        }
        else {
          iVar9 = FUN_004da9f0_TActionBlock_Ctor1(s_walkf_005dfad4,1);
        }
        iVar14 = *param_1;
        *(uint *)(iVar9 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
        puStack_8 = (undefined1 *)0xffffffff;
        iVar14 = (**(code **)(iVar14 + 0x208))(iVar9,0);
        if ((iVar14 == 0) && (iVar9 != 0)) {
          if (*(int *)(iVar9 + 0x5c) != 0) {
            FUN_00482f80(*(int *)(iVar9 + 0x5c));
          }
          FUN_004830f0(iVar9);
        }
        FUN_004cee70_Stop(0);
        iVar9 = FUN_0046dc60_AngleToPP(aiStack_14c + 1,&iStack_120);
        piVar6 = DAT_00667fcc;
        if (DAT_00667fcc != (int *)0x0) {
          *(char *)((int)DAT_00667fcc + 0x36) = (char)iVar9;
          piVar6[0x2c] = iVar9;
        }
        iVar9 = param_1[0x36];
        if (((iVar9 == 0) || ((*(byte *)(iVar9 + 0x60) & 0x10) == 0)) || (param_1[0x37] == iVar9)) {
          iVar9 = FUN_00482fb0(100);
          uStack_10 = 0x1d;
          if (iVar9 == 0) {
            iVar9 = 0;
          }
          else {
            iVar9 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_to_cdead_005dfc88,1);
          }
          iVar14 = *param_1;
          *(uint *)(iVar9 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
          uStack_10 = 0xffffffff;
          (**(code **)(iVar14 + 0x218))(iVar9,0,0);
        }
        iVar9 = FUN_00482fb0(100);
        uStack_10 = 0x1e;
        if (iVar9 == 0) {
          iVar9 = 0;
        }
        else {
          iVar9 = FUN_004da9f0_TActionBlock_Ctor1(s_cdead_005dfc98,1);
        }
        iVar14 = *param_1;
        uStack_10 = 0xffffffff;
        *(uint *)(iVar9 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
        iVar14 = (**(code **)(iVar14 + 0x208))(iVar9,0);
        if ((iVar14 == 0) && (iVar9 != 0)) {
          if (*(int *)(iVar9 + 0x5c) != 0) {
            FUN_00482f80(*(int *)(iVar9 + 0x5c));
          }
          FUN_004830f0(iVar9);
        }
        if (((param_1 != (int *)0x0) && (param_1[0x36] != 0)) &&
           (iVar9 = FUN_004dab80_TActionBlock_Is(s_combat_to_cdead_005dfca0), iVar9 != 0)) {
          iVar9 = param_1[0xa1];
          *(undefined4 *)(iVar9 + 0xc) = 2;
          *(undefined4 *)(iVar9 + 0x2c) = 0;
          FUN_0046e8a0();
          uVar8 = (**(code **)(*param_1 + 0x138))(s_combat_to_cdead_005dfcb0,0xffffffff);
          uVar8 = (**(code **)(iVar5 + 0x90))(uVar8);
          *(undefined4 *)(param_1[0xa1] + 0x30) = uVar8;
        }
      }
      else {
        iVar5 = FUN_0046dc60_AngleToPP(aiStack_114,&iStack_12c);
        FUN_004cedb0(iStack_12c,iStack_128,0);
        *(char *)((int)param_1 + 0x36) = (char)iVar5;
        param_1[0x2c] = iVar5;
        if (param_1 != (int *)0x0) {
          if ((param_1[0x36] != 0) &&
             (iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_to_fakedead_005dfadc), iVar5 != 0)) goto LAB_004cb3e1;
          if ((param_1[0x36] != 0) && (iVar5 = FUN_004dab80_TActionBlock_Is(s_fakedead_005dfaf0), iVar5 != 0)) {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0xc;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_fakedead_to_walk_005dfafc,1);
            }
            iVar9 = *param_1;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            uStack_4 = 0xffffffff;
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              if (*(int *)(iVar5 + 0x5c) != 0) {
                FUN_00482f80(*(int *)(iVar5 + 0x5c));
              }
              FUN_004830f0(iVar5);
            }
          }
          if ((param_1[0x36] != 0) &&
             (iVar5 = FUN_004dab80_TActionBlock_Is(s_fakedead_to_walk_005dfb10), iVar5 != 0)) {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0xd;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005dfb24,1);
            }
            iVar9 = *param_1;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              if (*(int *)(iVar5 + 0x5c) != 0) {
                FUN_00482f80(*(int *)(iVar5 + 0x5c));
              }
              FUN_004830f0(iVar5);
            }
          }
          if (((param_1[0x36] != 0) && (iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_005dfb2c), iVar5 != 0)) &&
             (*(int *)(param_1[0xa1] + 0x54) == 0)) {
            iVar5 = param_1[0x36];
            if (((iVar5 == 0) || ((*(byte *)(iVar5 + 0x60) & 0x10) == 0)) ||
               (param_1[0x37] == iVar5)) {
              iVar5 = FUN_00482fb0(100);
              uStack_4 = 0xe;
              if (iVar5 == 0) {
                iVar5 = 0;
              }
              else {
                iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_to_fakedead_005dfb34,1);
              }
              iVar9 = *param_1;
              *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
              uStack_4 = 0xffffffff;
              (**(code **)(iVar9 + 0x218))(iVar5,0,0);
            }
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0xf;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_fakedead_005dfb48,1);
            }
            iVar9 = *param_1;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              if (*(int *)(iVar5 + 0x5c) != 0) {
                FUN_00482f80(*(int *)(iVar5 + 0x5c));
              }
              FUN_004830f0(iVar5);
            }
          }
        }
        if (*(int *)(param_1[0xa1] + 0x54) == 0) {
          iVar5 = FUN_00482fb0(100);
          uStack_4 = 0x11;
          if (iVar5 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combatf_005dfb64,1);
          }
          iVar9 = *param_1;
          uStack_4 = 0xffffffff;
          *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
          iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
          if ((iVar9 == 0) && (iVar5 != 0)) {
            iVar9 = *(int *)(iVar5 + 0x5c);
joined_r0x004cac51:
            if (iVar9 != 0) {
              FUN_00482f80(iVar9);
            }
            FUN_004830f0(iVar5);
          }
        }
        else if (((param_1 != (int *)0x0) && (param_1[0x36] != 0)) &&
                (iVar5 = FUN_004dab80_TActionBlock_Is(&DAT_005dfb54), iVar5 != 0)) {
          iVar5 = FUN_00482fb0(100);
          uStack_4 = 0x10;
          if (iVar5 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_walkf_005dfb5c,1);
          }
          iVar9 = *param_1;
          *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
          uStack_4 = 0xffffffff;
          iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
          if ((iVar9 == 0) && (iVar5 != 0)) {
            iVar9 = *(int *)(iVar5 + 0x5c);
            goto joined_r0x004cac51;
          }
        }
      }
    }
    else {
      *(undefined4 *)(iVar5 + 0x54) = 0x30;
      if (*(int *)(iVar5 + 0x40) == 0) {
        if (((param_1 == (int *)0x0) || (param_1[0x36] == 0)) ||
           (iVar5 = FUN_004dab80_TActionBlock_Is(s_fakedead_005dfbe4), iVar5 == 0)) {
          iVar5 = FUN_0046dc60_AngleToPP(aiStack_114,&iStack_12c);
          FUN_004cedb0(iStack_12c,iStack_128,0);
          *(char *)((int)param_1 + 0x36) = (char)iVar5;
          param_1[0x2c] = iVar5;
          if (param_1 != (int *)0x0) {
            if ((param_1[0x36] != 0) &&
               (iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_to_fakedead_005dfbf0), iVar5 != 0)) goto LAB_004cb253;
            if ((param_1[0x36] != 0) && (iVar5 = FUN_004dab80_TActionBlock_Is(s_fakedead_005dfc04), iVar5 != 0)) {
              iVar5 = FUN_00482fb0(100);
              uStack_4 = 0x16;
              if (iVar5 == 0) {
                iVar5 = 0;
              }
              else {
                iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_fakedead_to_walk_005dfc10,1);
              }
              iVar9 = *param_1;
              uStack_4 = 0xffffffff;
              *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
              iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
              if ((iVar9 == 0) && (iVar5 != 0)) {
                if (*(int *)(iVar5 + 0x5c) != 0) {
                  FUN_00482f80(*(int *)(iVar5 + 0x5c));
                }
                FUN_004830f0(iVar5);
              }
            }
            if ((param_1[0x36] != 0) &&
               (iVar5 = FUN_004dab80_TActionBlock_Is(s_fakedead_to_walk_005dfc24), iVar5 != 0)) {
              iVar5 = FUN_00482fb0(100);
              uStack_4 = 0x17;
              if (iVar5 == 0) {
                iVar5 = 0;
              }
              else {
                iVar5 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005dfc38,1);
              }
              iVar9 = *param_1;
              *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
              uStack_4 = 0xffffffff;
              iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
              if ((iVar9 == 0) && (iVar5 != 0)) {
                if (*(int *)(iVar5 + 0x5c) != 0) {
                  FUN_00482f80(*(int *)(iVar5 + 0x5c));
                }
                FUN_004830f0(iVar5);
              }
            }
            if (((param_1[0x36] != 0) && (iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_005dfc40), iVar5 != 0)) &&
               (*(int *)(param_1[0xa1] + 0x54) != 0)) {
              iVar5 = param_1[0x36];
              if (((iVar5 == 0) || ((*(byte *)(iVar5 + 0x60) & 0x10) == 0)) ||
                 (param_1[0x37] == iVar5)) {
                iVar5 = FUN_00482fb0(100);
                uStack_4 = 0x18;
                if (iVar5 == 0) {
                  iVar5 = 0;
                }
                else {
                  iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_to_fakedead_005dfc48,1);
                }
                iVar9 = *param_1;
                *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
                uStack_4 = 0xffffffff;
                (**(code **)(iVar9 + 0x218))(iVar5,0,0);
              }
              iVar5 = FUN_00482fb0(100);
              uStack_4 = 0x19;
              if (iVar5 == 0) {
                iVar5 = 0;
              }
              else {
                iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_fakedead_005dfc5c,1);
              }
              iVar9 = *param_1;
              uStack_4 = 0xffffffff;
              *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
              iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
              if ((iVar9 == 0) && (iVar5 != 0)) {
                if (*(int *)(iVar5 + 0x5c) != 0) {
                  FUN_00482f80(*(int *)(iVar5 + 0x5c));
                }
                FUN_004830f0(iVar5);
              }
            }
          }
          if (*(int *)(param_1[0xa1] + 0x54) == 0) {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0x1b;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combatf_005dfc78,1);
            }
            iVar9 = *param_1;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              iVar9 = *(int *)(iVar5 + 0x5c);
joined_r0x004cb23f:
              if (iVar9 != 0) {
                FUN_00482f80(iVar9);
              }
              FUN_004830f0(iVar5);
            }
          }
          else if (((param_1 != (int *)0x0) && (param_1[0x36] != 0)) &&
                  (iVar5 = FUN_004dab80_TActionBlock_Is(&DAT_005dfc68), iVar5 != 0)) {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0x1a;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_walkf_005dfc70,1);
            }
            iVar9 = *param_1;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            uStack_4 = 0xffffffff;
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              iVar9 = *(int *)(iVar5 + 0x5c);
              goto joined_r0x004cb23f;
            }
          }
        }
      }
      else {
        if (((param_1 == (int *)0x0) || (param_1[0x36] == 0)) ||
           (iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_to_fakedead_005dfb6c), iVar5 == 0)) {
          iVar5 = param_1[0x36];
          if (((iVar5 == 0) || ((*(byte *)(iVar5 + 0x60) & 0x10) == 0)) || (param_1[0x37] == iVar5))
          {
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0x13;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_fakedead_005dfb8c,1);
            }
            iVar9 = *param_1;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            uStack_4 = 0xffffffff;
            (**(code **)(iVar9 + 0x218))(iVar5,0,0);
          }
          if ((param_1 == (int *)0x0) ||
             (((param_1[0x36] == 0 ||
               (iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_to_fakedead_005dfb98), iVar5 == 0)) &&
              ((param_1[0x36] == 0 || (iVar5 = FUN_004dab80_TActionBlock_Is(s_fakedead_005dfbac), iVar5 == 0)))))) {
            iVar5 = param_1[0x36];
            if (((iVar5 == 0) || ((*(byte *)(iVar5 + 0x60) & 0x10) == 0)) ||
               (param_1[0x37] == iVar5)) {
              iVar5 = FUN_00482fb0(100);
              uStack_4 = 0x14;
              if (iVar5 == 0) {
                iVar5 = 0;
              }
              else {
                iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_to_fakedead_005dfbb8,1);
              }
              iVar9 = *param_1;
              *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
              uStack_4 = 0xffffffff;
              (**(code **)(iVar9 + 0x218))(iVar5,0,0);
            }
            iVar5 = FUN_00482fb0(100);
            uStack_4 = 0x15;
            if (iVar5 == 0) {
              iVar5 = 0;
            }
            else {
              iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_fakedead_005dfbcc,1);
            }
            iVar9 = *param_1;
            uStack_4 = 0xffffffff;
            *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
            iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
            if ((iVar9 == 0) && (iVar5 != 0)) {
              iVar9 = *(int *)(iVar5 + 0x5c);
              goto joined_r0x004cae75;
            }
          }
        }
        else {
          iVar5 = FUN_00482fb0(100);
          uStack_4 = 0x12;
          if (iVar5 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_fakedead_005dfb80,1);
          }
          iVar9 = *param_1;
          uStack_4 = 0xffffffff;
          *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
          iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
          if ((iVar9 == 0) && (iVar5 != 0)) {
            iVar9 = *(int *)(iVar5 + 0x5c);
joined_r0x004cae75:
            if (iVar9 != 0) {
              FUN_00482f80(iVar9);
            }
            FUN_004830f0(iVar5);
          }
        }
        if (((param_1 != (int *)0x0) && (param_1[0x36] != 0)) &&
           (iVar5 = FUN_004dab80_TActionBlock_Is(s_fakedead_005dfbd8), iVar5 != 0)) {
          *(undefined4 *)(param_1[0xa1] + 0x40) = 0;
        }
      }
LAB_004cb253:
      piVar6 = DAT_00667fcc;
      if (DAT_00667fcc != (int *)0x0) {
        iVar5 = FUN_00482fb0(100);
        uStack_4 = 0x1c;
        if (iVar5 == 0) {
          iVar5 = 0;
        }
        else {
          iVar5 = FUN_004da9f0_TActionBlock_Ctor1(&DAT_005dfc80,1);
        }
        iVar9 = *piVar6;
        uStack_4 = 0xffffffff;
        *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar6 + 0x36);
        iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
        if ((iVar9 == 0) && (iVar5 != 0)) {
          if (*(int *)(iVar5 + 0x5c) != 0) {
            FUN_00482f80(*(int *)(iVar5 + 0x5c));
          }
          FUN_004830f0(iVar5);
        }
        FUN_004cedb0(iStack_120,iStack_11c,0);
      }
    }
LAB_004cb3e1:
    iVar5 = param_1[0xa1];
    if (*(int *)(iVar5 + 0x54) != 0) {
      *(int *)(iVar5 + 0x54) = *(int *)(iVar5 + 0x54) + -1;
    }
LAB_004cb3f2:
    piVar6 = DAT_00667fcc;
    if (*(int *)(iVar5 + 0xc) == 2) {
      if ((*(int *)(iVar5 + 0x2c) == 4) && (DAT_00667fcc != (int *)0x0)) {
        iVar5 = DAT_00667fcc[0x36];
        if ((iVar5 == 0) ||
           (((*(byte *)(iVar5 + 0x60) & 0x10) == 0 || (DAT_00667fcc[0x37] == iVar5)))) {
          iVar5 = FUN_00482fb0(100);
          uStack_4 = 0x1f;
          if (iVar5 == 0) {
            iVar5 = 0;
          }
          else {
            iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_sunsetflipper_005dfcc0,1);
          }
          *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)piVar6 + 0x36);
          uStack_4 = 0xffffffff;
          (**(code **)(*piVar6 + 0x218))(iVar5,0,0);
        }
        DAT_00667fcc[0x44] = DAT_00667fcc[0x44] | 0x8000;
        param_1[0x44] = param_1[0x44] | 0x8000;
      }
      iVar5 = param_1[0xa1];
      if (*(int *)(iVar5 + 0x2c) == (*(int *)(iVar5 + 0x30) * 0x35) / 0x96) {
        piVar6 = (int *)FUN_00452690_TMapPane_GetInstance(*(undefined4 *)(iVar5 + 0x28),0);
        if (piVar6 != (int *)0x0) {
          (**(code **)(*piVar6 + 0x18))(1);
        }
        FUN_004c8500_EffectCombatFlash();
      }
      piVar6 = DAT_00667fcc;
      iVar5 = param_1[0xa1];
      if (*(int *)(iVar5 + 0x30) <= *(int *)(iVar5 + 0x2c)) {
        *(undefined4 *)(iVar5 + 0xc) = 3;
        piVar6[0x44] = piVar6[0x44] & 0xffff7fff;
        param_1[0x44] = param_1[0x44] & 0xffff7fff;
      }
      *(int *)(iVar5 + 0x2c) = *(int *)(iVar5 + 0x2c) + 1;
    }
    if (*(int *)(iVar5 + 0xc) != 3) {
      ExceptionList = local_c;
      return 1;
    }
    iVar5 = param_1[0x36];
    if (((iVar5 == 0) || ((*(byte *)(iVar5 + 0x60) & 0x10) == 0)) || (param_1[0x37] == iVar5)) {
      iVar5 = FUN_00482fb0(100);
      uStack_4 = 0x20;
      if (iVar5 == 0) {
        iVar5 = 0;
      }
      else {
        iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_cdead_005dfcd0,1);
      }
      *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
      uStack_4 = 0xffffffff;
      (**(code **)(*param_1 + 0x218))(iVar5,0,0);
    }
    param_1[0x44] = param_1[0x44] & 0xfffffff7;
    (**(code **)(*param_1 + 0x1c4))(0);
    FUN_004538d0(DAT_00667fcc,4);
    ExceptionList = local_c;
    return 1;
  }
  iVar5 = (**(code **)(*param_1 + 0x1c0))();
  if (iVar5 < 2) {
    ExceptionList = local_c;
    return 1;
  }
  iVar5 = (**(code **)(*param_1 + 0x1c0))();
  iVar9 = (**(code **)(*param_1 + 0x1d8))();
  iVar9 = (iVar5 * 100) / iVar9;
  if ((iVar9 < 0x51) && (puVar10 = (undefined4 *)param_1[0xa1], puVar10[2] == 0)) {
    puVar10[2] = 0x50;
    puVar10[1] = 0x48;
    *puVar10 = 1;
    iVar5 = FUN_00482fb0(100);
    uStack_4 = 0x21;
    if (iVar5 == 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_to_stunned_005dfcd8,1);
    }
    iVar14 = *param_1;
    uStack_4 = 0xffffffff;
    *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
    iVar14 = (**(code **)(iVar14 + 0x208))(iVar5,0);
    if ((iVar14 == 0) && (iVar5 != 0)) {
      if (*(int *)(iVar5 + 0x5c) != 0) {
        FUN_00482f80(*(int *)(iVar5 + 0x5c));
      }
      FUN_004830f0(iVar5);
    }
  }
  if ((iVar9 < 0x3d) && (puVar10 = (undefined4 *)param_1[0xa1], puVar10[2] == 0x50)) {
    puVar10[2] = 0x3c;
    puVar10[1] = 0x48;
    *puVar10 = 1;
    iVar5 = FUN_00482fb0(100);
    uStack_4 = 0x22;
    if (iVar5 == 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_to_stunned_005dfcec,1);
    }
    iVar14 = *param_1;
    *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
    uStack_4 = 0xffffffff;
    iVar14 = (**(code **)(iVar14 + 0x208))(iVar5,0);
    if ((iVar14 == 0) && (iVar5 != 0)) {
      if (*(int *)(iVar5 + 0x5c) != 0) {
        FUN_00482f80(*(int *)(iVar5 + 0x5c));
      }
      FUN_004830f0(iVar5);
    }
  }
  if ((iVar9 < 0x29) && (puVar10 = (undefined4 *)param_1[0xa1], puVar10[2] == 0x3c)) {
    puVar10[2] = 0x28;
    puVar10[1] = 0x48;
    *puVar10 = 1;
    iVar5 = FUN_00482fb0(100);
    uStack_4 = 0x23;
    if (iVar5 == 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_to_stunned_005dfd00,1);
    }
    iVar14 = *param_1;
    uStack_4 = 0xffffffff;
    *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
    iVar14 = (**(code **)(iVar14 + 0x208))(iVar5,0);
    if ((iVar14 == 0) && (iVar5 != 0)) {
      if (*(int *)(iVar5 + 0x5c) != 0) {
        FUN_00482f80(*(int *)(iVar5 + 0x5c));
      }
      FUN_004830f0(iVar5);
    }
  }
  if ((iVar9 < 0x15) && (puVar10 = (undefined4 *)param_1[0xa1], puVar10[2] == 0x28)) {
    puVar10[2] = 0x14;
    puVar10[1] = 0x48;
    *puVar10 = 1;
    iVar5 = FUN_00482fb0(100);
    uStack_4 = 0x24;
    if (iVar5 == 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_to_stunned_005dfd14,1);
    }
    iVar9 = *param_1;
    uStack_4 = 0xffffffff;
    *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
    iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
    if ((iVar9 == 0) && (iVar5 != 0)) {
      if (*(int *)(iVar5 + 0x5c) != 0) {
        FUN_00482f80(*(int *)(iVar5 + 0x5c));
      }
      FUN_004830f0(iVar5);
    }
  }
  if (*(int *)param_1[0xa1] == 0) {
    if (param_1 == (int *)0x0) {
      ExceptionList = local_c;
      return 1;
    }
    if (param_1[0x36] == 0) {
      ExceptionList = local_c;
      return 1;
    }
    iVar5 = FUN_004dab80_TActionBlock_Is(s_stunned_005dfd74);
    if (iVar5 == 0) {
      ExceptionList = local_c;
      return 1;
    }
    iVar5 = FUN_00482fb0(100);
    uStack_4 = 0x28;
    if (iVar5 == 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_stunned_to_combat_005dfd7c,1);
    }
    iVar9 = *param_1;
    uStack_4 = 0xffffffff;
    *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
    iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
    if (iVar9 != 0) {
      ExceptionList = local_c;
      return 1;
    }
    if (iVar5 == 0) {
      ExceptionList = local_c;
      return 1;
    }
    iVar9 = *(int *)(iVar5 + 0x5c);
    goto joined_r0x004cbaff;
  }
  param_1[0x44] = param_1[0x44] | 4;
  FUN_004d4790_SetFighting(0);
  if (param_1 == (int *)0x0) {
LAB_004cb9e4:
    *(int *)(param_1[0xa1] + 4) = *(int *)(param_1[0xa1] + 4) + -1;
  }
  else if ((param_1[0x36] == 0) || (iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_005dfd28), iVar5 == 0)) {
    if ((param_1[0x36] == 0) || (iVar5 = FUN_004dab80_TActionBlock_Is(s_combat_to_stunned_005dfd44), iVar5 == 0))
    goto LAB_004cb9e4;
    iVar5 = FUN_00482fb0(100);
    uStack_4 = 0x26;
    if (iVar5 == 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_stunned_005dfd58,1);
    }
    iVar9 = *param_1;
    uStack_4 = 0xffffffff;
    *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
    iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
    if ((iVar9 == 0) && (iVar5 != 0)) {
      iVar9 = *(int *)(iVar5 + 0x5c);
      goto joined_r0x004cb9d9;
    }
  }
  else {
    iVar5 = FUN_00482fb0(100);
    uStack_4 = 0x25;
    if (iVar5 == 0) {
      iVar5 = 0;
    }
    else {
      iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_combat_to_stunned_005dfd30,1);
    }
    iVar9 = *param_1;
    *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
    uStack_4 = 0xffffffff;
    iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
    if ((iVar9 == 0) && (iVar5 != 0)) {
      iVar9 = *(int *)(iVar5 + 0x5c);
joined_r0x004cb9d9:
      if (iVar9 != 0) {
        FUN_00482f80(iVar9);
      }
      FUN_004830f0(iVar5);
    }
  }
  puVar10 = (undefined4 *)param_1[0xa1];
  if (0 < (int)puVar10[1]) {
    ExceptionList = local_c;
    return 1;
  }
  uVar11 = param_1[0x44];
  *puVar10 = 0;
  param_1[0x44] = uVar11 & 0xfffffffb;
  puVar10[1] = 0;
  iVar5 = FUN_00482fb0(100);
  uStack_4 = 0x27;
  if (iVar5 == 0) {
    iVar5 = 0;
  }
  else {
    iVar5 = FUN_004da9f0_TActionBlock_Ctor1(s_stunned_to_combat_005dfd60,1);
  }
  iVar9 = *param_1;
  uStack_4 = 0xffffffff;
  *(uint *)(iVar5 + 0x2c) = (uint)*(byte *)((int)param_1 + 0x36);
  iVar9 = (**(code **)(iVar9 + 0x208))(iVar5,0);
  if (iVar9 != 0) {
    ExceptionList = local_c;
    return 1;
  }
  if (iVar5 == 0) {
    ExceptionList = local_c;
    return 1;
  }
  iVar9 = *(int *)(iVar5 + 0x5c);
joined_r0x004cbaff:
  if (iVar9 != 0) {
    FUN_00482f80(iVar9);
  }
  FUN_004830f0(iVar5);
  ExceptionList = local_c;
  return 1;
}


