// FUN_005475e0 @ 005475e0 size=6689

/* WARNING: Type propagation algorithm not settling */

undefined4 __fastcall FUN_005475e0(int *param_1)

{
  char cVar1;
  void **ppvVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  int *piVar6;
  int *piVar7;
  int iVar8;
  LPSTR pCVar9;
  uint uVar10;
  int unaff_EBX;
  undefined4 *puVar11;
  char *pcVar12;
  LPSTR pCVar13;
  LPCSTR pCVar14;
  bool bVar15;
  LPSTR lpchText;
  HDC hdc;
  UINT align;
  int iStack_228;
  int local_224;
  int local_220;
  int local_21c;
  undefined4 uStack_218;
  int iStack_214;
  int iStack_210;
  undefined ***pppuStack_20c;
  int iStack_208;
  undefined4 uStack_204;
  undefined4 uStack_200;
  undefined4 uStack_1f8;
  undefined4 uStack_1f4;
  char *pcStack_1f0;
  undefined1 uStack_1ec;
  undefined4 uStack_1e8;
  undefined4 local_1d8;
  int iStack_1d4;
  int local_1d0;
  int iStack_1cc;
  undefined4 *puStack_1c8;
  undefined1 uStack_1c4;
  undefined1 uStack_1c3;
  undefined1 uStack_1c2;
  undefined1 uStack_1c1;
  int local_1c0;
  undefined1 uStack_1bc;
  undefined1 uStack_1bb;
  undefined1 uStack_1ba;
  undefined1 uStack_1b9;
  undefined1 uStack_1b8;
  undefined1 uStack_1b7;
  undefined1 uStack_1b6;
  undefined1 uStack_1b5;
  undefined1 uStack_1b4;
  undefined1 uStack_1b3;
  undefined1 uStack_1b2;
  undefined1 uStack_1b1;
  int iStack_1b0;
  int iStack_1ac;
  int iStack_1a8;
  int local_1a4;
  int local_1a0;
  int iStack_19c;
  int iStack_198;
  int iStack_194;
  int iStack_190;
  int iStack_18c;
  int iStack_188;
  undefined4 uStack_184;
  int iStack_180;
  int iStack_17c;
  undefined4 uStack_178;
  undefined4 uStack_174;
  undefined4 uStack_170;
  int iStack_16c;
  tagDRAWTEXTPARAMS tStack_168;
  tagDRAWTEXTPARAMS tStack_154;
  tagRECT tStack_140;
  tagRECT tStack_130;
  undefined **ppuStack_120;
  char *pcStack_11c;
  int iStack_118;
  int iStack_114;
  int iStack_110;
  tagSIZE tStack_10c;
  undefined4 auStack_104 [8];
  undefined1 auStack_e4 [20];
  undefined1 auStack_d0 [32];
  char acStack_b0 [9];
  undefined1 auStack_a7 [63];
  undefined1 auStack_68 [92];
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_005a2085;
  pvStack_c = ExceptionList;
  local_220 = 0x14;
  local_224 = 0x14;
  local_1a4 = 0xc;
  local_1c0 = -1;
  local_21c = 0x14;
  local_1d8 = CONCAT13(local_1d8._3_1_,0xffffff);
  local_1d0 = 0;
  local_1a0 = 1;
  ppvVar2 = &pvStack_c;
  if (param_1[0x66] == 0) {
    if (DAT_00667fcc == 0) {
      return 0;
    }
    ExceptionList = &pvStack_c;
    (**(code **)(*param_1 + 0xb8))(DAT_00667fcc);
    ppvVar2 = ExceptionList;
  }
  ExceptionList = ppvVar2;
  iStack_118 = param_1[0x68];
  if (iStack_118 == 0) {
    local_21c = 0xffffff;
    FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&local_21c,0,0);
    ExceptionList = pvStack_c;
    return 0;
  }
  ppuStack_120 = &PTR_LAB_005a36f8;
  iStack_114 = param_1[0x69] + iStack_118;
  pcStack_11c = s_String_005e54b8;
  iStack_110 = iStack_118;
  FUN_00478720();
  pppuStack_20c = &ppuStack_120;
  uStack_4 = 0;
  uStack_218 = 0;
  iStack_214 = 0;
  iStack_210 = 0;
  iStack_208 = 0;
  uStack_204 = 0;
  uStack_200 = 0;
  uStack_1f8 = 0;
  uStack_1f4 = 0;
  uStack_1ec = 0;
  uStack_1e8 = 1;
  pcStack_1f0 = (char *)FUN_00482fb0(0x2000);
  *pcStack_1f0 = '\0';
  uStack_4 = 1;
  FUN_00479450();
  while (iStack_208 != 10) {
    iVar3 = (*(code *)(*pppuStack_20c)[3])();
    iVar4 = FUN_00479700(s_CLASS_005e54c0,0);
    if (iVar4 == 0) {
      local_21c = 0xffffff;
      FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&local_21c,0,0);
      uStack_4 = 2;
LAB_005478c9:
      FUN_004830f0(pcStack_1f0);
      goto joined_r0x005478dc;
    }
    FUN_00479580();
    if (iStack_208 != 2) {
      local_21c = 0xffffff;
      FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&local_21c,0,0);
      uStack_4 = 3;
      goto LAB_005478c9;
    }
    iVar4 = FUN_00479700(s_default_005e54c8,0);
    if ((iVar4 == 0) &&
       (iVar4 = FUN_00483460(pcStack_1f0,*(undefined4 *)(*(int *)(param_1[0x66] + 0x48) + 4)),
       iVar4 == 0)) {
      FUN_004795c0();
    }
    else {
      FUN_00479580();
      bVar15 = true;
      iVar4 = FUN_00479700(s_FORMAT_005e54d0,0);
      if (iVar4 != 0) {
        FUN_00479580();
        iVar4 = FUN_00479700(param_1[0x67],0);
        bVar15 = iVar4 != 0;
        FUN_00479580();
      }
      FUN_004795a0();
      if (bVar15) {
        if (iVar3 == param_1[0x6a]) {
          local_1a0 = 0;
        }
        param_1[0x6a] = iVar3;
        if (local_1a0 == 0) goto LAB_0054791e;
        break;
      }
    }
    FUN_004795f0();
    FUN_004795a0();
  }
  FUN_00436890();
