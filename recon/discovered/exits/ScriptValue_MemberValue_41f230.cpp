// ScriptValue_MemberValue @ 0x0041f230 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// expression member evaluator; `isoutside` -> 0x0050d2b0 at 0x0041f51b
// FUN_0041f230 @ 0041f230 size=2328

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_0041f230(int param_1,uint *param_2,int param_3,undefined4 param_4)

{
  undefined **ppuVar1;
  char cVar2;
  int iVar3;
  int *piVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  int *piVar11;
  char *pcVar12;
  undefined **ppuVar13;
  char *pcVar14;
  char *pcVar15;
  undefined4 local_80;
  uint local_7c;
  int local_78;
  uint local_74;
  uint local_70;
  int local_6c;
  int local_68;
  undefined4 local_64;
  int local_60;
  int local_5c;
  undefined4 local_58;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
  int local_44;
  int local_40;
  char local_3c [60];
  
  local_78 = -1;
  local_74 = 0xfeced300;
  local_7c = 0xfeced300;
  if (param_3 == 0) {
    local_80 = 0;
  }
  else {
    local_80 = *(undefined4 *)(param_3 + 0x84);
  }
  iVar10 = *(int *)(param_1 + 0x10);
  uVar5 = local_70;
  if (iVar10 == 9) {
    return 0;
  }
  do {
    if (iVar10 == 10) break;
    if (iVar10 == 7) {
      if (local_78 != -1) {
        return 0;
      }
      uVar8 = 0xffffffff;
      pcVar12 = *(char **)(param_1 + 0x28);
      do {
        pcVar15 = pcVar12;
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        pcVar15 = pcVar12 + 1;
        cVar2 = *pcVar12;
        pcVar12 = pcVar15;
      } while (cVar2 != '\0');
      uVar8 = ~uVar8;
      pcVar12 = pcVar15 + -uVar8;
      pcVar15 = local_3c;
      for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
        *(undefined4 *)pcVar15 = *(undefined4 *)pcVar12;
        pcVar12 = pcVar12 + 4;
        pcVar15 = pcVar15 + 4;
      }
      for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
        *pcVar15 = *pcVar12;
        pcVar12 = pcVar12 + 1;
        pcVar15 = pcVar15 + 1;
      }
      FUN_00478a10();
      if (*(int *)(param_1 + 0x10) == 7) {
        uVar8 = 0xffffffff;
        pcVar12 = *(char **)(param_1 + 0x28);
        do {
          pcVar15 = pcVar12;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pcVar15 = pcVar12 + 1;
          cVar2 = *pcVar12;
          pcVar12 = pcVar15;
        } while (cVar2 != '\0');
        uVar8 = ~uVar8;
        iVar10 = -1;
        pcVar12 = local_3c;
        do {
          pcVar14 = pcVar12;
          if (iVar10 == 0) break;
          iVar10 = iVar10 + -1;
          pcVar14 = pcVar12 + 1;
          cVar2 = *pcVar12;
          pcVar12 = pcVar14;
        } while (cVar2 != '\0');
        pcVar12 = pcVar15 + -uVar8;
        pcVar15 = pcVar14 + -1;
        for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
          *(undefined4 *)pcVar15 = *(undefined4 *)pcVar12;
          pcVar12 = pcVar12 + 4;
          pcVar15 = pcVar15 + 4;
        }
        for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
          *pcVar15 = *pcVar12;
          pcVar12 = pcVar12 + 1;
          pcVar15 = pcVar15 + 1;
        }
      }
      iVar10 = 0;
      if (PTR_DAT_005c8350 == (undefined *)0x0) {
        return 0;
      }
      ppuVar13 = &PTR_DAT_005c8350;
      while( true ) {
        if (0x5c844f < (int)ppuVar13) {
          return 0;
        }
        iVar3 = FUN_0059a530(local_3c,*ppuVar13);
        if (iVar3 == 0) break;
        ppuVar1 = ppuVar13 + 1;
        ppuVar13 = ppuVar13 + 1;
        iVar10 = iVar10 + 1;
        if (*ppuVar1 == (undefined *)0x0) {
          return 0;
        }
      }
      if (iVar10 < 0) {
        return 0;
      }
      if ((local_7c == 0xfeced300) && (iVar10 != 8)) {
        return 0;
      }
      local_78 = iVar10;
      FUN_00479580();
      uVar8 = local_7c;
      uVar9 = local_74;
    }
    else {
      if (iVar10 == 4) {
LAB_0041f38b:
        local_70 = *(uint *)(param_1 + 0x28);
        iVar10 = 0;
        if (PTR_DAT_005c8350 != (undefined *)0x0) {
          ppuVar13 = &PTR_DAT_005c8350;
          do {
            if (0x5c844f < (int)ppuVar13) break;
            iVar3 = FUN_0059a530(local_70,*ppuVar13);
            if (iVar3 == 0) {
              if (-1 < iVar10) {
                if ((local_7c == 0xfeced300) && (iVar10 != 8)) {
                  return 0;
                }
                local_78 = iVar10;
                FUN_00479580();
                uVar8 = local_7c;
                uVar9 = local_74;
                goto LAB_0041fad1;
              }
              break;
            }
            ppuVar1 = ppuVar13 + 1;
            ppuVar13 = ppuVar13 + 1;
            iVar10 = iVar10 + 1;
          } while (*ppuVar1 != (undefined *)0x0);
        }
        uVar8 = 0xffffffff;
        pcVar12 = *(char **)(param_1 + 0x28);
        do {
          pcVar15 = pcVar12;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pcVar15 = pcVar12 + 1;
          cVar2 = *pcVar12;
          pcVar12 = pcVar15;
        } while (cVar2 != '\0');
        uVar8 = ~uVar8;
        pcVar12 = pcVar15 + -uVar8;
        pcVar15 = local_3c;
        for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
          *(undefined4 *)pcVar15 = *(undefined4 *)pcVar12;
          pcVar12 = pcVar12 + 4;
          pcVar15 = pcVar15 + 4;
        }
        for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
          *pcVar15 = *pcVar12;
          pcVar12 = pcVar12 + 1;
          pcVar15 = pcVar15 + 1;
        }
        FUN_00478a10();
        iVar10 = FUN_00479700(&DAT_005cac78,0);
        if (iVar10 == 0) {
          uVar5 = FUN_00497800(local_3c,param_3);
          if ((uVar5 == 0xfeced300) && (uVar5 = FUN_004975d0(local_3c), uVar5 == 0xfeced300)) {
            uVar5 = 0;
          }
        }
        else {
          FUN_00478a10();
          if (*(int *)(param_1 + 0x10) != 4) {
            return 0;
          }
          piVar4 = (int *)FUN_0041e690(local_3c,param_4,local_80);
          if (piVar4 == (int *)0x0) {
            iVar10 = *(int *)(param_1 + 0x10);
            if (iVar10 != 9) goto LAB_0041fae6;
            break;
          }
          iVar10 = FUN_00479700(s_state_005cac7c,0);
          if (iVar10 == 0) {
            iVar10 = FUN_00479700(s_getitemamount_005cac84,0);
            if ((iVar10 != 0) || (iVar10 = FUN_00479700(s_amount_005cac94,0), iVar10 != 0)) {
              FUN_00478a10();
              FUN_00479580();
              uVar5 = (**(code **)(*piVar4 + 0x84))(*(undefined4 *)(param_1 + 0x28));
              goto LAB_0041f9ec;
            }
            iVar10 = FUN_00479700(s_hasemptyslot_005cac9c,0);
            if (iVar10 == 0) {
              iVar10 = FUN_00479700(s_isoutside_005cacac,0);
              if (iVar10 == 0) {
                iVar10 = FUN_00479700(s_maxslots_005cacb8,0);
                if (iVar10 == 0) {
                  iVar10 = FUN_00479700(s_timeofday_005cacc4,0);
                  if (iVar10 == 0) {
                    iVar10 = FUN_00479700(s_random_005cacd0,0);
                    if (iVar10 == 0) {
                      iVar10 = FUN_00479700(&DAT_005cacd8,0);
                      if (iVar10 == 0) {
                        iVar10 = FUN_00479700(s_getdistance_005cace0,0);
                        if ((iVar10 == 0) &&
                           (iVar10 = FUN_00479700(s_getdist_005cacec,0), iVar10 == 0)) {
                          iVar10 = FUN_00479700(s_isatrelativeposition_005cacf4,0);
                          if ((iVar10 == 0) &&
                             (iVar10 = FUN_00479700(s_isrelpos_005cad0c,0), iVar10 == 0)) {
                            iVar10 = FUN_00479700(s_isatrelativedistance_005cad18,0);
                            if (iVar10 == 0) {
                              iVar10 = FUN_00479700(s_lastattack_005cad58,0);
                              if (iVar10 == 0) {
                                iVar10 = FUN_00479700(s_position_005cad68,0);
                                if (iVar10 == 0) {
                                  iVar10 = FUN_00479700(s_groupinrange_005cad80,0);
                                  if (iVar10 == 0) {
                                    FUN_00478a10();
                                    FUN_00479580();
                                    iVar10 = FUN_00473900(*(undefined4 *)(param_1 + 0x28));
                                    if (iVar10 == 0) goto LAB_0041f7e2;
                                    uVar5 = (**(code **)(*piVar4 + 0xd4))
                                                      (*(undefined4 *)(param_1 + 0x28));
                                    FUN_00479580();
                                  }
                                  else {
                                    uVar5 = FUN_00428f50(param_3,param_1,param_4,local_80);
                                    FUN_00479580();
                                  }
                                }
                                else {
                                  FUN_00478a10();
                                  FUN_00478a10();
                                  iVar10 = FUN_00479700(&DAT_005cad74,0);
                                  if (iVar10 != 0) {
                                    uVar5 = piVar4[4];
                                  }
                                  iVar10 = FUN_00479700(&DAT_005cad78,0);
                                  if (iVar10 != 0) {
                                    uVar5 = piVar4[5];
                                  }
                                  iVar10 = FUN_00479700(&DAT_005cad7c,0);
                                  if (iVar10 == 0) goto LAB_0041f9ec;
                                  uVar5 = piVar4[6];
                                  FUN_00479580();
                                }
                              }
                              else {
                                if ((((short)piVar4[1] == 0xc) || ((short)piVar4[1] == 0xb)) &&
                                   (piVar4[0x58] != 0)) {
                                  FUN_00478a10();
                                  FUN_00479580();
                                  iVar10 = FUN_00479700(&DAT_005cad64,0);
                                  if (iVar10 != 0) {
                                    FUN_00478a10();
                                    FUN_00479580();
                                  }
                                  if ((*(int *)(param_1 + 0x28) != 0) && (piVar4[0x5a] != 0)) {
                                    uVar5 = FUN_00479700(piVar4[0x58],0);
                                    FUN_00479580();
                                    goto LAB_0041fa31;
                                  }
                                }
LAB_0041f7e2:
                                uVar5 = 0;
                                FUN_00479580();
                              }
                            }
                            else {
                              FUN_00478a10();
                              FUN_00479580();
                              iVar10 = FUN_0041e690(*(undefined4 *)(param_1 + 0x28),param_4,local_80
                                                   );
                              FUN_00479580();
                              uVar5 = 0;
                              if (*(int *)(param_1 + 0x10) == 8) {
                                local_74 = *(uint *)(param_1 + 0x14);
                                FUN_00478a10();
                                FUN_00479580();
                                uVar5 = 0;
                                if (*(int *)(param_1 + 0x10) == 8) {
                                  local_70 = *(uint *)(param_1 + 0x14);
                                  FUN_00478a10();
                                  uVar5 = local_70;
                                }
                                if (iVar10 == 0) {
                                  FUN_0041ee50(s_Can_t_find_any_object_by_that_na_005cad30);
                                  return 4;
                                }
                                iVar3 = *(int *)(iVar10 + 0x10);
                                local_68 = *(int *)(iVar10 + 0x14);
                                local_64 = *(undefined4 *)(iVar10 + 0x18);
                                local_54 = piVar4[4];
                                local_50 = piVar4[5];
                                local_4c = piVar4[6];
                                iVar10 = *(byte *)(iVar10 + 0x36) + uVar5;
                                fcos((float10)iVar10 * (float10)_DAT_005a3a90);
                                local_70 = iVar10;
                                local_6c = iVar3;
                                iVar7 = __ftol();
                                local_70 = iVar10 + 0x7f;
                                local_68 = local_68 + iVar7;
                                fsin((float10)(int)local_70 * (float10)_DAT_005a3a90);
                                local_6c = __ftol();
                                local_6c = iVar3 + local_6c;
                                piVar4 = &local_54;
                                piVar11 = &local_6c;
                                goto LAB_0041f972;
                              }
LAB_0041f9ec:
                              FUN_00479580();
                            }
                          }
                          else {
                            FUN_00478a10();
                            FUN_00479580();
                            iVar10 = FUN_0041e690(*(undefined4 *)(param_1 + 0x28),param_4,local_80);
                            FUN_00479580();
                            uVar5 = 0;
                            if (*(int *)(param_1 + 0x10) != 8) goto LAB_0041f9ec;
                            local_70 = *(uint *)(param_1 + 0x14);
                            FUN_00478a10();
                            FUN_00479580();
                            if (*(int *)(param_1 + 0x10) == 8) {
                              uVar5 = *(uint *)(param_1 + 0x14);
                              FUN_00478a10();
                            }
                            local_58 = *(undefined4 *)(iVar10 + 0x18);
                            local_48 = piVar4[4];
                            local_44 = piVar4[5];
                            local_40 = piVar4[6];
                            local_60 = *(int *)(iVar10 + 0x10) + local_70;
                            local_5c = *(int *)(iVar10 + 0x14) + uVar5;
                            piVar4 = &local_48;
                            piVar11 = &local_60;
LAB_0041f972:
                            iVar10 = FUN_0046de60(piVar11,piVar4);
                            uVar5 = (uint)(iVar10 < 0x32);
                            FUN_00479580();
                          }
                        }
                        else {
                          FUN_00479580();
                          iVar10 = FUN_0041e690(*(undefined4 *)(param_1 + 0x28),param_4,local_80);
                          FUN_00478a10();
                          if (iVar10 == 0) goto LAB_0041f7e2;
                          uVar5 = (**(code **)(*piVar4 + 4))(iVar10);
                          FUN_00479580();
                        }
                      }
                      else {
                        uVar5 = (uint)*(byte *)((int)piVar4 + 0x36);
                        FUN_00479580();
                      }
                    }
                    else if (*(int *)(param_1 + 0x10) == 8) {
                      uVar5 = FUN_00483300(1,*(undefined4 *)(param_1 + 0x14));
                      FUN_00479580();
                    }
                    else {
                      uVar5 = FUN_00483300(1,100);
                      FUN_00479580();
                    }
                  }
                  else {
                    uVar5 = FUN_0047e9e0();
                    FUN_00479580();
                  }
                }
                else {
                  uVar5 = FUN_00470040();
                  FUN_00479580();
                }
              }
              else {
                FUN_00478a10();
                FUN_00479580();
                uVar6 = FUN_0041e690(*(undefined4 *)(param_1 + 0x28),param_4,local_80);
                uVar5 = FUN_0050d2b0(uVar6);
                FUN_00479580();
              }
            }
            else {
              uVar5 = (**(code **)(*piVar4 + 0x88))();
              FUN_00479580();
            }
          }
          else {
            uVar5 = (uint)*(ushort *)(piVar4 + 3);
            FUN_00479580();
          }
        }
