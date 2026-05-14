// FUN_004933d0_TScript_Continue @ 004933d0 size=2666

undefined4 __thiscall FUN_004933d0_TScript_Continue(int param_1,int param_2)

{
  char *pcVar1;
  int iVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  uint uVar8;
  int iVar9;
  undefined4 *puVar10;
  char *pcVar11;
  bool bVar12;
  int local_32c;
  undefined4 local_328;
  int local_324;
  int *local_320;
  undefined ***local_31c;
  int local_318;
  undefined4 local_314;
  int local_310;
  undefined4 local_308;
  undefined4 local_304;
  undefined1 *local_300;
  undefined1 local_2fc;
  undefined4 local_2f8;
  int local_2e8;
  int local_2e4;
  int local_2e0;
  undefined **local_2dc;
  char *local_2d8;
  int local_2d4;
  int local_2d0;
  char *local_2cc;
  undefined1 auStack_2c8 [100];
  undefined1 auStack_264 [100];
  undefined1 auStack_200 [100];
  undefined1 auStack_19c [100];
  undefined1 auStack_138 [100];
  undefined1 auStack_d4 [100];
  undefined1 auStack_70 [100];
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059d957;
  local_c = ExceptionList;
  local_32c = 0;
  local_2e0 = -1;
  local_2e4 = 0;
  local_2e8 = 0;
  if ((((*(uint *)(param_1 + 0x4c) & 0x10000) == 0) && (DAT_0066856c == 0)) &&
     ((*(int *)(param_1 + 0xc) == 0 || ((*(uint *)(*(int *)(param_1 + 0xc) + 8) & 0x200000) == 0))))
  {
    ExceptionList = &local_c;
    if (*(short *)(param_1 + 0xc0) != 0) {
      sVar3 = *(short *)(param_1 + 0xc0) + -1;
      bVar12 = DAT_0066829c != 0;
      ExceptionList = &local_c;
      *(short *)(param_1 + 0xc0) = sVar3;
      if ((bVar12) && (sVar3 == 0)) {
        FUN_00492490();
      }
    }
    if (((param_2 == 0) || (*(char *)(param_1 + 0xb4) != '\0')) &&
       (iVar4 = FUN_00492d70_TScript_Triggered(param_2), iVar4 == 0)) {
      ExceptionList = local_c;
      return 0;
    }
    iVar4 = DAT_0066829c;
    *(undefined1 *)(param_1 + 0xb4) = 0;
    *(undefined1 *)(param_1 + 0xb5) = 0;
    if (((iVar4 != 0) && (DAT_00676828 != 0)) && (DAT_0067682c == 0)) {
      ExceptionList = local_c;
      return 0;
    }
    if ((*(int *)(param_1 + 0xe4) != 0) && (*(int *)(param_1 + 0xd8) != 0)) {
      FUN_004d0950(*(int *)(param_1 + 0xd8),0xffffffff,0,*(undefined4 *)(param_1 + 0xdc));
      if (*(int *)(param_1 + 0xe4) != 0) {
        FUN_00492b00(8,*(int *)(param_1 + 0xe4));
      }
      *(undefined4 *)(param_1 + 0xe4) = 0;
      ExceptionList = local_c;
      return 0;
    }
    local_2d4 = *(int *)(*(int *)(param_1 + 8) + 4);
    local_2d0 = *(int *)(*(int *)(param_1 + 8) + 0x40) + local_2d4;
    local_2dc = &PTR_LAB_005a36f8;
    local_2d8 = s_String_005da144;
    local_2cc = (char *)local_2d4;
    FUN_00478720();
    local_31c = &local_2dc;
    local_4 = 0;
    local_328 = 0;
    local_324 = 0;
    local_320 = (int *)0x0;
    local_318 = 0;
    local_314 = 0;
    local_310 = 0;
    local_308 = 0;
    local_304 = 0;
    local_2fc = 0;
    local_2f8 = 1;
    local_300 = (undefined1 *)FUN_00482fb0(0x2000);
    *local_300 = 0;
    local_4 = 1;
    iVar4 = *(int *)(param_1 + 4);
    if (iVar4 != 0) {
      do {
        iVar9 = 0;
        if (0 < *(int *)(iVar4 + 0x44)) {
          do {
            iVar5 = *(int *)(*(int *)(iVar4 + 0x1c) + iVar9 * 4);
            if (iVar5 == 0) {
              iVar5 = *(int *)(iVar4 + 0x20);
            }
            iVar6 = FUN_004927b0(iVar5,*(undefined4 *)(param_1 + 0x4c),
                                 *(undefined4 *)(param_1 + 0xc));
            iVar5 = local_32c;
            iVar2 = local_2e0;
            if (iVar6 != 0) {
              piVar7 = *(int **)(*(int *)(iVar4 + 0x1c) + iVar9 * 4);
              if (piVar7 == (int *)0x0) {
                piVar7 = *(int **)(iVar4 + 0x20);
              }
              if (((*piVar7 != 1) || (iVar5 = iVar4, iVar2 = iVar9, local_32c != 0)) &&
                 ((*(int *)(param_1 + 0x48) == 0 ||
                  ((*(int *)(param_1 + 0x1c) == 1 ||
                   (iVar5 = local_32c, iVar2 = local_2e0, *(int *)(param_1 + 0xac) != 0)))))) {
                if ((*(int *)(param_1 + 0x48) != 0) && (*(int *)(param_1 + 0x1c) == 1)) {
                  *(int *)(param_1 + 0xac) = *(int *)(param_1 + 0x48);
                  *(undefined4 *)(param_1 + 0xb0) = *(undefined4 *)(param_1 + 0xa4);
                }
                puVar10 = *(undefined4 **)(*(int *)(iVar4 + 0x1c) + iVar9 * 4);
                if (puVar10 == (undefined4 *)0x0) {
                  puVar10 = *(undefined4 **)(iVar4 + 0x20);
                }
                FUN_00492440(iVar4,puVar10[1],puVar10[0xe]);
                *(undefined4 *)(param_1 + 0x1c) = *puVar10;
                *(undefined4 *)(param_1 + 0x18) = 0;
                *(undefined1 *)(param_1 + 0x20) = 0;
                local_2e8 = 1;
                *(undefined4 *)(param_1 + 0xa4) = 0;
                if (*(int **)(param_1 + 0xc) != (int *)0x0) {
                  (**(code **)(**(int **)(param_1 + 0xc) + 0x148))();
                }
                if (*(int **)(param_1 + 0xc4) != (int *)0x0) {
                  (**(code **)(**(int **)(param_1 + 0xc4) + 0x148))();
                }
                if (*(int **)(param_1 + 200) != (int *)0x0) {
                  (**(code **)(**(int **)(param_1 + 200) + 0x148))();
                }
                if (*(int *)(param_1 + 0x1c) == 1) {
                  *(undefined4 *)(param_1 + 0x10) = 0xffffffff;
                }
                else if (((*(int *)(param_1 + 0xc4) != 0) && (*(int *)(param_1 + 0xcc) != 0)) &&
                        (iVar4 = FUN_0059a530_stricmp(*(int *)(param_1 + 0xcc),&DAT_005da14c), iVar4 == 0))
                {
                  *(undefined4 *)(param_1 + 0x10) = *(undefined4 *)(*(int *)(param_1 + 0xc4) + 0x40)
                  ;
                }
                if (*(int *)(param_1 + 0x1c) != 1) {
                  *(undefined2 *)(param_1 + 0xc0) = 0xb40;
                }
                goto LAB_004937b4;
              }
            }
            local_2e0 = iVar2;
            local_32c = iVar5;
            iVar9 = iVar9 + 1;
          } while (iVar9 < *(int *)(iVar4 + 0x44));
        }
        iVar4 = *(int *)(iVar4 + 8);
      } while (iVar4 != 0);
      if (local_32c != 0) {
        if (*(int *)(param_1 + 0x48) != 0) goto LAB_00493827;
        puVar10 = *(undefined4 **)(*(int *)(local_32c + 0x1c) + local_2e0 * 4);
        if (puVar10 == (undefined4 *)0x0) {
          puVar10 = *(undefined4 **)(local_32c + 0x20);
        }
        FUN_00492440(local_32c,puVar10[1],puVar10[0xe]);
        *(undefined4 *)(param_1 + 0x1c) = *puVar10;
        *(undefined4 *)(param_1 + 0x18) = 0;
        *(undefined1 *)(param_1 + 0x20) = 0;
        if (*(int **)(param_1 + 0xc) != (int *)0x0) {
          (**(code **)(**(int **)(param_1 + 0xc) + 0x148))();
        }
        if (*(int **)(param_1 + 0xc4) != (int *)0x0) {
          (**(code **)(**(int **)(param_1 + 0xc4) + 0x148))();
        }
        if (*(int **)(param_1 + 200) != (int *)0x0) {
          (**(code **)(**(int **)(param_1 + 200) + 0x148))();
        }
        local_2e8 = 1;
        if (*(int *)(param_1 + 0xac) == 0) {
          *(undefined4 *)(param_1 + 0xa4) = 0;
          *(undefined4 *)(param_1 + 0xac) = *(undefined4 *)(param_1 + 0x48);
        }
        else {
          *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0xac);
          *(undefined4 *)(param_1 + 0xa4) = *(undefined4 *)(param_1 + 0xb0);
          *(undefined4 *)(param_1 + 0xac) = 0;
          *(undefined4 *)(param_1 + 0xb0) = 0;
        }
      }
    }
LAB_004937b4:
    if (*(int *)(param_1 + 0x48) != 0) {
LAB_00493827:
      (**(code **)(**(int **)(param_1 + 0xc) + 0x144))();
      pcVar1 = *(char **)(param_1 + 0x48);
      local_32c = 6000;
      pcVar11 = pcVar1;
      do {
        local_2cc = pcVar11;
        if ((pcVar1 == (char *)0x0) || ((*(uint *)(param_1 + 0x4c) & 0x10000) != 0))
        goto LAB_00493d81;
        if ((pcVar1 < pcVar11) &&
           ((iVar4 = FUN_0058af6f((int)*pcVar11), iVar4 == 0 &&
            (iVar4 = FUN_0058af6f((int)pcVar11[-1]), iVar4 == 0)))) {
          pcVar11 = pcVar11 + -1;
        }
        FUN_00478a10();
        FUN_00479680();
        if (local_2e4 != 0) {
          (*(code *)(*local_31c)[4])(local_2e4);
          FUN_00479580();
        }
        local_2e4 = 0;
        if (local_318 == 7) {
          if (local_310 != 0x3a) goto LAB_004938e7;
          FUN_004795c0();
        }
        else if (((local_318 == 4) || (local_318 == 3)) || (local_318 == 2)) {
          uVar8 = FUN_0041e8e0(*(undefined4 *)(param_1 + 0xc),&local_328,0,param_1);
          if ((uVar8 & 0x20) != 0) {
            local_4 = 3;
            FUN_004830f0(local_300);
            if (local_324 != 0) {
              FUN_004830f0(local_31c);
              FUN_004830f0(local_324);
              goto LAB_00493e1c;
            }
            if (local_320 == (int *)0x0) goto LAB_00493e1c;
            FUN_004830f0(local_31c);
            goto LAB_00493e14;
          }
          if ((uVar8 & 0x40) == 0) {
            if ((uVar8 & 0x80) != 0) {
              FUN_004795a0();
              iVar4 = FUN_00479700(s_BEGIN_005da154,0);
              if (iVar4 == 0) {
                FUN_004795c0();
              }
              else {
                iVar4 = FUN_004795f0();
                if (iVar4 == 0) {
                  FUN_0058b100(auStack_d4,s_Script_error_at_line__d___s_005da0d0,0,
                               s_BEGIN_without_matching_END_005da0f0);
                  if (DAT_00668154 == 0) {
                    FUN_0054d170(&DAT_0065c5d0,auStack_d4);
                  }
                  else {
                    FUN_0041ee50(auStack_d4);
                  }
                }
              }
              *(undefined4 *)(param_1 + 0x58 + *(int *)(param_1 + 0xa4) * 8) = 0;
            }
          }
          else {
            *(undefined4 *)(param_1 + 0x58 + *(int *)(param_1 + 0xa4) * 8) = 1;
          }
          if ((uVar8 & 0x100) != 0) {
            iVar4 = *(int *)(param_1 + 0x58 + *(int *)(param_1 + 0xa4) * 8);
            if (iVar4 == 0xdeaf) {
              FUN_0058b100(auStack_19c,s_Script_error_at_line__d___s_005da0d0,local_2f8,
                           s_ELSE_without_matching_IF_005da15c);
              if (DAT_00668154 == 0) {
                FUN_0054d170(&DAT_0065c5d0,auStack_19c);
              }
              else {
                FUN_0041ee50(auStack_19c);
              }
            }
            else if (iVar4 == 1) {
              FUN_004795a0();
              iVar4 = FUN_00479700(s_BEGIN_005da178,0);
              if (iVar4 == 0) {
                FUN_004795c0();
              }
              else {
                iVar4 = FUN_004795f0();
                if (iVar4 == 0) {
                  FUN_0058b100(auStack_264,s_Script_error_at_line__d___s_005da0d0,0,
                               s_BEGIN_without_matching_END_005da0f0);
                  if (DAT_00668154 == 0) {
                    FUN_0054d170(&DAT_0065c5d0,auStack_264);
                  }
                  else {
                    FUN_0041ee50(auStack_264);
                  }
                }
              }
              *(undefined4 *)(param_1 + 0x58 + *(int *)(param_1 + 0xa4) * 8) = 0xdeaf;
            }
            iVar4 = FUN_00479700(&DAT_005da180,0);
            if (iVar4 != 0) {
              (*(code *)(*local_31c)[4])(pcVar11);
              FUN_00478a10();
              FUN_00479680();
              local_2e4 = (**(code **)(*local_320 + 0xc))();
              FUN_00478a10();
            }
          }
          if (((uVar8 & 0x200) != 0) && (iVar4 = FUN_004795f0(), iVar4 == 0)) {
            FUN_0058b100(auStack_200,s_Script_error_at_line__d___s_005da0d0,0,
                         s_BEGIN_without_matching_END_005da0f0);
            if (DAT_00668154 == 0) {
              FUN_0054d170(&DAT_0065c5d0,auStack_200);
            }
            else {
              FUN_0041ee50(auStack_200);
            }
          }
          if ((uVar8 & 0x800) != 0) {
            iVar4 = *(int *)(param_1 + 0xa4) + 1;
            *(int *)(param_1 + 0xa4) = iVar4;
            *(undefined4 *)(param_1 + 0x54 + iVar4 * 8) = 0;
            *(undefined4 *)(param_1 + 0x58 + *(int *)(param_1 + 0xa4) * 8) = 0xdeaf;
          }
          if (((uVar8 & 0x1000) != 0) &&
             (iVar4 = *(int *)(param_1 + 0xa4) + -1, *(int *)(param_1 + 0xa4) = iVar4, iVar4 < 0)) {
            FUN_0058b100(auStack_138,s_Script_error_at_line__d___s_005da0d0,local_2f8,
                         s_END_without_matching_BEGIN_005da184);
            if (DAT_00668154 == 0) {
              FUN_0054d170(&DAT_0065c5d0,auStack_138);
            }
            else {
              FUN_0041ee50(auStack_138);
            }
          }
          pcVar1 = *(char **)(param_1 + 0x54 + *(int *)(param_1 + 0xa4) * 8);
          if (pcVar1 != (char *)0x0) {
            *(undefined4 *)(param_1 + 0x54 + *(int *)(param_1 + 0xa4) * 8) = 0;
            local_2cc = pcVar1;
          }
          if ((uVar8 & 0x400) != 0) {
            *(char **)(param_1 + 0x54 + *(int *)(param_1 + 0xa4) * 8) = pcVar11;
          }
          if (((uVar8 & 1) != 0) || (*(char *)(param_1 + 0xb4) != '\0')) {
            *(char **)(param_1 + 0x48) = local_2cc;
LAB_00493d81:
            if (*(int *)(param_1 + 0x48) == 0) {
              if (local_2e8 != 0) {
                if (*(int **)(param_1 + 0xc) != (int *)0x0) {
                  (**(code **)(**(int **)(param_1 + 0xc) + 0x14c))();
                }
                if (*(int **)(param_1 + 0xc4) != (int *)0x0) {
                  (**(code **)(**(int **)(param_1 + 0xc4) + 0x14c))();
                }
                if (*(int **)(param_1 + 200) != (int *)0x0) {
                  (**(code **)(**(int **)(param_1 + 200) + 0x14c))();
                }
              }
              *(undefined4 *)(param_1 + 0x50) = 0;
              FUN_00493e40_TScript_End();
            }
            local_4 = 4;
            FUN_004830f0(local_300);
            if (local_324 == 0) {
              if (local_320 != (int *)0x0) {
                FUN_004830f0(local_31c);
LAB_00493e14:
                FUN_004a1540(local_320);
              }
            }
            else {
              FUN_004830f0(local_31c);
              FUN_004830f0(local_324);
            }
LAB_00493e1c:
            local_4 = 0xffffffff;
            FUN_00478730();
            ExceptionList = local_c;
            return 1;
          }
          if ((uVar8 & 0x2000) != 0) {
            local_2cc = *(char **)(param_1 + 0x48);
          }
        }
        else {
LAB_004938e7:
          FUN_0058b100(auStack_2c8,s_Script_error_at_line__d___s_005da0d0,local_2f8,
                       s_Bad_token_in_trigger_block_005da1a0);
          if (DAT_00668154 == 0) {
            FUN_0054d170(&DAT_0065c5d0,auStack_2c8);
          }
          else {
            FUN_0041ee50(auStack_2c8);
          }
        }
        while ((local_318 != 9 && (local_318 != 10))) {
          FUN_00478a10();
        }
        if (*(int *)(param_1 + 0xa4) < 1) {
          if (local_32c < 1) goto LAB_00493cbd;
LAB_00493d0c:
          *(undefined4 *)(param_1 + 0x48) = 0;
          *(undefined4 *)(param_1 + 0x4c) = 0;
        }
        else {
          iVar4 = local_32c + -1;
          bVar12 = local_32c < 1;
          local_32c = iVar4;
          if (bVar12) {
LAB_00493cbd:
            FUN_0058b100(auStack_70,s_Script_error_at_line__d___s_005da0d0,local_2f8,
                         s_Infinite_loop_detected_005da1bc);
            if (DAT_00668154 == 0) {
              FUN_0054d170(&DAT_0065c5d0,auStack_70);
            }
            else {
              FUN_0041ee50(auStack_70);
            }
            goto LAB_00493d0c;
          }
        }
        pcVar1 = *(char **)(param_1 + 0x48);
        pcVar11 = local_2cc;
      } while( true );
    }
    *(undefined4 *)(param_1 + 0x10) = 0xffffffff;
    local_4 = 2;
    FUN_004830f0(local_300);
    if (local_324 == 0) {
      if (local_320 != (int *)0x0) {
        FUN_004830f0(local_31c);
        FUN_004a1540(local_320);
      }
    }
    else {
      FUN_004830f0(local_31c);
      FUN_004830f0(local_324);
    }
    local_4 = 0xffffffff;
    FUN_00478730();
  }
  ExceptionList = local_c;
  return 0;
}


