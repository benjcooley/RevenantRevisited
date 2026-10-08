// FUN_00532340 @ 00532340 size=3068

void __thiscall FUN_00532340(int param_1,uint param_2,int param_3,int param_4)

{
  short *psVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  uint local_c;
  int local_4;
  
  uVar8 = *(uint *)(param_1 + 0x17c);
  if ((uVar8 & 8) == 0) {
    if ((uVar8 & 4) == 0) {
      uVar4 = param_2;
      if ((uVar8 & 0x10) != 0) {
        uVar4 = 0;
      }
    }
    else {
      uVar4 = 2;
    }
  }
  else {
    uVar4 = 1;
  }
  if (((uVar8 & 8) == 0) && ((uVar8 & 4) == 0)) {
    uVar8 = -(uint)(1 < DAT_0065a258) & DAT_0065a14c;
    iVar3 = FUN_00474210(param_2);
    if ((iVar3 != -1) && (local_c = 0, 0 < *(short *)(param_1 + 0x194))) {
      local_4 = 0;
      do {
        uVar4 = FUN_00475210(*(undefined4 *)(local_4 + 4 + *(int *)(param_1 + 0x198)),0);
        if (uVar4 != 0xffffffff) {
          uVar5 = FUN_00474210(param_2);
          if ((uVar4 < *(uint *)(uVar8 + 0x24)) &&
             (iVar3 = FUN_0044ce10(uVar4), uVar5 < (uint)(int)*(short *)(iVar3 + 0xc))) {
            iVar3 = FUN_0044ce10(uVar4);
            iVar3 = *(int *)(*(int *)(iVar3 + 0x10) + uVar5 * 4);
          }
          else {
            iVar3 = 0;
          }
          if ((param_3 <= iVar3) && (iVar3 <= param_4)) {
            FUN_0052f310();
            if (local_c < (uint)(int)*(short *)(param_1 + 0x194)) {
              iVar3 = *(short *)(param_1 + 0x196) + -1;
              if ((int)local_c < iVar3) {
                iVar3 = iVar3 - local_c;
                puVar10 = (undefined4 *)(*(int *)(param_1 + 0x198) + local_4);
                do {
                  iVar3 = iVar3 + -1;
                  puVar9 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar9;
                    puVar9 = puVar9 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar3 != 0);
              }
              sVar2 = *(short *)(param_1 + 0x194) + -1;
              *(short *)(param_1 + 0x194) = sVar2;
              if (sVar2 < 1) {
                FUN_00533250();
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)*(short *)(param_1 + 0x194));
    }
    uVar8 = -(uint)(2 < DAT_0065a258) & DAT_0065a150;
    iVar3 = FUN_00474210(param_2);
    if (iVar3 != -1) {
      psVar1 = (short *)(param_1 + 0x194);
      local_c = 0;
      if (0 < *psVar1) {
        local_4 = 0;
        do {
          uVar4 = FUN_00475210(*(undefined4 *)(local_4 + 4 + *(int *)(param_1 + 0x198)),0);
          if (uVar4 != 0xffffffff) {
            uVar5 = FUN_00474210(param_2);
            if (uVar4 < *(uint *)(uVar8 + 0x24)) {
              iVar3 = *(int *)(*(int *)(uVar8 + 0x34) + uVar4 * 4);
              if (iVar3 == 0) {
                iVar3 = *(int *)(uVar8 + 0x38);
              }
              if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar5) goto LAB_0053259e;
              iVar3 = FUN_0044ce10(uVar4);
              iVar3 = *(int *)(*(int *)(iVar3 + 0x10) + uVar5 * 4);
            }
            else {
LAB_0053259e:
              iVar3 = 0;
            }
            if ((param_3 <= iVar3) && (iVar3 <= param_4)) {
              FUN_0052f310();
              if (local_c < (uint)(int)*psVar1) {
                iVar3 = *(short *)(param_1 + 0x196) + -1;
                if ((int)local_c < iVar3) {
                  iVar3 = iVar3 - local_c;
                  puVar10 = (undefined4 *)(*(int *)(param_1 + 0x198) + local_4);
                  do {
                    iVar3 = iVar3 + -1;
                    puVar9 = puVar10 + 0x12;
                    puVar11 = puVar10;
                    for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                      *puVar11 = *puVar9;
                      puVar9 = puVar9 + 1;
                      puVar11 = puVar11 + 1;
                    }
                    puVar10 = puVar10 + 0x12;
                  } while (iVar3 != 0);
                }
                sVar2 = *psVar1;
                *psVar1 = sVar2 + -1;
                if ((short)(sVar2 + -1) < 1) {
                  FUN_00533250();
                }
              }
              local_c = local_c - 1;
              local_4 = local_4 + -0x48;
            }
          }
          local_c = local_c + 1;
          local_4 = local_4 + 0x48;
        } while ((int)local_c < (int)*psVar1);
      }
    }
    uVar8 = -(uint)(4 < DAT_0065a258) & DAT_0065a158;
    iVar3 = FUN_00474210(param_2);
    if ((iVar3 != -1) && (local_c = 0, 0 < *(short *)(param_1 + 0x194))) {
      local_4 = 0;
      do {
        uVar4 = FUN_00475210(*(undefined4 *)(local_4 + 4 + *(int *)(param_1 + 0x198)),0);
        if (uVar4 != 0xffffffff) {
          uVar5 = FUN_00474210(param_2);
          if (uVar4 < *(uint *)(uVar8 + 0x24)) {
            iVar3 = *(int *)(*(int *)(uVar8 + 0x34) + uVar4 * 4);
            if (iVar3 == 0) {
              iVar3 = *(int *)(uVar8 + 0x38);
            }
            if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar5) goto LAB_005326e9;
            iVar3 = FUN_0044ce10(uVar4);
            iVar3 = *(int *)(*(int *)(iVar3 + 0x10) + uVar5 * 4);
          }
          else {
LAB_005326e9:
            iVar3 = 0;
          }
          if ((param_3 <= iVar3) && (iVar3 <= param_4)) {
            FUN_0052f310();
            sVar2 = *(short *)(param_1 + 0x194);
            if (local_c < (uint)(int)sVar2) {
              iVar3 = *(short *)(param_1 + 0x196) + -1;
              if ((int)local_c < iVar3) {
                iVar3 = iVar3 - local_c;
                puVar10 = (undefined4 *)(*(int *)(param_1 + 0x198) + local_4);
                do {
                  iVar3 = iVar3 + -1;
                  puVar9 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar9;
                    puVar9 = puVar9 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar3 != 0);
              }
              sVar2 = sVar2 + -1;
              *(short *)(param_1 + 0x194) = sVar2;
              if (sVar2 < 1) {
                if (*(int *)(param_1 + 0x198) != 0) {
                  FUN_004830f0(*(int *)(param_1 + 0x198));
                }
                *(undefined4 *)(param_1 + 0x198) = 0;
                *(undefined2 *)(param_1 + 0x194) = 0;
                *(undefined2 *)(param_1 + 0x196) = 0;
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)*(short *)(param_1 + 0x194));
    }
    uVar8 = -(uint)(0x12 < DAT_0065a258) & DAT_0065a190;
    iVar3 = FUN_00474210(param_2);
    if ((iVar3 != -1) && (local_c = 0, 0 < *(short *)(param_1 + 0x194))) {
      local_4 = 0;
      do {
        uVar4 = FUN_00475210(*(undefined4 *)(local_4 + 4 + *(int *)(param_1 + 0x198)),0);
        if (uVar4 != 0xffffffff) {
          uVar5 = FUN_00474210(param_2);
          if (uVar4 < *(uint *)(uVar8 + 0x24)) {
            iVar3 = *(int *)(*(int *)(uVar8 + 0x34) + uVar4 * 4);
            if (iVar3 == 0) {
              iVar3 = *(int *)(uVar8 + 0x38);
            }
            if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar5) goto LAB_00532872;
            iVar3 = FUN_0044ce10(uVar4);
            iVar3 = *(int *)(*(int *)(iVar3 + 0x10) + uVar5 * 4);
          }
          else {
LAB_00532872:
            iVar3 = 0;
          }
          if ((param_3 <= iVar3) && (iVar3 <= param_4)) {
            FUN_0052f310();
            sVar2 = *(short *)(param_1 + 0x194);
            if (local_c < (uint)(int)sVar2) {
              iVar3 = *(short *)(param_1 + 0x196) + -1;
              if ((int)local_c < iVar3) {
                iVar3 = iVar3 - local_c;
                puVar10 = (undefined4 *)(*(int *)(param_1 + 0x198) + local_4);
                do {
                  iVar3 = iVar3 + -1;
                  puVar9 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar9;
                    puVar9 = puVar9 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar3 != 0);
              }
              sVar2 = sVar2 + -1;
              *(short *)(param_1 + 0x194) = sVar2;
              if (sVar2 < 1) {
                if (*(int *)(param_1 + 0x198) != 0) {
                  FUN_004830f0(*(int *)(param_1 + 0x198));
                }
                *(undefined4 *)(param_1 + 0x198) = 0;
                *(undefined2 *)(param_1 + 0x194) = 0;
                *(undefined2 *)(param_1 + 0x196) = 0;
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)*(short *)(param_1 + 0x194));
    }
    uVar8 = -(uint)(0x15 < DAT_0065a258) & DAT_0065a19c;
    iVar3 = FUN_00474210(param_2);
    if ((iVar3 != -1) && (local_c = 0, 0 < *(short *)(param_1 + 0x194))) {
      local_4 = 0;
      do {
        uVar4 = FUN_00475210(*(undefined4 *)(*(int *)(param_1 + 0x198) + 4 + local_4),0);
        if (uVar4 != 0xffffffff) {
          uVar5 = FUN_00474210(param_2);
          if (uVar4 < *(uint *)(uVar8 + 0x24)) {
            iVar3 = *(int *)(*(int *)(uVar8 + 0x34) + uVar4 * 4);
            if (iVar3 == 0) {
              iVar3 = *(int *)(uVar8 + 0x38);
            }
            if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar5) goto LAB_005329fb;
            iVar3 = FUN_0044ce10(uVar4);
            iVar3 = *(int *)(*(int *)(iVar3 + 0x10) + uVar5 * 4);
          }
          else {
LAB_005329fb:
            iVar3 = 0;
          }
          if ((param_3 <= iVar3) && (iVar3 <= param_4)) {
            FUN_0052f310();
            sVar2 = *(short *)(param_1 + 0x194);
            if (local_c < (uint)(int)sVar2) {
              iVar3 = *(short *)(param_1 + 0x196) + -1;
              if ((int)local_c < iVar3) {
                iVar3 = iVar3 - local_c;
                puVar10 = (undefined4 *)(*(int *)(param_1 + 0x198) + local_4);
                do {
                  iVar3 = iVar3 + -1;
                  puVar9 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar9;
                    puVar9 = puVar9 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar3 != 0);
              }
              sVar2 = sVar2 + -1;
              *(short *)(param_1 + 0x194) = sVar2;
              if (sVar2 < 1) {
                if (*(int *)(param_1 + 0x198) != 0) {
                  FUN_004830f0(*(int *)(param_1 + 0x198));
                }
                *(undefined4 *)(param_1 + 0x198) = 0;
                *(undefined2 *)(param_1 + 0x194) = 0;
                *(undefined2 *)(param_1 + 0x196) = 0;
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)*(short *)(param_1 + 0x194));
    }
    uVar8 = -(uint)(5 < DAT_0065a258) & DAT_0065a15c;
    iVar3 = FUN_00474210(param_2);
    if ((iVar3 != -1) && (local_c = 0, 0 < *(short *)(param_1 + 0x194))) {
      local_4 = 0;
      do {
        uVar4 = FUN_00475210(*(undefined4 *)(*(int *)(param_1 + 0x198) + 4 + local_4),0);
        if (uVar4 != 0xffffffff) {
          uVar5 = FUN_00474210(param_2);
          if (uVar4 < *(uint *)(uVar8 + 0x24)) {
            iVar3 = *(int *)(*(int *)(uVar8 + 0x34) + uVar4 * 4);
            if (iVar3 == 0) {
              iVar3 = *(int *)(uVar8 + 0x38);
            }
            if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar5) goto LAB_00532b84;
            iVar3 = FUN_0044ce10(uVar4);
            iVar3 = *(int *)(*(int *)(iVar3 + 0x10) + uVar5 * 4);
          }
          else {
LAB_00532b84:
            iVar3 = 0;
          }
          if ((param_3 <= iVar3) && (iVar3 <= param_4)) {
            FUN_0052f310();
            sVar2 = *(short *)(param_1 + 0x194);
            if (local_c < (uint)(int)sVar2) {
              iVar3 = *(short *)(param_1 + 0x196) + -1;
              if ((int)local_c < iVar3) {
                iVar3 = iVar3 - local_c;
                puVar10 = (undefined4 *)(*(int *)(param_1 + 0x198) + local_4);
                do {
                  iVar3 = iVar3 + -1;
                  puVar9 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar9;
                    puVar9 = puVar9 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar3 != 0);
              }
              sVar2 = sVar2 + -1;
              *(short *)(param_1 + 0x194) = sVar2;
              if (sVar2 < 1) {
                if (*(int *)(param_1 + 0x198) != 0) {
                  FUN_004830f0(*(int *)(param_1 + 0x198));
                }
                *(undefined4 *)(param_1 + 0x198) = 0;
                *(undefined2 *)(param_1 + 0x194) = 0;
                *(undefined2 *)(param_1 + 0x196) = 0;
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)*(short *)(param_1 + 0x194));
    }
    uVar8 = -(uint)(0x11 < DAT_0065a258) & DAT_0065a18c;
    iVar3 = FUN_00474210(param_2);
    if ((iVar3 != -1) && (local_c = 0, 0 < *(short *)(param_1 + 0x194))) {
      local_4 = 0;
      do {
        uVar4 = FUN_00475210(*(undefined4 *)(*(int *)(param_1 + 0x198) + 4 + local_4),0);
        if (uVar4 != 0xffffffff) {
          uVar5 = FUN_00474210(param_2);
          if (uVar4 < *(uint *)(uVar8 + 0x24)) {
            iVar3 = *(int *)(*(int *)(uVar8 + 0x34) + uVar4 * 4);
            if (iVar3 == 0) {
              iVar3 = *(int *)(uVar8 + 0x38);
            }
            if ((uint)(int)*(short *)(iVar3 + 0xc) <= uVar5) goto LAB_00532d02;
            iVar3 = *(int *)(*(int *)(uVar8 + 0x34) + uVar4 * 4);
            if (iVar3 == 0) {
              iVar3 = *(int *)(uVar8 + 0x38);
            }
            iVar3 = *(int *)(*(int *)(iVar3 + 0x10) + uVar5 * 4);
          }
          else {
LAB_00532d02:
            iVar3 = 0;
          }
          if ((param_3 <= iVar3) && (iVar3 <= param_4)) {
            FUN_0052f310();
            sVar2 = *(short *)(param_1 + 0x194);
            if (local_c < (uint)(int)sVar2) {
              iVar3 = *(short *)(param_1 + 0x196) + -1;
              if ((int)local_c < iVar3) {
                iVar3 = iVar3 - local_c;
                puVar10 = (undefined4 *)(*(int *)(param_1 + 0x198) + local_4);
                do {
                  iVar3 = iVar3 + -1;
                  puVar9 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar6 = 0x12; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *puVar11 = *puVar9;
                    puVar9 = puVar9 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar3 != 0);
              }
              sVar2 = sVar2 + -1;
              *(short *)(param_1 + 0x194) = sVar2;
              if (sVar2 < 1) {
                if (*(int *)(param_1 + 0x198) != 0) {
                  FUN_004830f0(*(int *)(param_1 + 0x198));
                }
                *(undefined4 *)(param_1 + 0x198) = 0;
                *(undefined2 *)(param_1 + 0x194) = 0;
                *(undefined2 *)(param_1 + 0x196) = 0;
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
        if ((int)*(short *)(param_1 + 0x194) <= (int)local_c) {
          return;
        }
      } while( true );
    }
  }
  else {
    if (uVar4 < DAT_0065a258) {
      iVar3 = (&DAT_0065a148)[uVar4];
    }
    else {
      iVar3 = 0;
    }
    iVar6 = FUN_00474210(param_2);
    if ((iVar6 != -1) && (local_c = 0, 0 < *(short *)(param_1 + 0x194))) {
      local_4 = 0;
      do {
        uVar8 = FUN_00475210(*(undefined4 *)(*(int *)(param_1 + 0x198) + 4 + local_4),0);
        if (uVar8 != 0xffffffff) {
          uVar4 = FUN_00474210(param_2);
          if ((uVar8 < *(uint *)(iVar3 + 0x24)) &&
             (iVar6 = FUN_0044ce10(uVar8), uVar4 < (uint)(int)*(short *)(iVar6 + 0xc))) {
            iVar6 = FUN_0044ce10(uVar8);
            iVar6 = *(int *)(*(int *)(iVar6 + 0x10) + uVar4 * 4);
          }
          else {
            iVar6 = 0;
          }
          if ((param_3 <= iVar6) && (iVar6 <= param_4)) {
            FUN_0052f310();
            if (local_c < (uint)(int)*(short *)(param_1 + 0x194)) {
              iVar6 = *(short *)(param_1 + 0x196) + -1;
              if ((int)local_c < iVar6) {
                iVar6 = iVar6 - local_c;
                puVar10 = (undefined4 *)(*(int *)(param_1 + 0x198) + local_4);
                do {
                  iVar6 = iVar6 + -1;
                  puVar9 = puVar10 + 0x12;
                  puVar11 = puVar10;
                  for (iVar7 = 0x12; iVar7 != 0; iVar7 = iVar7 + -1) {
                    *puVar11 = *puVar9;
                    puVar9 = puVar9 + 1;
                    puVar11 = puVar11 + 1;
                  }
                  puVar10 = puVar10 + 0x12;
                } while (iVar6 != 0);
              }
              sVar2 = *(short *)(param_1 + 0x194) + -1;
              *(short *)(param_1 + 0x194) = sVar2;
              if (sVar2 < 1) {
                FUN_00533250();
              }
            }
            local_c = local_c - 1;
            local_4 = local_4 + -0x48;
          }
        }
        local_c = local_c + 1;
        local_4 = local_4 + 0x48;
      } while ((int)local_c < (int)*(short *)(param_1 + 0x194));
    }
  }
  return;
}


