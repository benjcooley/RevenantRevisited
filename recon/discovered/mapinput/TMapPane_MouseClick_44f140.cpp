// FUN_0044f140 @ 0044f140 size=4997

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0044f140(int param_1,int param_2,int param_3,int param_4)

{
  short sVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  uint uVar8;
  uint uVar9;
  undefined4 *puVar10;
  char *pcVar11;
  undefined4 *puVar12;
  undefined4 uVar13;
  int iVar14;
  float fVar15;
  float fVar16;
  float10 fVar17;
  unkbyte10 Var18;
  uint local_d4;
  float local_d0;
  float local_cc;
  int local_c8;
  float local_c4;
  float local_c0;
  float fStack_bc;
  float fStack_b8;
  uint uStack_b4;
  float local_b0;
  float fStack_ac;
  float fStack_a8;
  uint uStack_a4;
  int local_a0;
  int local_9c;
  int local_98;
  int local_94;
  undefined4 local_90;
  undefined4 uStack_8c;
  uint uStack_88;
  int local_84;
  int *piStack_74;
  
  iVar2 = FUN_004364d0((*(int *)(param_1 + 4) - DAT_0065be54) + param_3,
                       (*(int *)(param_1 + 8) - DAT_0065be58) + param_4);
  if (iVar2 != 0) {
    return;
  }
  if (DAT_00668154 != 0) {
    if (param_2 == 1) {
      if (param_3 < 0) {
        return;
      }
      if (param_4 < 0) {
        return;
      }
      if (*(int *)(param_1 + 0xc) <= param_3) {
        return;
      }
      if (*(int *)(param_1 + 0x10) <= param_4) {
        return;
      }
      if (*(int *)(param_1 + 0x894) == 0) {
        if (0 < (int)DAT_00656f00) {
          if (DAT_00656f00 < 0xb) {
            iVar2 = 0;
          }
          else {
            iVar2 = DAT_00656f10[10];
          }
          local_d4 = *(uint *)(iVar2 + 0x14) >> 0x10 & 1;
          if (local_d4 != 0) {
            local_98 = *(int *)(param_1 + 0x6c) + 1;
            if (*(int *)(param_1 + 0x6c) + -1 <= local_98) {
              local_a0 = local_98 * 0x40;
              do {
                fVar16 = (float)(*(int *)(param_1 + 0x68) + 1);
                if (*(int *)(param_1 + 0x68) + -1 <= (int)fVar16) {
                  local_94 = (int)fVar16 * 0x40;
                  do {
                    local_b0 = fVar16;
                    local_d4 = FUN_00499e10(*(undefined4 *)(param_1 + 0x9c),fVar16,local_98);
                    if (local_d4 != 0) {
                      local_9c = *(int *)(param_1 + 0x8c8);
                      if (0x3f < local_9c) {
                        local_9c = 0x40;
                      }
                      local_9c = local_9c + -1;
                      if (*(int *)(param_1 + 0x8c0) <= local_9c) {
                        do {
                          iVar2 = *(int *)(param_1 + 0x8c4);
                          if (0x3f < iVar2) {
                            iVar2 = 0x40;
                          }
                          iVar2 = iVar2 + -1;
                          if (*(int *)(param_1 + 0x8bc) <= iVar2) {
                            iVar5 = local_a0 + local_9c;
                            fVar15 = (float)((local_94 + iVar2) * 0x10);
                            do {
                              local_d0 = fVar15;
                              local_cc = (float)(iVar5 * 0x10);
                              local_c8 = FUN_00499720(iVar2,local_9c);
                              FUN_0046d7a0(&local_d0,&local_c0,&local_c4);
                              iVar3 = (((int)local_c0 - param_3) - *(int *)(param_1 + 0x78)) / 2;
                              iVar14 = ((int)local_c4 - *(int *)(param_1 + 0x7c)) - param_4;
                              if (iVar3 < 1) {
                                iVar3 = -iVar3;
                              }
                              if (iVar14 < 1) {
                                iVar14 = -iVar14;
                              }
                              if (iVar14 + iVar3 < 8) {
                                *(int *)(param_1 + 0x104) = local_98;
                                *(int *)(param_1 + 0x10c) = local_9c;
                                *(float *)(param_1 + 0x100) = local_b0;
                                *(int *)(param_1 + 0x110) = *(int *)(param_1 + 0x7c) + param_4;
                                *(int *)(param_1 + 0xfc) = local_c8;
                                *(int *)(param_1 + 0x108) = iVar2;
                                local_84 = (int)local_c4 + 0x14;
                                if (*(int *)(param_1 + 0x50) != 0) {
                                  return;
                                }
                                iVar2 = *(int *)(param_1 + 0x130);
                                if (0x3f < iVar2) {
                                  FUN_004546a0();
                                  *(undefined4 *)(param_1 + 0x130) = 0;
                                  return;
                                }
                                piVar6 = (int *)(param_1 + (iVar2 + 0xb) * 0x1c);
                                *piVar6 = (int)local_c0 + -0x14;
                                piVar6[1] = (int)local_c4 + -0x14;
                                piVar6[2] = (int)local_c0 + 0x14;
                                piVar6[3] = local_84;
                                *(undefined4 *)(param_1 + 0x144 + iVar2 * 0x1c) = 3;
                                *(int *)(param_1 + 0x130) = *(int *)(param_1 + 0x130) + 1;
                                return;
                              }
                              iVar2 = iVar2 + -1;
                              fVar15 = (float)((int)fVar15 + -0x10);
                              fVar16 = local_b0;
                            } while (*(int *)(param_1 + 0x8bc) <= iVar2);
                          }
                          local_9c = local_9c + -1;
                        } while (*(int *)(param_1 + 0x8c0) <= local_9c);
                      }
                    }
                    fVar16 = (float)((int)fVar16 + -1);
                    local_94 = local_94 + -0x40;
                  } while (*(int *)(param_1 + 0x68) + -1 <= (int)fVar16);
                }
                local_98 = local_98 + -1;
                local_a0 = local_a0 + -0x40;
              } while (*(int *)(param_1 + 0x6c) + -1 <= local_98);
            }
            goto LAB_0044f78e;
          }
        }
        iVar2 = FUN_00452520(param_3,param_4,0);
        if (iVar2 == 0) {
          return;
        }
        local_d0 = *(float *)(iVar2 + 0x10);
        local_cc = *(float *)(iVar2 + 0x14);
        local_c8 = *(undefined4 *)(iVar2 + 0x18);
        piVar6 = (int *)(param_1 + 0x10c);
        piVar7 = (int *)(param_1 + 0x108);
        FUN_0046d7a0(&local_d0,piVar7,piVar6);
        iVar3 = *(int *)(param_1 + 0x78) - *piVar7;
        *(int *)(param_1 + 0x100) = iVar3;
        iVar5 = DAT_0065c9e0;
        iVar14 = *(int *)(param_1 + 0x7c) - *piVar6;
        *piVar7 = iVar3 + param_3;
        *(int *)(param_1 + 0x104) = iVar14;
        uVar4 = *(undefined4 *)(iVar2 + 0x40);
        *piVar6 = iVar14 + param_4;
        *(int *)(param_1 + 0x110) = local_c8;
        FUN_004405d0(uVar4,iVar5);
        return;
      }
    }
    else {
      if (param_2 == 3) {
        if (param_3 < 0) {
          return;
        }
        if (param_4 < 0) {
          return;
        }
        if (*(int *)(param_1 + 0xc) <= param_3) {
          return;
        }
        if (*(int *)(param_1 + 0x10) <= param_4) {
          return;
        }
        iVar2 = FUN_00452520(param_3,param_4,0);
        if (iVar2 != 0) {
          FUN_004405d0(*(undefined4 *)(iVar2 + 0x40),0);
        }
        FUN_0043f350(s_follow_005d0304);
        return;
      }
      if (param_2 != 4) {
        if (param_2 != 2) {
          return;
        }
        if (*(int *)(param_1 + 0x88c) != 0) {
          if (*(code **)(param_1 + 0x890) != (code *)0x0) {
            (**(code **)(param_1 + 0x890))();
          }
          uVar4 = DAT_0065a28c;
          *(undefined4 *)(param_1 + 0x88c) = 0;
          FUN_0043a020(uVar4);
          *(undefined4 *)(param_1 + 0x894) = 0;
          return;
        }
        if (DAT_00668518 == 1) {
          FUN_00440d20();
        }
        iVar2 = FUN_004406e0();
        while (iVar2 != 0) {
          iVar2 = FUN_004406e0();
        }
        *(undefined4 *)(param_1 + 0x894) = 0;
        return;
      }
      if (*(int *)(param_1 + 0x88c) != 0) {
        FUN_0046dad0((*(int *)(param_1 + 0x78) - *(int *)(param_1 + 0x108)) + param_3,
                     (*(int *)(param_1 + 0x7c) - *(int *)(param_1 + 0x10c)) + param_4,&local_d0,
                     *(undefined4 *)(param_1 + 0xb8));
        if (0 < (int)DAT_00656f00) {
          if (DAT_00656f00 == 0) {
            iVar2 = 0;
          }
          else {
            iVar2 = *DAT_00656f10;
          }
          local_d4 = *(uint *)(iVar2 + 0x14) >> 0x10 & 1;
          if (local_d4 != 0) {
            local_d0 = (float)((uint)local_d0 & 0xfffffff0);
            local_cc = (float)((uint)local_cc & 0xfffffff0);
          }
        }
        (**(code **)(param_1 + 0x88c))(local_d0,local_cc,local_c8);
        uVar4 = DAT_0065a28c;
        if (*(int *)(param_1 + 0x898) != 2) {
          return;
        }
        *(undefined4 *)(param_1 + 0x894) = 0;
        *(undefined4 *)(param_1 + 0x88c) = 0;
        FUN_0043a020(uVar4);
        return;
      }
      if (0 < (int)DAT_00656f00) {
        if (DAT_00656f00 < 0xb) {
          iVar2 = 0;
        }
        else {
          iVar2 = DAT_00656f10[10];
        }
        local_d4 = *(uint *)(iVar2 + 0x14) >> 0x10 & 1;
        if (local_d4 != 0) {
          iVar2 = *(int *)(param_1 + 0x108);
          if (iVar2 < 0) {
            return;
          }
          iVar5 = *(int *)(param_1 + 0x10c);
          if (iVar5 < 0) {
            return;
          }
          local_d0 = (float)((*(int *)(param_1 + 0x100) * 0x40 + iVar2) * 0x10);
          local_cc = (float)((*(int *)(param_1 + 0x104) * 0x40 + iVar5) * 0x10);
          iVar3 = FUN_00499e10(DAT_00666970,*(int *)(param_1 + 0x100),*(int *)(param_1 + 0x104));
          if (iVar3 == 0) {
            local_c8 = 0;
          }
          else {
            local_c8 = FUN_00499720(iVar2,iVar5);
          }
          FUN_0046d7a0(&local_d0,&local_d4,&local_c4);
          local_84 = (int)local_c4 + 0x14;
          if (*(int *)(param_1 + 0x50) == 0) {
            iVar2 = *(int *)(param_1 + 0x130);
            if (iVar2 < 0x40) {
              piVar6 = (int *)(param_1 + (iVar2 + 0xb) * 0x1c);
              *piVar6 = local_d4 - 0x14;
              piVar6[1] = (int)local_c4 + -0x14;
              piVar6[2] = local_d4 + 0x14;
              piVar6[3] = local_84;
              *(undefined4 *)(param_1 + 0x144 + iVar2 * 0x1c) = 0;
              *(int *)(param_1 + 0x130) = *(int *)(param_1 + 0x130) + 1;
            }
            else {
              FUN_004546a0();
              *(undefined4 *)(param_1 + 0x130) = 0;
            }
          }
LAB_0044f78e:
          *(undefined4 *)(param_1 + 0xfc) = 0xffffffff;
          *(undefined4 *)(param_1 + 0xf8) = 0xffffffff;
          *(undefined4 *)(param_1 + 0x110) = 0xffffffff;
          *(undefined4 *)(param_1 + 0x10c) = 0xffffffff;
          *(undefined4 *)(param_1 + 0x108) = 0xffffffff;
          *(undefined4 *)(param_1 + 0x104) = 0xffffffff;
          *(undefined4 *)(param_1 + 0x100) = 0xffffffff;
          return;
        }
      }
    }
    FUN_00440c60();
    return;
  }
  FUN_00499ff0(4,DAT_00667fcc);
  if (param_2 == 2) {
    FUN_00499ff0(4,DAT_00667fcc);
    if (DAT_0065c9e0 == 0) {
      if (DAT_00667fcc == (int *)0x0) {
        return;
      }
      FUN_0044ee00(param_3,param_4);
      return;
    }
    iVar2 = FUN_00452520(param_3,param_4,0);
    if (iVar2 == 0) {
      return;
    }
    sVar1 = *(short *)(iVar2 + 4);
    if (sVar1 == 9) {
      return;
    }
    if (sVar1 == 0x19) {
      return;
    }
    if (sVar1 == 10) {
      return;
    }
    if (sVar1 == 0xf) {
      return;
    }
    if (sVar1 == 0xe) {
      return;
    }
    FUN_0047eff0();
    FUN_005496a0(iVar2);
    return;
  }
  if (param_2 == 5) {
    if (DAT_00667fcc == (int *)0x0) {
      return;
    }
    if (*(int *)(param_1 + 0x11c) == 0) {
      return;
    }
    uVar4 = FUN_0046d710(s_cursor_005d030c);
    FUN_0043a020(uVar4);
    (**(code **)(*DAT_00667fd0 + 0x30))(*(undefined4 *)(param_1 + 0x128),0);
    *(undefined4 *)(param_1 + 0x128) = 0xffffffff;
    *(undefined4 *)(param_1 + 0x11c) = 0;
    return;
  }
  if (param_2 == 1) {
    if (DAT_00667fcc == (int *)0x0) {
      return;
    }
    if (param_3 < 0) {
      return;
    }
    if (param_4 < 0) {
      return;
    }
    if (*(int *)(param_1 + 0xc) <= param_3) {
      return;
    }
    if (*(int *)(param_1 + 0x10) <= param_4) {
      return;
    }
    piVar6 = (int *)DAT_00667fcc[0x38];
    if (piVar6 != (int *)0x0) {
      if ((*piVar6 == 3) && ((*(int *)(param_1 + 300) == -1 || (*(int *)(param_1 + 300) == 6)))) {
        iVar2 = FUN_00452520(param_3,param_4,0);
        if ((iVar2 != 0) && (*(short *)(iVar2 + 4) == 0xc)) {
          piVar6 = (int *)DAT_00667fcc[0x38];
          if ((piVar6 == (int *)0x0) ||
             ((*piVar6 != 3 && ((piVar6 == (int *)0x0 || (*piVar6 != 0x19)))))) {
            iVar5 = 0;
          }
          else {
            iVar5 = piVar6[0x11];
          }
          if (iVar5 != iVar2) {
            iVar5 = FUN_004c89c0(iVar2);
            if (iVar5 == 0) {
              return;
            }
            FUN_004d4790(iVar2);
            return;
          }
        }
        uVar4 = FUN_00483300(1,3);
        FUN_004d2480(uVar4);
        return;
      }
      if (((piVar6 != (int *)0x0) && (*piVar6 == 0x19)) &&
         ((*(int *)(param_1 + 300) == -1 || (*(int *)(param_1 + 300) == 6)))) {
        iVar2 = FUN_004d1050();
        if (iVar2 == 0) {
          FUN_004d0aa0();
          local_d0 = (float)DAT_00667fcc[4];
          local_cc = (float)DAT_00667fcc[5];
          local_c8 = DAT_00667fcc[6];
          FUN_0044ed50(&local_90,0x32);
          uVar4 = FUN_0046dc60(&local_d0,&local_90);
          FUN_004d0c70(uVar4);
        }
        *(undefined4 *)(param_1 + 0x120) = 1;
        return;
      }
    }
    piVar6 = (int *)FUN_00452520(param_3,param_4,0);
    if (piVar6 == (int *)0x0) {
      *(undefined4 *)(param_1 + 0xf8) = 0xffffffff;
    }
    else {
      *(int *)(param_1 + 0xf8) = piVar6[0x10];
    }
    *(undefined4 *)(param_1 + 0x120) = 1;
    if (piVar6 == (int *)0x0) {
      return;
    }
    iVar2 = (**(code **)(*piVar6 + 0x134))();
    if (iVar2 == 0) {
      return;
    }
    FUN_005496a0(piVar6);
    return;
  }
  if (param_2 != 4) {
    return;
  }
  if (((((DAT_00667fcc == (int *)0x0) || (iVar2 = (**(code **)(*DAT_00667fcc + 0x1c0))(), iVar2 < 1)
        ) || (param_3 < 0)) || ((param_4 < 0 || (*(int *)(param_1 + 0xc) <= param_3)))) ||
     (*(int *)(param_1 + 0x10) <= param_4)) goto LAB_0045046f;
  piVar6 = (int *)DAT_00667fcc[0x38];
  if (piVar6 != (int *)0x0) {
    if (((*piVar6 == 3) && ((*(int *)(param_1 + 300) == -1 || (*(int *)(param_1 + 300) == 6)))) &&
       (*(int *)(param_1 + 0x120) != 0)) goto LAB_0045046f;
    if ((((piVar6 != (int *)0x0) && (*piVar6 == 0x19)) && (iVar2 = FUN_004d1050(), iVar2 != 0)) &&
       (*(int *)(param_1 + 0x120) != 0)) {
      local_d0 = (float)DAT_00667fcc[4];
      iVar5 = DAT_00668510 - *(int *)(param_1 + 4);
      local_cc = (float)DAT_00667fcc[5];
      local_c8 = DAT_00667fcc[6];
      iVar2 = DAT_00668514 - *(int *)(param_1 + 8);
      FUN_0041c750(DAT_00667fcc + 4);
      FUN_0046dad0(*(int *)(param_1 + 0x78) + iVar5,*(int *)(param_1 + 0x7c) + iVar2,&fStack_bc,
                   uStack_88 + 0x32);
      uVar4 = FUN_0046dc60(&local_d0,&fStack_bc);
      FUN_004d0fd0(uVar4);
      goto LAB_0045046f;
    }
  }
  piVar6 = (int *)0x0;
  if ((((DAT_0065d674 == 0) || (piVar6 = (int *)FUN_004701f0(DAT_0065d67c), piVar6 == (int *)0x0))
      && ((DAT_0065b088 == 0 ||
          ((DAT_0065b090 < 0 ||
           (piVar6 = (int *)FUN_004701f0(DAT_0065b090 + 0x10b), piVar6 == (int *)0x0)))))) &&
     (DAT_00667fcc != (int *)0x0)) {
    piVar6 = (int *)DAT_00667fcc[DAT_0065b878 + 0xa8];
  }
  iVar2 = -1;
  piVar7 = (int *)FUN_00452520(param_3,param_4,piVar6);
  if (piVar7 != (int *)0x0) {
    iVar2 = piVar7[0x10];
  }
  if (*(int *)(param_1 + 0x120) != 0) {
    if (((piVar7 == (int *)0x0) || (iVar5 = (**(code **)(*piVar7 + 4))(DAT_00667fcc), iVar5 < 0x61))
       || ((short)piVar7[1] == 0xc)) {
      if ((*(int *)(param_1 + 0xf8) == iVar2) && (piVar7 != (int *)0x0)) {
        iVar5 = (**(code **)(*piVar7 + 0x134))();
        if (iVar5 == 0) {
          (**(code **)(*piVar7 + 0xbc))(DAT_00667fcc,0xffffffff);
        }
        else {
          if (iVar2 < 0) {
            piStack_74 = (int *)0x0;
          }
          else {
            FUN_0044cf80(0,0x80,0,0,0xffffffff);
            while (piStack_74 != (int *)0x0) {
              if (piStack_74[0x10] == iVar2) goto LAB_0044ffc8;
              FUN_0044d080();
            }
            piStack_74 = (int *)FUN_0051f330(iVar2);
          }
LAB_0044ffc8:
          _DAT_00668570 = piStack_74;
          if (((short)piStack_74[1] != 0xb) && ((short)piStack_74[1] != 0xc)) {
            FUN_004cfef0(piStack_74,0);
          }
        }
      }
    }
    else {
      fStack_bc = (float)DAT_00667fcc[4];
      fStack_b8 = (float)DAT_00667fcc[5];
      uStack_b4 = DAT_00667fcc[6];
      FUN_0044e930(param_3,param_4,&fStack_bc,&fStack_ac);
      local_d4 = (int)fStack_ac - (int)fStack_bc;
      Var18 = fpatan((float10)((int)fStack_a8 - (int)fStack_b8),(float10)(int)local_d4);
      fVar17 = (float10)fcos(Var18);
      local_c4 = (float)(fVar17 * (float10)_DAT_005a4968);
      fVar17 = (float10)fsin(Var18);
      local_c0 = (float)(fVar17 * (float10)_DAT_005a4968);
      local_d0 = (float)(int)fStack_bc;
      local_cc = (float)(int)fStack_b8;
      iVar2 = __ftol();
      fStack_ac = (float)((int)fStack_ac + iVar2);
      iVar2 = __ftol();
      fStack_a8 = (float)((int)fStack_a8 + iVar2);
      iVar2 = 0;
      iVar5 = (int)(iVar5 + -0x20 + (iVar5 + -0x20 >> 0x1f & 0x1fU)) >> 5;
      if (0 < iVar5) {
        local_d4 = __ftol();
        do {
          local_90 = __ftol();
          uStack_8c = __ftol();
          uStack_88 = local_d4;
          FUN_004530a0(&local_90,*(undefined2 *)((int)DAT_00667fcc + 0xe),0x20,&local_b0,&local_94,
                       &local_a0);
          fVar16 = local_b0;
          if ((int)local_b0 < 0) {
            fVar16 = (float)-(int)local_b0;
          }
          if ((0x20 < (int)fVar16) || (local_a0 != 0)) break;
          local_d0 = local_d0 + local_c4;
          iVar2 = iVar2 + 1;
          local_cc = local_cc + local_c0;
        } while (iVar2 < iVar5);
      }
      if (iVar2 == iVar5) {
        FUN_004cedb0(fStack_ac,fStack_a8,piVar7);
      }
      else {
        uVar4 = FUN_0049d800(s_ITEMTOFAR_005d0314);
        FUN_0054d190(&DAT_0065c5d0,0x10,uVar4);
      }
    }
    goto LAB_0045046f;
  }
  if (piVar6 == (int *)0x0) goto LAB_0045046f;
  if (-1 < iVar2) {
    FUN_0044cf80(0,0x80,0,0,0xffffffff);
    while (piStack_74 != (int *)0x0) {
      if (piStack_74[0x10] == iVar2) goto LAB_00450047;
      FUN_0044d080();
    }
    piStack_74 = (int *)FUN_0051f330(iVar2);
LAB_00450047:
    if (piStack_74 != (int *)0x0) {
      iVar2 = piStack_74[0x1f];
      iVar5 = (**(code **)(*piStack_74 + 0xbc))(DAT_00667fcc,piVar6[0x10]);
      if (iVar5 != 0) {
        if ((short)iVar2 < 0x100) {
          _DAT_0065d548 = 1;
          (**(code **)(DAT_0065d4f8 + 0x90))();
        }
        else if (0x10a < (short)iVar2) {
          _DAT_0065b078 = 1;
        }
        goto LAB_0045046f;
      }
    }
  }
  iVar2 = (**(code **)(*piVar6 + 0x5c))();
  if ((iVar2 == 0) || ((short)piVar6[1] == 0x15)) goto LAB_0045046f;
  if ((0xff < (short)piVar6[0x1f]) && ((short)piVar6[0x1f] < 0x10b)) {
    FUN_005199b0(0,DAT_0065b878);
  }
  local_d0 = (float)DAT_00667fcc[4];
  local_cc = (float)DAT_00667fcc[5];
  local_c8 = DAT_00667fcc[6];
  FUN_0044e930(param_3,param_4,&local_d0,&fStack_ac);
  if ((short)piVar6[1] != 8) {
    fStack_ac = (float)((int)fStack_ac + 0x40);
    fStack_a8 = (float)((int)fStack_a8 + 0x40);
  }
  local_c0 = (float)((int)fStack_ac - (int)local_d0);
  local_c4 = (float)((int)fStack_a8 - (int)local_cc);
  local_d4 = (int)local_c0 * (int)local_c0 + (int)local_c4 * (int)local_c4;
  iVar2 = __ftol();
  if (0x60 < iVar2) {
    iVar2 = 0;
    Var18 = fpatan((float10)(int)local_c4,(float10)(int)local_c0);
    fVar17 = (float10)fcos(Var18);
    local_c0 = (float)(fVar17 * (float10)_DAT_005a4968);
    fVar17 = (float10)fsin(Var18);
    local_b0 = (float)(fVar17 * (float10)_DAT_005a4968);
    fStack_bc = (float)(int)local_d0;
    fStack_b8 = (float)(int)local_cc;
    uVar8 = __ftol();
    do {
      local_90 = __ftol();
      uStack_8c = __ftol();
      uStack_88 = uVar8;
      FUN_004530a0(&local_90,*(undefined2 *)((int)DAT_00667fcc + 0xe),0x20,&local_d4,&local_a0,
                   &local_c4);
      uVar9 = local_d4;
      if ((int)local_d4 < 0) {
        uVar9 = -local_d4;
      }
      if ((0x20 < (int)uVar9) || (local_c4 != 0.0)) break;
      fStack_bc = fStack_bc + local_c0;
      iVar2 = iVar2 + 1;
      fStack_b8 = fStack_b8 + local_b0;
    } while (iVar2 < 3);
    fStack_ac = (float)__ftol();
    fStack_a8 = (float)__ftol();
    uStack_a4 = uVar8;
  }
  (**(code **)(*piVar6 + 0x60))();
  (**(code **)(*piVar6 + 8))(&fStack_ac,*(undefined2 *)((int)DAT_00667fcc + 0xe),0);
  (**(code **)(*piVar6 + 0x8c))();
  FUN_0046e7d0(&uStack_8c,0x80);
  iVar2 = (**(code **)(*piVar6 + 0x198))();
  if (iVar2 < 2) {
    iVar2 = FUN_0049d6d0(s_FULLSINGDROP_005d0384);
    if (iVar2 < 0) {
      puVar12 = (undefined4 *)FUN_0049d800(s_BASEDROPEPED_005d03a4);
      puVar10 = &uStack_8c;
      pcVar11 = s__s__s_005d03b4;
      goto LAB_004503bc;
    }
    puVar12 = &uStack_8c;
    uVar4 = FUN_0049d800(s_FULLSINGDROP_005d0394);
    FUN_0054d170(&DAT_0065c5d0,uVar4,puVar12);
  }
  else {
    iVar2 = FUN_0049d6d0(s_FULLMULTDROP_005d0320);
    if (iVar2 < 0) {
      iVar2 = FUN_0049d6d0(s_FULLMULTDROPREV_005d0340);
      if (iVar2 < 0) {
        uVar4 = FUN_0049d800(s_BASEDROPEPED_005d0360);
        uVar13 = FUN_0049d800(s_PLURAL_005d0370);
        uVar4 = (**(code **)(*piVar6 + 0x198))(&uStack_8c,uVar13,uVar4);
        FUN_0054d170(&DAT_0065c5d0,s__d__s_s__s_005d0378,uVar4);
        goto LAB_004503c9;
      }
      puVar12 = (undefined4 *)(**(code **)(*piVar6 + 0x198))();
      puVar10 = &uStack_8c;
      pcVar11 = (char *)FUN_0049d800(s_FULLMULTDROPREV_005d0350);
    }
    else {
      puVar12 = &uStack_8c;
      puVar10 = (undefined4 *)(**(code **)(*piVar6 + 0x198))(puVar12);
      pcVar11 = (char *)FUN_0049d800(s_FULLMULTDROP_005d0330);
    }
LAB_004503bc:
    FUN_0054d170(&DAT_0065c5d0,pcVar11,puVar10,puVar12);
  }
LAB_004503c9:
  _DAT_00668574 = piVar6;
  iVar2 = (**(code **)(*piVar6 + 0x30))();
  if (iVar2 != 0) {
    (**(code **)(*piVar6 + 0x28))();
  }
  iVar2 = (**(code **)(*piVar6 + 0x24))();
  if (iVar2 != 0) {
    fStack_bc = fStack_ac;
    fStack_b8 = fStack_a8;
    uStack_b4 = uStack_a4;
    iVar2 = FUN_0059a530(*(undefined4 *)(piVar6[0x12] + 4),s_Armor_005d03bc);
    if ((iVar2 != 0) && ((short)piVar6[1] != 8)) {
      uStack_b4 = uStack_b4 + 0x40;
      FUN_00445ef0(&fStack_bc,&fStack_ac,0,0xffffffff,1);
    }
  }
  if ((DAT_00676828 != 0) && (DAT_0067682c == 0)) {
    FUN_00585bc0(DAT_00667fcc,piVar6,&fStack_ac);
  }
LAB_0045046f:
  *(undefined4 *)(param_1 + 0x120) = 0;
  if ((((-1 < param_3) && (-1 < param_4)) && (param_3 < *(int *)(param_1 + 0xc))) &&
     (param_4 < *(int *)(param_1 + 0x10))) {
    FUN_0043a100(0,0,0);
    FUN_0043a170(&DAT_00658d98);
    FUN_0043a140(0);
  }
  return;
}