LAB_0054791e:
  iVar3 = FUN_00479700(s_BEGIN_005e54d8,0);
  if (iVar3 == 0) {
    local_21c = 0xffffff;
    FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&local_21c,0,0);
    uStack_4 = 4;
    FUN_004830f0(pcStack_1f0);
joined_r0x005478dc:
    if (iStack_214 == 0) {
      if (iStack_210 != 0) {
        FUN_004830f0(pppuStack_20c);
LAB_00548dc3:
        FUN_004a1540(iStack_210);
      }
    }
    else {
      FUN_004830f0(pppuStack_20c);
      FUN_004830f0(iStack_214);
    }
LAB_00548dcb:
    uStack_4 = 0xffffffff;
    FUN_00478730();
    ExceptionList = pvStack_c;
    return 0;
  }
  FUN_004795a0();
  iVar3 = FUN_00479700(&DAT_005e54e0,0);
  do {
    if (iVar3 != 0) {
      FUN_004795a0();
      uStack_4 = 0x11;
      FUN_004830f0(pcStack_1f0);
      if (iStack_214 == 0) {
        if (iStack_210 != 0) {
          FUN_004830f0(pppuStack_20c);
          FUN_004a1540(iStack_210);
        }
      }
      else {
        FUN_004830f0(pppuStack_20c);
        FUN_004830f0(iStack_214);
      }
      uStack_4 = 0xffffffff;
      FUN_00478730();
      ExceptionList = pvStack_c;
      return 1;
    }
    iVar3 = FUN_00479700(s_BUTTON_005e54e4,0);
    if (iVar3 == 0) {
      iVar3 = FUN_00479700(&DAT_005e5500,0);
      if (iVar3 == 0) {
        iVar3 = FUN_00479700(s_LINEHEIGHT_005e5508,0);
        if (iVar3 == 0) {
          iVar3 = FUN_00479700(s_LINEWRAP_005e5518,0);
          if (iVar3 == 0) {
            iVar3 = FUN_00479700(s_COLOR_005e5530,0);
            if (iVar3 == 0) {
              iVar3 = FUN_00479700(&DAT_005e5544,0);
              if (iVar3 == 0) {
                iVar3 = FUN_00479700(&DAT_005e5550,0);
                if (iVar3 == 0) {
                  iVar3 = FUN_00479700(&PTR_LAB_005e5560,0);
                  if (iVar3 == 0) {
                    iVar3 = FUN_00479700(s_NEXTLINE_005e5568,0);
                    if (iVar3 == 0) {
                      iVar3 = FUN_00479700(&DAT_005e5574,0);
                      if ((iVar3 == 0) && (iVar3 = FUN_00479700(s_FIELD_005e557c,0), iVar3 == 0)) {
                        iStack_228 = 0xffffff;
                        FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&iStack_228,0,0);
                        uStack_4 = 0x10;
                        FUN_004830f0(pcStack_1f0);
                        if (iStack_214 != 0) {
                          FUN_004830f0(pppuStack_20c);
                          FUN_004830f0(iStack_214);
                          goto LAB_00548dcb;
                        }
                        if (iStack_210 == 0) goto LAB_00548dcb;
                        FUN_004830f0(pppuStack_20c);
                        goto LAB_00548dc3;
                      }
                      iStack_190 = FUN_00479700(s_FIELD_005e5584,0);
                      iStack_1ac = 1;
                      iStack_19c = 0;
                      iStack_188 = 0;
                      iStack_1b0 = 0;
                      iStack_198 = 0;
                      iStack_18c = 0;
                      FUN_00479580();
                      iVar3 = FUN_00479700(s_block_005e558c,0);
                      if (iVar3 == 0) {
                        iVar3 = FUN_00479700(s_multipage_005e559c,0);
                        if (iVar3 == 0) {
                          iVar3 = FUN_00479700(s_grayable_005e55b0,0);
                          if (iVar3 == 0) {
                            iVar3 = FUN_00479700(s_prevbutton_005e55bc,0);
                            if (iVar3 == 0) {
                              iVar3 = FUN_00479700(s_nextbutton_005e55c8,0);
                              if (iVar3 == 0) {
                                iVar3 = FUN_00479700(s_center_005e55d4,0);
                                if (iVar3 == 0) {
                                  iVar3 = FUN_00479700(s_right_005e55dc,0);
                                  if (iVar3 == 0) {
                                    iVar3 = FUN_00479700(&DAT_005e55e4,0);
                                    if (iVar3 == 0) {
                                      SetTextAlign((HDC)param_1[0x5f],1);
                                    }
                                    else {
                                      SetTextAlign((HDC)param_1[0x5f],1);
                                      FUN_00479580();
                                    }
                                    goto LAB_005481fa;
                                  }
                                  hdc = (HDC)param_1[0x5f];
                                  align = 3;
                                }
                                else {
                                  hdc = (HDC)param_1[0x5f];
                                  align = 7;
                                }
                                SetTextAlign(hdc,align);
                                FUN_00479580();
                                iStack_1ac = 0;
                                goto LAB_005481fa;
                              }
                              iStack_18c = 1;
                            }
                            else {
                              iStack_198 = 1;
                            }
                          }
                          iStack_1b0 = 1;
                          FUN_00479580();
                        }
                        else {
                          FUN_00479580();
                          iVar3 = FUN_0047a410(&uStack_218,s__d__d_005e55a8,&iStack_1cc,&iStack_1d4)
                          ;
                          if (iVar3 == 0) {
                            iStack_228 = 0xffffff;
                            FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&iStack_228,0,0);
                            uStack_4 = 0xe;
                            FUN_004830f0(pcStack_1f0);
                            if (iStack_214 == 0) {
                              if (iStack_210 != 0) {
                                FUN_004830f0(pppuStack_20c);
                                FUN_004a1540(iStack_210);
                              }
                            }
                            else {
                              FUN_004830f0(pppuStack_20c);
                              FUN_004830f0(iStack_214);
                            }
                            uStack_4 = 0xffffffff;
                            FUN_00478730();
                            ExceptionList = pvStack_c;
                            return 0;
                          }
                          iStack_188 = 1;
                          iStack_19c = 1;
                        }
                      }
                      else {
                        FUN_00479580();
                        iVar3 = FUN_0047a410(&uStack_218,s__d__d_005e5594,&iStack_1cc,&iStack_1d4);
                        if (iVar3 == 0) {
                          iStack_228 = 0xffffff;
                          FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&iStack_228,0,0);
                          uStack_4 = 0xd;
                          FUN_004830f0(pcStack_1f0);
                          if (iStack_214 == 0) {
                            if (iStack_210 != 0) {
                              FUN_004830f0(pppuStack_20c);
                              FUN_004a1540(iStack_210);
                            }
                          }
                          else {
                            FUN_004830f0(pppuStack_20c);
                            FUN_004830f0(iStack_214);
                          }
                          uStack_4 = 0xffffffff;
                          FUN_00478730();
                          ExceptionList = pvStack_c;
                          return 0;
                        }
                        iStack_19c = 1;
                      }
LAB_005481fa:
                      iVar3 = local_1d0;
                      puStack_1c8 = &local_1d8;
                      if ((iStack_208 == 4) && (iVar4 = 0, 0 < local_1d0)) {
                        pcVar12 = acStack_b0;
                        do {
                          iVar8 = FUN_0059a530(pcVar12,pcStack_1f0);
                          if (iVar8 == 0) {
                            puStack_1c8 = auStack_104 + iVar4;
                            FUN_00479580();
                            break;
                          }
                          iVar4 = iVar4 + 1;
                          pcVar12 = pcVar12 + 10;
                        } while (iVar4 < iVar3);
                      }
                      iStack_194 = 1;
                      if (iStack_190 == 0) {
                        iVar3 = param_1[0x65];
                        pcVar12 = (char *)param_1[100];
                        _strncpy(pcVar12,pcStack_1f0,iVar3 - 1);
                        pcVar12[iVar3 + -1] = '\0';
                      }
                      else {
                        pcVar12 = (char *)param_1[100];
                        iVar4 = 0;
                        *pcVar12 = '\0';
                        if (iStack_208 == 2) {
                          iVar4 = param_1[0x65];
                          _strncpy(pcVar12,pcStack_1f0,iVar4 - 1);
                          pcVar12[iVar4 + -1] = '\0';
                          FUN_00479580();
                          uVar10 = 0xffffffff;
                          pcVar12 = (char *)param_1[100];
                          do {
                            if (uVar10 == 0) break;
                            uVar10 = uVar10 - 1;
                            cVar1 = *pcVar12;
                            pcVar12 = pcVar12 + 1;
                          } while (cVar1 != '\0');
                          iVar4 = ~uVar10 - 1;
                        }
                        iVar8 = FUN_0047a410(&uStack_218,&DAT_005e55ec,auStack_d0);
                        if (iVar8 == 0) {
                          iStack_228 = 0xffffff;
                          FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&iStack_228,0,0);
                          uStack_4 = 0xf;
                          FUN_004830f0(pcStack_1f0);
                          if (iStack_214 == 0) {
                            if (iStack_210 != 0) {
                              FUN_004830f0(pppuStack_20c);
                              FUN_004a1540(iStack_210);
                            }
                          }
                          else {
                            FUN_004830f0(pppuStack_20c);
                            FUN_004830f0(iStack_214);
                          }
                          uStack_4 = 0xffffffff;
                          FUN_00478730();
                          ExceptionList = pvStack_c;
                          return 0;
                        }
                        iStack_1a8 = 0;
                        iStack_194 = FUN_00547240(auStack_d0,param_1[100] + iVar4,
                                                  param_1[0x65] - iVar4,&iStack_1a8);
                        iVar4 = local_1d0;
                        if (((iStack_194 != 0) && (iStack_1a8 != 0)) && (iVar8 = 0, 0 < iVar3)) {
                          pcVar12 = acStack_b0;
                          do {
                            iVar3 = FUN_0059a530(pcVar12,iStack_1a8);
                            if (iVar3 == 0) {
                              puStack_1c8 = auStack_104 + iVar8;
                              break;
                            }
                            iVar8 = iVar8 + 1;
                            pcVar12 = pcVar12 + 10;
                          } while (iVar8 < iVar4);
                        }
                        if (iStack_208 == 2) {
                          uVar10 = 0xffffffff;
                          pcVar12 = (char *)param_1[100];
                          do {
                            if (uVar10 == 0) break;
                            uVar10 = uVar10 - 1;
                            cVar1 = *pcVar12;
                            pcVar12 = pcVar12 + 1;
                          } while (cVar1 != '\0');
                          iVar3 = param_1[0x65] - (~uVar10 - 1);
                          pcVar12 = (char *)param_1[100] + (~uVar10 - 1);
                          _strncpy(pcVar12,pcStack_1f0,iVar3 - 1);
                          pcVar12[iVar3 + -1] = '\0';
                          FUN_00479580();
                        }
                      }
                      FUN_004795a0();
                      if ((0 < local_1c0) && (iStack_1ac != 0)) {
                        uVar10 = 0xffffffff;
                        pCVar14 = (LPCSTR)param_1[100];
                        do {
                          if (uVar10 == 0) break;
                          uVar10 = uVar10 - 1;
                          cVar1 = *pCVar14;
                          pCVar14 = pCVar14 + 1;
                        } while (cVar1 != '\0');
                        GetTextExtentExPointA
                                  ((HDC)param_1[0x5f],(LPCSTR)param_1[100],~uVar10 - 1,10000,
                                   (LPINT)0x0,(LPINT)0x0,&tStack_10c);
                        if (local_1c0 < tStack_10c.cx + local_220) {
                          local_224 = local_224 + local_1a4;
                          local_220 = local_21c;
                          MoveToEx((HDC)param_1[0x5f],local_21c,local_224,(LPPOINT)0x0);
                        }
                      }
                      if ((iStack_190 == 0) || (iStack_194 != 0)) {
                        if (iStack_188 == 0) {
                          if (iStack_19c == 0) {
                            if (iStack_1b0 == 0) {
                              FUN_00546de0(local_220,local_224,param_1[100],puStack_1c8,&local_220,
                                           &local_224);
                            }
                            else {
                              iVar3 = FUN_0059a530(param_1[100],&DAT_005e55f4);
                              if ((iVar3 == 0) || (iStack_18c != 0)) {
                                if (param_1[0x6b] < param_1[0x6c]) {
                                  uStack_1b2 = 6;
                                  uStack_1b3 = 0xdd;
                                  uStack_1b4 = 0;
                                  uStack_1b1 = 0;
                                  FUN_00546de0(local_220,local_224,param_1[100],&uStack_1b4,
                                               &local_220,&local_224);
                                }
                                else {
                                  iStack_228 = 0x3c3c3c;
                                  FUN_00546de0(local_220,local_224,param_1[100],&iStack_228,
                                               &local_220,&local_224);
                                }
                              }
                              else {
                                iVar3 = FUN_0059a530(param_1[100],&DAT_005e55fc);
                                if ((iVar3 == 0) || (iStack_198 != 0)) {
                                  if (param_1[0x6b] < 1) {
                                    uStack_1b6 = 0x3c;
                                    uStack_1b7 = 0x3c;
                                    uStack_1b8 = 0x3c;
                                    uStack_1b5 = 0;
                                    FUN_00546de0(local_220,local_224,param_1[100],&uStack_1b8,
                                                 &local_220,&local_224);
                                  }
                                  else {
                                    uStack_1c2 = 6;
                                    uStack_1c3 = 0xdd;
                                    uStack_1c4 = 0;
                                    uStack_1c1 = 0;
                                    FUN_00546de0(local_220,local_224,param_1[100],&uStack_1c4,
                                                 &local_220,&local_224);
                                  }
                                }
                              }
                            }
                            goto LAB_005488ba;
                          }
                          lpchText = (LPSTR)param_1[100];
                        }
                        else {
                          lpchText = (LPSTR)param_1[100];
                          uVar10 = 0xffffffff;
                          pCVar13 = lpchText;
                          do {
                            if (uVar10 == 0) break;
                            uVar10 = uVar10 - 1;
                            cVar1 = *pCVar13;
                            pCVar13 = pCVar13 + 1;
                          } while (cVar1 != '\0');
                          tStack_168.iTabLength = 0;
                          tStack_140.right = iStack_1cc + local_220;
                          tStack_168.iLeftMargin = 0;
                          tStack_168.iRightMargin = 0;
                          tStack_168.uiLengthDrawn = 0;
                          tStack_140.top = local_224;
                          tStack_140.bottom = iStack_1d4 + local_224;
                          tStack_168.cbSize = 0x14;
                          tStack_140.left = local_220;
                          DrawTextExA((HDC)param_1[0x60],lpchText,~uVar10 - 1,&tStack_140,0x2810,
                                      &tStack_168);
                          uVar10 = 0xffffffff;
                          pcVar12 = (char *)param_1[100];
                          do {
                            if (uVar10 == 0) break;
                            uVar10 = uVar10 - 1;
                            cVar1 = *pcVar12;
                            pcVar12 = pcVar12 + 1;
                          } while (cVar1 != '\0');
                          if (tStack_168.uiLengthDrawn < ~uVar10 - 1) {
                            pCVar13 = (char *)param_1[100] + tStack_168.uiLengthDrawn;
                            param_1[0x6c] = 1;
                            if (pCVar13 == (LPSTR)0x0) goto LAB_00548808;
                            uVar10 = 0xffffffff;
                            tStack_154.iTabLength = 0;
                            tStack_154.cbSize = 0x14;
                            pCVar9 = pCVar13;
                            do {
                              if (uVar10 == 0) break;
                              uVar10 = uVar10 - 1;
                              cVar1 = *pCVar9;
                              pCVar9 = pCVar9 + 1;
                            } while (cVar1 != '\0');
                            tStack_154.iLeftMargin = 0;
                            tStack_130.left = local_220;
                            tStack_154.iRightMargin = 0;
                            tStack_130.right = iStack_1cc + local_220;
                            tStack_154.uiLengthDrawn = 0;
                            tStack_130.top = local_224;
                            tStack_130.bottom = iStack_1d4 + local_224;
                            DrawTextExA((HDC)param_1[0x60],pCVar13,~uVar10 - 1,&tStack_130,0x2810,
                                        &tStack_154);
                            uVar10 = 0xffffffff;
                            pCVar9 = pCVar13;
                            do {
                              if (uVar10 == 0) break;
                              uVar10 = uVar10 - 1;
                              cVar1 = *pCVar9;
                              pCVar9 = pCVar9 + 1;
                            } while (cVar1 != '\0');
                            if (~uVar10 - 1 <= tStack_154.uiLengthDrawn) goto LAB_00548808;
                            param_1[0x6c] = 2;
                            pCVar9 = pCVar13 + tStack_154.uiLengthDrawn;
                          }
                          else {
                            pCVar13 = (LPSTR)0x0;
                            param_1[0x6c] = 0;
LAB_00548808:
                            pCVar9 = (LPSTR)0x0;
                          }
                          if (param_1[0x6c] < param_1[0x6b]) {
                            param_1[0x6b] = param_1[0x6c];
                            pCVar9 = (LPSTR)0x0;
                          }
                          iVar3 = param_1[0x6b];
                          if ((((iVar3 != 0) && (lpchText = pCVar13, iVar3 != 1)) &&
                              (lpchText = pCVar9, iVar3 != 2)) || (lpchText == (LPSTR)0x0))
                          goto LAB_005488ba;
                        }
                        FUN_00546f40(local_220,local_224,iStack_1cc,iStack_1d4,lpchText,&local_1d8,
                                     &local_220,&local_224);
                      }
                    }
                    else {
                      FUN_00479580();
                      FUN_004795a0();
                      local_224 = local_224 + local_1a4;
                      local_220 = local_21c;
                      MoveToEx((HDC)param_1[0x5f],local_21c,local_224,(LPPOINT)0x0);
                    }
                  }
                  else {
                    FUN_00479580();
                    iVar3 = FUN_0047a410(&uStack_218,&DAT_005e5564,&iStack_17c);
                    if (iVar3 == 0) {
                      iStack_228 = 0xffffff;
                      FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&iStack_228,0,0);
                      uStack_4 = 0xc;
                      FUN_004830f0(pcStack_1f0);
                      if (iStack_214 == 0) {
                        if (iStack_210 != 0) {
                          FUN_004830f0(pppuStack_20c);
                          FUN_004a1540(iStack_210);
                        }
                      }
                      else {
                        FUN_004830f0(pppuStack_20c);
                        FUN_004830f0(iStack_214);
                      }
                      uStack_4 = 0xffffffff;
                      FUN_00478730();
                      ExceptionList = pvStack_c;
                      return 0;
                    }
                    local_220 = iStack_17c + local_21c;
                    MoveToEx((HDC)param_1[0x5f],local_220,local_224,(LPPOINT)0x0);
                  }
                }
                else {
                  FUN_00479580();
                  iVar3 = FUN_0047a410(&uStack_218,s__d__d_005e5558,&iStack_16c,&iStack_180);
                  if (iVar3 == 0) {
                    iStack_228 = 0xffffff;
                    FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&iStack_228,0,0);
                    uStack_4 = 0xb;
                    FUN_004830f0(pcStack_1f0);
                    if (iStack_214 == 0) {
                      if (iStack_210 != 0) {
                        FUN_004830f0(pppuStack_20c);
                        FUN_004a1540(iStack_210);
                      }
                    }
                    else {
                      FUN_004830f0(pppuStack_20c);
                      FUN_004830f0(iStack_214);
                    }
                    uStack_4 = 0xffffffff;
                    FUN_00478730();
                    ExceptionList = pvStack_c;
                    return 0;
                  }
                  local_224 = local_224 + iStack_180;
                  local_220 = local_220 + iStack_16c;
                  local_21c = local_220;
                  MoveToEx((HDC)param_1[0x5f],local_220,local_224,(LPPOINT)0x0);
                }
              }
              else {
                FUN_00479580();
                iVar3 = FUN_0047a410(&uStack_218,s__d__d_005e5548,&local_220,&local_224);
                if (iVar3 == 0) {
                  iStack_228 = 0xffffff;
                  FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&iStack_228,0,0);
                  uStack_4 = 10;
                  FUN_004830f0(pcStack_1f0);
                  if (iStack_214 == 0) {
                    if (iStack_210 != 0) {
                      FUN_004830f0(pppuStack_20c);
                      FUN_004a1540(iStack_210);
                    }
                  }
                  else {
                    FUN_004830f0(pppuStack_20c);
                    FUN_004830f0(iStack_214);
                  }
                  uStack_4 = 0xffffffff;
                  FUN_00478730();
                  ExceptionList = pvStack_c;
                  return 0;
                }
                local_21c = local_220;
                MoveToEx((HDC)param_1[0x5f],local_220,local_224,(LPPOINT)0x0);
              }
            }
            else {
              FUN_00479580();
              iVar3 = -1;
              puVar11 = &local_1d8;
              if (iStack_208 == 4) {
                iVar3 = 0;
                if (local_1d0 < 1) {
LAB_00547dcd:
                  _strncpy(acStack_b0 + iVar3 * 10,pcStack_1f0,9);
                  local_1d0 = iVar3 + 1;
                  auStack_a7[iVar3 * 10] = 0;
                  puVar11 = auStack_104 + iVar3;
                }
                else {
                  pcVar12 = acStack_b0;
                  do {
                    iVar4 = FUN_0059a530(pcVar12,pcStack_1f0);
                    if (iVar4 == 0) {
                      puVar11 = auStack_104 + iVar3;
                      if (iVar3 < local_1d0) goto LAB_00547df9;
                      break;
                    }
                    iVar3 = iVar3 + 1;
                    pcVar12 = pcVar12 + 10;
                  } while (iVar3 < local_1d0);
                  if (iVar3 < 8) goto LAB_00547dcd;
                }
LAB_00547df9:
                FUN_00479580();
              }
              if (iStack_208 == 8) {
                iVar3 = FUN_0047a410(&uStack_218,s__b__b__b_005e5538,(int)puVar11 + 2,
                                     (int)puVar11 + 1,puVar11);
                if (iVar3 == 0) {
                  iStack_228 = 0xffffff;
                  FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&iStack_228,0,0);
                  uStack_4 = 8;
                  FUN_004830f0(pcStack_1f0);
                  if (iStack_214 == 0) {
                    if (iStack_210 != 0) {
                      FUN_004830f0(pppuStack_20c);
                      FUN_004a1540(iStack_210);
                    }
                  }
                  else {
                    FUN_004830f0(pppuStack_20c);
                    FUN_004830f0(iStack_214);
                  }
                  uStack_4 = 0xffffffff;
                  FUN_00478730();
                  ExceptionList = pvStack_c;
                  return 0;
                }
              }
              else {
                if ((iVar3 < 0) || (7 < iVar3)) {
                  iStack_228 = 0xffffff;
                  FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&iStack_228,0,0);
                  uStack_4 = 9;
                  FUN_004830f0(pcStack_1f0);
                  if (iStack_214 == 0) {
                    if (iStack_210 != 0) {
                      FUN_004830f0(pppuStack_20c);
                      FUN_004a1540(iStack_210);
                    }
                  }
                  else {
                    FUN_004830f0(pppuStack_20c);
                    FUN_004830f0(iStack_214);
                  }
                  uStack_4 = 0xffffffff;
                  FUN_00478730();
                  ExceptionList = pvStack_c;
                  return 0;
                }
                local_1d8 = *puVar11;
                FUN_004795a0();
              }
            }
          }
          else {
            FUN_00479580();
            iVar3 = FUN_00479700(&DAT_005e5524,0);
            if (iVar3 == 0) {
              iVar3 = FUN_0047a410(&uStack_218,&DAT_005e552c,&local_1c0);
              if (iVar3 == 0) {
                iStack_228 = 0xffffff;
                FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&iStack_228,0,0);
                uStack_4 = 7;
                FUN_004830f0(pcStack_1f0);
                if (iStack_214 == 0) {
                  if (iStack_210 != 0) {
                    FUN_004830f0(pppuStack_20c);
                    FUN_004a1540(iStack_210);
                  }
                }
                else {
                  FUN_004830f0(pppuStack_20c);
                  FUN_004830f0(iStack_214);
                }
                uStack_4 = 0xffffffff;
                FUN_00478730();
                ExceptionList = pvStack_c;
                return 0;
              }
            }
            else {
              FUN_00479580();
              FUN_004795a0();
              local_1c0 = -1;
            }
          }
        }
        else {
          FUN_00479580();
          iVar3 = FUN_0047a410(&uStack_218,&DAT_005e5514,&local_1a4);
          if (iVar3 == 0) {
            iStack_228 = 0xffffff;
            FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&iStack_228,0,0);
            uStack_4 = 6;
            FUN_004830f0(pcStack_1f0);
            if (iStack_214 == 0) {
              if (iStack_210 != 0) {
                FUN_004830f0(pppuStack_20c);
                FUN_004a1540(iStack_210);
              }
            }
            else {
              FUN_004830f0(pppuStack_20c);
              FUN_004830f0(iStack_214);
            }
            uStack_4 = 0xffffffff;
            FUN_00478730();
            ExceptionList = pvStack_c;
            return 0;
          }
        }
      }
      else {
        FUN_00479580();
        FUN_004795a0();
        piVar6 = (int *)(**(code **)(*(int *)param_1[0x61] + 0xc))();
        iVar3 = (**(code **)(*piVar6 + 0x68))(piVar6,param_1[0x5f]);
        if (iVar3 != 0) {
          FUN_004a90d0(iVar3,s_d__revenant_StatPane_cpp_005e5620,0x360);
        }
        iVar3 = (**(code **)(*(int *)param_1[0x66] + 0x130))();
        local_1d0 = 0;
        if (iVar3 != 0) {
          if ((*(int *)(iVar3 + 0x18) == 0) && (*(short *)(param_1[0x66] + 4) != 8)) {
            iVar4 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))();
            local_1d0 = 1;
            *(uint *)(iVar3 + 0x18) = (-(uint)(iVar4 != 0x10) & 0xffff8400) + 0xf81f;
          }
          if (*(short *)(param_1[0x66] + 4) != 0xc) {
            FUN_004bda20(iStack_228 + 3,unaff_EBX + 3,iVar3,0x10100,0x1f);
          }
          FUN_004bd680(iStack_228,unaff_EBX,iVar3,0x100,0);
          if (*(short *)(param_1[0x66] + 4) == 0xc) {
            iVar4 = param_1[0x62];
            piVar6 = (int *)param_1[0x61];
            FUN_00438d80(auStack_68,iStack_228,unaff_EBX,0,0,*(undefined4 *)(iVar4 + 4),
                         *(undefined4 *)(iVar4 + 8),0x100);
            (**(code **)(*piVar6 + 0x5c))(auStack_68,iVar4,0,0);
          }
          if (local_1d0 != 0) {
            *(undefined4 *)(iVar3 + 0x18) = 0;
          }
        }
        piVar7 = (int *)(**(code **)(*(int *)param_1[0x61] + 0xc))();
        piVar6 = param_1 + 0x5f;
        iVar3 = (**(code **)(*piVar7 + 0x44))(piVar7,piVar6);
        if (iVar3 != 0) {
          FUN_004a90d0(iVar3,s_d__revenant_StatPane_cpp_005e5604,0x354);
        }
        iVar3 = *(int *)(DAT_0065b020 + DAT_0065abc4 * 4);
        if (iVar3 == 0) {
          iVar3 = DAT_0065b024;
        }
        SelectObject((HDC)*piVar6,*(HGDIOBJ *)(iVar3 + 0x38));
        SetViewportOrgEx((HDC)*piVar6,*(int *)(param_1[0x61] + 0x14),*(int *)(param_1[0x61] + 0x18),
                         (LPPOINT)0x0);
      }
    }
    else {
      FUN_00479580();
      iVar3 = FUN_0047a410(&uStack_218,s__20s__d__d__d__d_005e54ec,auStack_e4,&uStack_184,
                           &uStack_170,&uStack_174,&uStack_178);
      if (iVar3 == 0) {
        uStack_1ba = 0xff;
        uStack_1bb = 0xff;
        uStack_1bc = 0xff;
        uStack_1b9 = 0;
        FUN_00546de0(0x14,0x14,s_Invalid_Stat_Layout_005e53dc,&uStack_1bc,0,0);
      }
      if (local_1a0 != 0) {
        iStack_198 = FUN_00482fb0(0x148);
        uStack_4._0_1_ = 5;
        if (iStack_198 == 0) {
          uVar5 = 0;
        }
        else {
          uVar5 = FUN_0042c600(auStack_e4,uStack_184,uStack_170,uStack_174,uStack_178,0,0,0,0,
                               0xffffffff,0x100000,0xffffffff,0);
        }
        uStack_4 = CONCAT31(uStack_4._1_3_,1);
        FUN_00436790(uVar5);
      }
    }
LAB_005488ba:
    iVar3 = FUN_00479700(&DAT_005e54e0,0);
  } while( true );
}


