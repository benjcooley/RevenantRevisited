// FUN_0041a4e0_TArea_Load @ 0041a4e0 size=3019

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall FUN_0041a4e0_TArea_Load(char *param_1,char *param_2,int param_3)

{
  char *pcVar1;
  uint *puVar2;
  int *piVar3;
  char cVar4;
  short sVar5;
  byte *pbVar6;
  int iVar7;
  undefined4 *puVar8;
  int iVar9;
  undefined4 *puVar10;
  uint uVar11;
  char *pcVar12;
  uint uVar13;
  int iVar14;
  char *pcVar15;
  undefined4 *puVar16;
  undefined4 *puVar17;
  bool bVar18;
  char *pcVar19;
  undefined4 *local_154;
  int local_150;
  int local_14c;
  int local_148;
  int local_144;
  int local_140;
  int local_13c;
  int local_138;
  int local_134;
  int local_130;
  int local_12c;
  int local_128;
  int local_124;
  int local_120;
  undefined4 local_11c;
  undefined4 local_118;
  int local_114;
  int local_110;
  int local_10c;
  int local_108;
  undefined4 local_104;
  int local_100;
  int local_fc;
  char local_f8 [28];
  char local_dc;
  char local_db [127];
  char local_5c [80];
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059c904;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  _strncpy(param_1,param_2,0x3f);
  param_1[0x3f] = '\0';
  FUN_00479680();
  iVar7 = FUN_00479700(s_BEGIN_005c66bc,0);
  if (iVar7 == 0) {
    FUN_00479950(s_Area_block_BEGIN_expected_005c66c4,0);
  }
  FUN_004795a0();
  iVar7 = *(int *)(param_3 + 0x10);
  do {
    if ((iVar7 == 10) || (iVar7 = FUN_00479700(&DAT_005c66e0,0), iVar7 != 0)) {
      iVar7 = FUN_00479700(&DAT_005c6c70,0);
      if (iVar7 == 0) {
        FUN_00479950(s_Area_block_END_expected_005c6c74,0);
      }
      if (*(short *)(param_1 + 0x48) < 1) {
        sVar5 = *(short *)(param_1 + 0x4a);
        if (sVar5 <= *(short *)(param_1 + 0x48)) {
          iVar7 = sVar5 + 4;
          local_154 = (undefined4 *)FUN_00482fb0(iVar7 * 0x10);
          if (local_154 == (undefined4 *)0x0) {
            local_154 = (undefined4 *)0x0;
          }
          else {
            local_130 = sVar5 + 3;
          }
          iVar9 = (int)*(short *)(param_1 + 0x4a);
          puVar10 = local_154 + iVar9 * 4;
          for (uVar13 = (uint)((iVar7 - iVar9) * 0x10) >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
            *puVar10 = 0;
            puVar10 = puVar10 + 1;
          }
          for (iVar14 = 0; iVar14 != 0; iVar14 = iVar14 + -1) {
            *(undefined1 *)puVar10 = 0;
            puVar10 = (undefined4 *)((int)puVar10 + 1);
          }
          puVar10 = *(undefined4 **)(param_1 + 0x4c);
          if (puVar10 != (undefined4 *)0x0) {
            puVar8 = puVar10;
            puVar16 = local_154;
            for (uVar13 = (uint)(iVar9 * 0x10) >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
              *puVar16 = *puVar8;
              puVar8 = puVar8 + 1;
              puVar16 = puVar16 + 1;
            }
            for (iVar9 = 0; iVar9 != 0; iVar9 = iVar9 + -1) {
              *(undefined1 *)puVar16 = *(undefined1 *)puVar8;
              puVar8 = (undefined4 *)((int)puVar8 + 1);
              puVar16 = (undefined4 *)((int)puVar16 + 1);
            }
            FUN_004830f0(puVar10);
          }
          *(short *)(param_1 + 0x4a) = (short)iVar7;
          *(undefined4 **)(param_1 + 0x4c) = local_154;
        }
        sVar5 = *(short *)(param_1 + 0x48);
        puVar10 = (undefined4 *)(sVar5 * 0x10 + *(int *)(param_1 + 0x4c));
        *puVar10 = 0xc4653600;
        puVar10[1] = 0xc4653600;
        puVar10[2] = 1000000000;
        puVar10[3] = 1000000000;
        *(short *)(param_1 + 0x48) = sVar5 + 1;
      }
      ExceptionList = local_c;
      return 1;
    }
    if (*(int *)(param_3 + 0x10) != 4) {
      FUN_00479950(s_Area_keyword_expected_005c66e4,0);
    }
    iVar7 = FUN_00479700(s_LEVEL_005c66fc,0);
    if (iVar7 == 0) {
      iVar7 = FUN_00479700(&DAT_005c6728,0);
      if (iVar7 == 0) {
        iVar7 = FUN_00479700(s_SCRIPT_005c6770,0);
        if (iVar7 == 0) {
          iVar7 = FUN_00479700(s_BGEFFECT_005c67a4,0);
          if (iVar7 == 0) {
            iVar7 = FUN_00479700(s_AMBLIGHT_005c67e4,0);
            if (iVar7 == 0) {
              iVar7 = FUN_00479700(s_AMBCOLOR_005c681c,0);
              if (iVar7 == 0) {
                iVar7 = FUN_00479700(s_NIGHTAMBLIGHT_005c6864,0);
                if (iVar7 == 0) {
                  iVar7 = FUN_00479700(s_NIGHTAMBCOLOR_005c68a8,0);
                  if (iVar7 == 0) {
                    iVar7 = FUN_00479700(s_AUDIOENV_005c68fc,0);
                    if (iVar7 == 0) {
                      iVar7 = FUN_00479700(s_AMBSOUND_005c6944,0);
                      if (iVar7 == 0) {
                        iVar7 = FUN_00479700(s_CDPLAYLIST_005c69b4,0);
                        if (iVar7 == 0) {
                          iVar7 = FUN_00479700(s_USE3DMODE_005c6a50,0);
                          if (iVar7 == 0) {
                            iVar7 = FUN_00479700(s_CAMERA_005c6ad0,0);
                            if (iVar7 == 0) {
                              iVar7 = FUN_00479700(s_CAMERAFOV_005c6b3c,0);
                              if (iVar7 == 0) {
                                iVar7 = FUN_00479700(s_CAMERALIMITS_005c6b68,0);
                                if (iVar7 == 0) {
                                  iVar7 = FUN_00479700(s_LIGHT_005c6bbc,0);
                                  if (iVar7 == 0) {
                                    FUN_0058b100(local_5c,s_Invalid_area_tag__s_005c6c5c,
                                                 *(undefined4 *)(param_3 + 0x28));
                                    pcVar19 = local_5c;
                                    goto LAB_0041af8d;
                                  }
                                  FUN_00479580();
                                  puVar8 = (undefined4 *)FUN_00482fb0(100);
                                  puVar10 = puVar8;
                                  for (iVar7 = 0x19; iVar7 != 0; iVar7 = iVar7 + -1) {
                                    *puVar10 = 0;
                                    puVar10 = puVar10 + 1;
                                  }
                                  puVar2 = puVar8 + 0x10;
                                  iVar7 = FUN_0047a410(param_3,
                                                  s__20s_ID__i_MATCH__b__b__b_RANGE___005c6bc4,
                                                  puVar8,puVar2,(int)puVar8 + 0x46,
                                                  (int)puVar8 + 0x45,puVar8 + 0x11,puVar8 + 0x12,
                                                  (int)puVar8 + 0x4e,(int)puVar8 + 0x4d,
                                                  puVar8 + 0x13,puVar8 + 0x14,puVar8 + 0x15,
                                                  (int)puVar8 + 0x5a,(int)puVar8 + 0x59,
                                                  puVar8 + 0x16,puVar8 + 0x17,puVar8 + 0x18);
                                  if (iVar7 == 0) {
                                    FUN_00479950(s_Invalid_light_tag_005c6c18,0);
                                  }
                                  if (((int)*puVar2 < 0) || (0x8000 < (int)*puVar2)) {
                                    FUN_00479950(s_Light_id_must_be_between_0_and_3_005c6c2c,0);
                                  }
                                  *puVar2 = *puVar2 << 0x10 | *(uint *)(param_1 + 0x124);
                                  FUN_0041c840(puVar8);
                                }
                                else {
                                  FUN_00479580();
                                  iVar7 = FUN_0047a410(param_3,
                                                  s_ZOOM__i__i_PANHORZ__i__i_PANVERT_005c6b78,
                                                  &local_fc,&local_110,&local_114,&local_10c,
                                                  &local_100,&local_130);
                                  if (iVar7 == 0) {
                                    FUN_00479950(s_Invalid_camera_limit_tag_005c6ba0,0);
                                  }
                                  *(float *)(param_1 + 0x108) = (float)local_fc;
                                  *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x4000;
                                  *(float *)(param_1 + 0x10c) = (float)local_110;
                                  *(float *)(param_1 + 0x110) = (float)local_114 * _DAT_005a39f0;
                                  *(float *)(param_1 + 0x114) = (float)local_10c * _DAT_005a39f0;
                                  *(float *)(param_1 + 0x118) = (float)local_100 * _DAT_005a39f0;
                                  *(float *)(param_1 + 0x11c) = (float)local_130 * _DAT_005a39f0;
                                }
                              }
                              else {
                                FUN_00479580();
                                iVar7 = FUN_0047a410(param_3,&DAT_005c6b48,param_1 + 0x120);
                                if (iVar7 == 0) {
                                  pcVar19 = s_Unable_to_parse_camera_FOV_005c6b4c;
LAB_0041af8d:
                                  FUN_00479950(pcVar19,0);
                                }
                              }
                            }
                            else {
                              FUN_00479580();
                              pcVar19 = param_1 + 0xf8;
                              local_118 = 0xffffd8f0;
                              local_11c = 0xffffd8f0;
                              local_120 = -10000;
                              local_148 = -10000;
                              local_14c = -10000;
                              local_150 = -10000;
                              pcVar19[0] = -0x10;
                              pcVar19[1] = -0x28;
                              pcVar19[2] = -1;
                              pcVar19[3] = -1;
                              param_1[0x104] = '\0';
                              param_1[0x105] = '@';
                              param_1[0x106] = '\x1c';
                              param_1[0x107] = -0x3a;
                              param_1[0x100] = '\0';
                              param_1[0x101] = '@';
                              param_1[0x102] = '\x1c';
                              param_1[0x103] = -0x3a;
                              param_1[0xfc] = '\0';
                              param_1[0xfd] = '@';
                              param_1[0xfe] = '\x1c';
                              param_1[0xff] = -0x3a;
                              pcVar12 = param_1 + 0xec;
                              pcVar15 = param_1 + 0xe8;
                              piVar3 = (int *)(param_1 + 0xe4);
                              pcVar12[0] = -0x10;
                              pcVar12[1] = -0x28;
                              pcVar12[2] = -1;
                              pcVar12[3] = -1;
                              pcVar15[0] = -0x10;
                              pcVar15[1] = -0x28;
                              pcVar15[2] = -1;
                              pcVar15[3] = -1;
                              *piVar3 = -10000;
                              param_1[0xf4] = -0x10;
                              param_1[0xf5] = -0x28;
                              param_1[0xf6] = -1;
                              param_1[0xf7] = -1;
                              param_1[0xf0] = -0x10;
                              param_1[0xf1] = -0x28;
                              param_1[0xf2] = -1;
                              param_1[0xf3] = -1;
                              iVar7 = FUN_0047a410(param_3,
                                                  s_<POS__i__i__i_TARG__i__i__i>_<RE_005c6ad8,piVar3
                                                  ,pcVar15,pcVar12,param_1 + 0xf0,param_1 + 0xf4,
                                                  pcVar19,&local_120,&local_11c,&local_118,
                                                  &local_150,&local_14c,&local_148);
                              if (iVar7 == 0) {
                                FUN_00479950(s_Invalid_camera_tag_005c6b28,0);
                              }
                              if (*piVar3 != -10000) {
                                *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x400;
                              }
                              if (*(int *)(param_1 + 0xf0) != -10000) {
                                *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x800;
                              }
                              if (local_120 != -10000) {
                                *piVar3 = local_120;
                                *(undefined4 *)(param_1 + 0xe8) = local_11c;
                                *(undefined4 *)(param_1 + 0xec) = local_118;
                                *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x1000;
                              }
                              if (local_150 != -10000) {
                                *(float *)(param_1 + 0xfc) = (float)local_150;
                                *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x2000;
                                *(float *)(param_1 + 0x100) = (float)local_14c * _DAT_005a39f0;
                                *(float *)(param_1 + 0x104) = (float)local_148 * _DAT_005a39f0;
                              }
                            }
                          }
                          else {
                            FUN_00479580();
                            *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x100;
                            local_12c = 0;
                            local_124 = 0;
                            local_128 = 0;
                            local_134 = 0;
                            iVar7 = FUN_0047a410(param_3,s_<FREE>_<FOLLOW>_<FOLLOWREL>_<LOO_005c6a5c
                                                 ,&local_12c,&local_124,&local_128,&local_134);
                            if (iVar7 == 0) {
                              FUN_00479950(s_Invalid_camera_mode_tag_005c6a84,0);
                            }
                            if (local_12c == 0) {
                              if (local_124 == 0) {
                                if (local_128 == 0) {
                                  if (local_134 == 0) {
                                    pcVar19 = s_Must_use_FREE__FOLLOW__FOLLOWREL_005c6a9c;
                                    goto LAB_0041af8d;
                                  }
                                  *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x40000;
                                }
                                else {
                                  *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x20000;
                                }
                              }
                              else {
                                *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x10000;
                              }
                            }
                            else {
                              *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x8000;
                            }
                          }
                        }
                        else {
                          FUN_00479580();
                          iVar7 = FUN_00479700(s_RANDOM_005c6a10,0);
                          if (iVar7 == 0) {
                            param_1[0x240] = '\0';
                            param_1[0x241] = '\0';
                            param_1[0x242] = '\0';
                            param_1[0x243] = '\0';
                          }
                          else {
                            param_1[0x240] = '\x01';
                            param_1[0x241] = '\0';
                            param_1[0x242] = '\0';
                            param_1[0x243] = '\0';
                            FUN_00479580();
                          }
                          iVar7 = FUN_00479700(s_PAUSE_005c6a18,0);
                          if ((iVar7 != 0) &&
                             (iVar7 = FUN_0047a410(param_3,s_PAUSE__d_005c6a20,param_1 + 0x244),
                             iVar7 == 0)) {
                            FUN_00479950(s__CDPLAYLIST__RANDOM___PAUSE_paus_005c69c0,0);
                          }
                          iVar7 = FUN_00479700(s_TRACKS_005c6a2c,0);
                          if (iVar7 == 0) {
                            FUN_00479950(s__CDPLAYLIST__RANDOM___PAUSE_paus_005c69c0,0);
                          }
                          FUN_00479580();
                          iVar7 = *(int *)(param_3 + 0x10);
                          param_1[0x1bc] = '\0';
                          param_1[0x1bd] = '\0';
                          param_1[0x1be] = '\0';
                          param_1[0x1bf] = '\0';
                          while ((iVar7 != 9 && (iVar7 != 10))) {
                            if (0x1f < *(int *)(param_1 + 0x1bc)) {
                              FUN_00479950(s_CDPLAYLIST_too_many_tracks_005c6a34,0);
                            }
                            if (*(int *)(param_3 + 0x10) != 8) {
                              FUN_00479950(s__CDPLAYLIST__RANDOM___PAUSE_paus_005c69c0,0);
                            }
                            *(int *)(param_1 + *(int *)(param_1 + 0x1bc) * 4 + 0x1c0) =
                                 *(int *)(param_3 + 0x14) + -1;
                            *(int *)(param_1 + 0x1bc) = *(int *)(param_1 + 0x1bc) + 1;
                            FUN_00479580();
                            iVar7 = *(int *)(param_3 + 0x10);
                          }
                          FUN_004795a0();
                          *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x20;
                        }
                      }
                      else {
                        FUN_00479580();
                        iVar7 = FUN_0047a410(param_3,&DAT_005c6950,&local_dc);
                        if (iVar7 == 0) {
                          FUN_00479950(s__AMBSOUND_<soundfile_reference>__005c6954,0);
                          FUN_004795a0();
                        }
                        else {
                          pcVar19 = param_1 + 0x138;
                          pcVar12 = &local_dc;
                          if (local_dc == '\"') {
                            pcVar12 = local_db;
                          }
                          cVar4 = *pcVar12;
                          pcVar15 = pcVar19;
                          while ((cVar4 != '\"' && (cVar4 != '\0'))) {
                            *pcVar15 = cVar4;
                            pcVar1 = pcVar12 + 1;
                            pcVar15 = pcVar15 + 1;
                            pcVar12 = pcVar12 + 1;
                            cVar4 = *pcVar1;
                          }
                          *pcVar15 = '\0';
                          iVar7 = FUN_0049c430(pcVar19);
                          if (iVar7 < 0) {
                            FUN_004820b0(s_AREA_DEF__line__d_____s__sound_f_005c6980,
                                         *(undefined4 *)(param_3 + 0x30),pcVar19);
                            FUN_004795a0();
                          }
                          else {
                            *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x40;
                            FUN_004795a0();
                          }
                        }
                      }
                    }
                    else {
                      iVar7 = FUN_0047a410(param_3,s_AUDIOENV__i_005c6908,&local_104);
                      if (iVar7 == 0) {
                        FUN_00479950(s__AUDIOENV_<environment_number>__e_005c6918,0);
                      }
                      *(undefined4 *)(param_1 + 600) = local_104;
                      *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x80;
                    }
                  }
                  else {
                    iVar7 = FUN_0047a410(param_3,s_NIGHTAMBCOLOR__b___b___b_005c68b8,param_1 + 0xde,
                                         param_1 + 0xdd,param_1 + 0xdc);
                    if (iVar7 == 0) {
                      FUN_00479950(s__NIGHTAMBCOLOR_red_green_blue__e_005c68d4,0);
                    }
                    *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 8;
                  }
                }
                else {
                  iVar7 = FUN_0047a410(param_3,s_NIGHTAMBLIGHT__i_005c6874,param_1 + 0xd4);
                  if (iVar7 == 0) {
                    FUN_00479950(s__NIGHTAMBLIGHT_light__expected_005c6888,0);
                  }
                  *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 8;
                }
              }
              else {
                iVar7 = FUN_0047a410(param_3,s_AMBCOLOR__b___b___b_005c6828,param_1 + 0xda,
                                     param_1 + 0xd9,param_1 + 0xd8);
                if (iVar7 == 0) {
                  FUN_00479950(s__AMBCOLOR_red_green_blue__expect_005c6840,0);
                }
                *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 4;
              }
            }
            else {
              iVar7 = FUN_0047a410(param_3,s_AMBLIGHT__i_005c67f0,param_1 + 0xd0);
              if (iVar7 == 0) {
                FUN_00479950(s__AMBLIGHT_light__expected_005c6800,0);
              }
              *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 4;
            }
          }
          else {
            iVar7 = FUN_0047a410(param_3,s_BGEFFECT__s_005c67b0,local_f8);
            if (iVar7 == 0) {
              pcVar19 = s__BGEFFECT__effect_name___expecte_005c67c0;
              goto LAB_0041af8d;
            }
            pcVar19 = (char *)FUN_00482ef0(0x24);
            uVar13 = 0xffffffff;
            *(char **)(param_1 + 0x25c) = pcVar19;
            pcVar12 = local_f8;
            do {
              pcVar15 = pcVar12;
              if (uVar13 == 0) break;
              uVar13 = uVar13 - 1;
              pcVar15 = pcVar12 + 1;
              cVar4 = *pcVar12;
              pcVar12 = pcVar15;
            } while (cVar4 != '\0');
            uVar13 = ~uVar13;
            pcVar12 = pcVar15 + -uVar13;
            for (uVar11 = uVar13 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
              *(undefined4 *)pcVar19 = *(undefined4 *)pcVar12;
              pcVar12 = pcVar12 + 4;
              pcVar19 = pcVar19 + 4;
            }
            for (uVar13 = uVar13 & 3; uVar13 != 0; uVar13 = uVar13 - 1) {
              *pcVar19 = *pcVar12;
              pcVar12 = pcVar12 + 1;
              pcVar19 = pcVar19 + 1;
            }
            iVar7 = *(int *)(param_1 + 0x25c);
            *(undefined4 *)(iVar7 + 0x1c) = 0;
            *(undefined4 *)(iVar7 + 0x20) = 0;
          }
        }
        else {
          iVar7 = FUN_0047a410(param_3,s_SCRIPT__s_005c6778,param_1 + 0x50);
          if (iVar7 == 0) {
            FUN_00479950(s__SCRIPT__filename___expected_005c6784,0);
          }
          bVar18 = DAT_0066829c != 0;
          *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) | 0x10;
          if ((((bVar18) && (-1 < DAT_0065a784)) &&
              (pbVar6 = *(byte **)(DAT_0065a77c + DAT_0065a784 * 4), pbVar6 != (byte *)0x0)) &&
             ((*pbVar6 & 0x10) != 0)) {
            FUN_00496490_TScriptManager_Load(param_1 + 0x50,param_1);
          }
        }
      }
      else {
        iVar7 = FUN_0047a410(param_3,s_RECT__i___i___i___i_005c6730,&local_144,&local_140,&local_13c
                             ,&local_138);
        if (iVar7 == 0) {
          FUN_00479950(s__RECT_left_top_right_bottom__exp_005c6748,0);
        }
        iVar9 = local_140;
        iVar7 = local_144;
        if (local_13c < local_144) {
          local_144 = local_13c;
          local_13c = iVar7;
        }
        if (local_138 < local_140) {
          local_140 = local_138;
          local_138 = iVar9;
        }
        if (*(short *)(param_1 + 0x4a) <= *(short *)(param_1 + 0x48)) {
          iVar7 = *(short *)(param_1 + 0x4a) + 4;
          local_108 = iVar7;
          puVar10 = (undefined4 *)FUN_00482fb0(iVar7 * 0x10);
          local_4 = 0;
          if (puVar10 == (undefined4 *)0x0) {
            puVar10 = (undefined4 *)0x0;
          }
          else {
            FUN_00406130(puVar10,0x10,iVar7,&LAB_0041c6f0);
          }
          iVar9 = (int)*(short *)(param_1 + 0x4a);
          local_4 = 0xffffffff;
          puVar8 = puVar10 + iVar9 * 4;
          for (uVar13 = (uint)((iVar7 - iVar9) * 0x10) >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
            *puVar8 = 0;
            puVar8 = puVar8 + 1;
          }
          for (iVar7 = 0; iVar7 != 0; iVar7 = iVar7 + -1) {
            *(undefined1 *)puVar8 = 0;
            puVar8 = (undefined4 *)((int)puVar8 + 1);
          }
          puVar8 = *(undefined4 **)(param_1 + 0x4c);
          if (puVar8 != (undefined4 *)0x0) {
            puVar16 = puVar8;
            puVar17 = puVar10;
            for (uVar13 = (uint)(iVar9 * 0x10) >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
              *puVar17 = *puVar16;
              puVar16 = puVar16 + 1;
              puVar17 = puVar17 + 1;
            }
            for (iVar7 = 0; iVar7 != 0; iVar7 = iVar7 + -1) {
              *(undefined1 *)puVar17 = *(undefined1 *)puVar16;
              puVar16 = (undefined4 *)((int)puVar16 + 1);
              puVar17 = (undefined4 *)((int)puVar17 + 1);
            }
            FUN_004830f0(puVar8);
          }
          *(undefined4 **)(param_1 + 0x4c) = puVar10;
          *(undefined2 *)(param_1 + 0x4a) = (undefined2)local_108;
        }
        FUN_0041c700(&local_144);
        *(short *)(param_1 + 0x48) = *(short *)(param_1 + 0x48) + 1;
      }
    }
    else {
      FUN_00479580();
      if (*(int *)(param_3 + 0x10) == 8) {
        *(undefined4 *)(param_1 + 0x40) = *(undefined4 *)(param_3 + 0x14);
LAB_0041a5c6:
        FUN_00479580();
      }
      else {
        iVar7 = FUN_00479700(&DAT_005c6704,0);
        if (iVar7 != 0) {
          param_1[0x40] = -1;
          param_1[0x41] = -1;
          param_1[0x42] = -1;
          param_1[0x43] = -1;
          goto LAB_0041a5c6;
        }
      }
      if (*(int *)(param_3 + 0x10) != 9) {
        FUN_00479950(s__LEVEL_<ALL_level>__expected_005c6708,0);
      }
      FUN_004795a0();
    }
    iVar7 = *(int *)(param_3 + 0x10);
  } while( true );
}