LAB_0041fa31:
        if (*(int *)(param_1 + 0x10) == 1) {
          FUN_00478a10();
        }
      }
      else {
        if ((iVar10 != 8) && (uVar8 = local_7c, uVar9 = local_74, iVar10 != 2)) goto LAB_0041fad1;
        if (iVar10 == 4) goto LAB_0041f38b;
        if (iVar10 == 2) {
          uVar5 = 0;
          iVar10 = 0;
          cVar2 = **(char **)(param_1 + 0x28);
          while (cVar2 != '\0') {
            uVar5 = uVar5 | cVar2 + -0x41 << ((byte)iVar10 & 0x1f);
            iVar3 = iVar10 + 1;
            iVar10 = iVar10 + 1;
            cVar2 = (*(char **)(param_1 + 0x28))[iVar3];
          }
        }
        else {
          uVar5 = *(uint *)(param_1 + 0x14);
        }
        FUN_00479580();
      }
      if (uVar5 == 0xfeced300) {
        return 0;
      }
      if (local_7c == 0xfeced300) {
        uVar8 = uVar5;
        uVar9 = uVar5;
        if (local_78 == 7) {
          uVar9 = FUN_0041f0b0(&local_7c,7,uVar5);
          uVar8 = local_7c;
        }
      }
      else {
        if (local_78 == -1) {
          return 0;
        }
        uVar9 = FUN_0041f0b0(&local_7c,local_78,uVar5);
        local_78 = -1;
        uVar8 = local_7c;
      }
    }
LAB_0041fad1:
    local_74 = uVar9;
    local_7c = uVar8;
    iVar10 = *(int *)(param_1 + 0x10);
  } while (iVar10 != 9);
  goto LAB_0041fafa;
  while( true ) {
    FUN_00478a10();
    iVar10 = *(int *)(param_1 + 0x10);
    if (iVar10 == 9) break;
LAB_0041fae6:
    if (iVar10 == 10) break;
  }
LAB_0041fafa:
  if (local_74 == 0xfeced300) {
    return 0;
  }
  *param_2 = local_74;
  return 1;
}


