// FUN_004c4950_ResolveAttack @ 004c4950 size=3776

void __thiscall
FUN_004c4950_ResolveAttack(int *param_1,int param_2,int param_3,undefined4 param_4,uint param_5,int *param_6)

{
  char cVar1;
  int *piVar2;
  undefined4 uVar3;
  byte bVar4;
  int iVar5;
  int iVar6;
  byte *pbVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  byte *pbVar12;
  char *pcVar13;
  char *pcVar14;
  byte *pbVar15;
  bool bVar16;
  byte **ppbVar17;
  uint uStack_98;
  byte *pbStack_94;
  uint uStack_90;
  byte *pbStack_8c;
  int iStack_88;
  byte abStack_84 [52];
  undefined4 uStack_50;
  char acStack_40 [52];
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e355;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  uStack_50 = (**(code **)(*param_1 + 0x1c0))();
  if ((param_6 != (int *)0x0) && ((param_6[2] & 0x2800000U) != 0)) {
    ExceptionList = pvStack_c;
    return;
  }
  if ((DAT_00668104 != 0) && ((short)param_1[1] == 0xb)) {
    ExceptionList = pvStack_c;
    return;
  }
  if ((param_1[2] & 0x10000000U) != 0) {
    ExceptionList = pvStack_c;
    return;
  }
  if ((DAT_0066829c == 0) && (DAT_00668108 != 0)) {
    if (param_6 != (int *)0x0) {
      if ((short)param_6[1] == 0xb) {
        param_2 = 100000;
      }
      goto LAB_004c49ec;
    }
  }
  else {
LAB_004c49ec:
    if ((param_6 != (int *)0x0) && (iVar5 = FUN_004c89c0_IsEnemy(), iVar5 == 0)) {
      ExceptionList = pvStack_c;
      return;
    }
  }
  if (((short)param_1[1] == 0xb) && ((*(byte *)(param_1 + 0xdb) & 2) != 0)) {
    ExceptionList = pvStack_c;
    return;
  }
  if (DAT_0066829c == 0) {
LAB_004c4a54:
    if ((((-1 < param_3) &&
         (param_2 = (**(code **)(*param_1 + 0x224))(param_2,param_3), (short)param_1[1] == 0xb)) &&
        (iVar5 = (**(code **)(*param_1 + 0x2cc))(), 0 < iVar5)) && ((iVar5 < 3 || (iVar5 == 4)))) {
      iVar5 = (**(code **)(*param_1 + 0x3d4))();
      param_2 = ((iVar5 + 100) * param_2) / 100;
    }
    if (((*(byte *)(param_1 + 0x44) & 8) != 0) &&
       (iVar5 = (**(code **)(*param_1 + 0x1c0))(), iVar5 + -1 <= param_2)) {
      param_2 = (**(code **)(*param_1 + 0x1c0))();
      param_2 = param_2 + -1;
    }
  }
  else {
    if (DAT_00676828 != 0) {
      if (DAT_0067682c != 0) goto LAB_004c4a54;
      if (DAT_00676e5c == '\0') {
        ExceptionList = pvStack_c;
        return;
      }
    }
    if (DAT_0067682c != 0) goto LAB_004c4a54;
  }
  iVar5 = (**(code **)(*param_1 + 0x24))();
  if (iVar5 != 0) {
    FUN_0046eb40(&pbStack_8c);
    iStack_88 = iStack_88 + -0x87;
    iVar5 = (**(code **)(*param_1 + 0x1c0))();
    if (-1 < iVar5 - param_2) {
      (**(code **)(*param_1 + 0x1c0))();
    }
    (**(code **)(*param_1 + 0x1d8))();
    iVar6 = (**(code **)(*param_1 + 0x1c0))();
    iVar5 = param_2;
    if (iVar6 < param_2) {
      iVar5 = (**(code **)(*param_1 + 0x1c0))();
    }
    iVar5 = -iVar5;
    ppbVar17 = &pbStack_8c;
    (**(code **)(*param_1 + 0x24))(iVar5,ppbVar17);
    FUN_004da3b0(iVar5,ppbVar17);
  }
  if ((param_1[0x44] & 0x80000U) == 0) {
    iVar5 = *(int *)(param_1[0x36] + 0x48);
    if (((iVar5 == 0) || ((*(uint *)(iVar5 + 0x24) & 0x2000000) == 0)) &&
       ((iVar5 = *(int *)(param_1[0x36] + 0x4c), iVar5 == 0 ||
        ((*(byte *)(iVar5 + 0x24) & 0x80) == 0)))) goto LAB_004c4c20;
  }
  else {
LAB_004c4c20:
    if (param_2 != 0) {
      FUN_0046e970_Animate(param_2);
    }
  }
  piVar2 = (int *)param_1[0x38];
  pbStack_94 = (byte *)0x0;
  uStack_98 = 0;
  if ((((piVar2 != (int *)0x0) && (*piVar2 == 3)) || ((piVar2 != (int *)0x0 && (*piVar2 == 0x19))))
     && (piVar2[0x11] != 0)) {
    iVar5 = FUN_0046ea90_AngleTo();
    if (iVar5 < 0x20) {
      if (0x5f < iVar5) goto LAB_004c4c96;
      if (iVar5 < 0xa0) goto LAB_004c4cbd;
LAB_004c4cae:
      if (iVar5 < 0xe0) {
        uStack_98 = 0x800;
      }
    }
    else if (iVar5 < 0x60) {
      uStack_98 = 0x400;
    }
    else {
LAB_004c4c96:
      if (0x9f < iVar5) goto LAB_004c4cae;
      uStack_98 = 0x200;
    }
  }
LAB_004c4cbd:
  iVar5 = (**(code **)(*param_1 + 0x1c0))();
  if ((iVar5 < 1) && ((param_1[0x44] & 0x40000U) == 0)) {
    param_1[0x44] = param_1[0x44] | 0x40000;
    (**(code **)(*param_1 + 0x248))();
    if (param_6 != (int *)0x0) {
      (**(code **)(*param_6 + 0x244))();
    }
  }
  if ((param_1[0x44] & 0x80000U) == 0) {
    iVar5 = *(int *)(param_1[0x36] + 0x48);
    if (((iVar5 != 0) && ((*(uint *)(iVar5 + 0x24) & 0x2000000) != 0)) ||
       ((iVar5 = *(int *)(param_1[0x36] + 0x4c), iVar5 != 0 &&
        ((*(byte *)(iVar5 + 0x24) & 0x80) != 0)))) goto LAB_004c5769;
  }
  iVar5 = (**(code **)(*param_1 + 0x1c0))();
  if (iVar5 < 1) {
    uStack_90 = param_5;
    if ((param_5 == 0) ||
       (((pbVar7 = *(byte **)(param_5 + 0x4c), pbVar7 != (byte *)0x0 && ((pbVar7[0x24] & 4) == 0))
        && ((*(uint *)(*(int *)(param_5 + 0x48) + 0x24) & 0x2000000) == 0)))) {
      abStack_84[0] = 0;
      pbStack_8c = (byte *)0x0;
      pbStack_94 = (byte *)(param_1[0x3f] + 0x218);
      if (0 < *(int *)(param_1[0x3f] + 0x214)) {
        do {
          uVar10 = 0xffffffff;
          pbVar7 = pbStack_94;
          do {
            pbVar12 = pbVar7;
            if (uVar10 == 0) break;
            uVar10 = uVar10 - 1;
            pbVar12 = pbVar7 + 1;
            bVar4 = *pbVar7;
            pbVar7 = pbVar12;
          } while (bVar4 != 0);
          uVar10 = ~uVar10;
          iVar5 = *param_1;
          pbVar7 = pbVar12 + -uVar10;
          pbVar12 = abStack_84;
          for (uVar9 = uVar10 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
            *(undefined4 *)pbVar12 = *(undefined4 *)pbVar7;
            pbVar7 = pbVar7 + 4;
            pbVar12 = pbVar12 + 4;
          }
          for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
            *pbVar12 = *pbVar7;
            pbVar7 = pbVar7 + 1;
            pbVar12 = pbVar12 + 1;
          }
          iVar5 = (**(code **)(iVar5 + 0x1f0))(abStack_84);
          if (iVar5 == 0) {
            FUN_004ce1b0_BuildActionName(abStack_84);
          }
          uVar10 = *(uint *)(pbStack_94 + 0x24);
          if ((((uVar10 & uStack_98) == uStack_98) && ((uVar10 & 4) != 0)) &&
             ((((uVar10 & 8) == 0 || (*(int *)param_1[0x36] == 0xd)) &&
              (((((uVar10 & 0x10) == 0 || (*(int *)param_1[0x36] == 0xe)) &&
                (iVar5 = (**(code **)(*param_1 + 0x1f0))(abStack_84), iVar5 != 0)) &&
               ((pbStack_94[0x28] == 0 ||
                (iVar5 = (**(code **)(*param_1 + 0x1f0))(pbStack_94 + 0x28), iVar5 != 0)))))))) {
            iVar6 = (**(code **)(*param_1 + 0x1c0))();
            iVar5 = param_2;
            if (param_2 < iVar6) {
              iVar5 = (**(code **)(*param_1 + 0x1c0))();
            }
            if (iVar5 < 1) {
              iVar5 = 1;
            }
            else {
              iVar6 = (**(code **)(*param_1 + 0x1c0))();
              iVar5 = param_2;
              if (param_2 < iVar6) {
                iVar5 = (**(code **)(*param_1 + 0x1c0))();
              }
            }
            iVar5 = (param_2 * 100) / iVar5;
            if ((*(int *)(pbStack_94 + 0x4c) <= iVar5) && (iVar5 <= *(int *)(pbStack_94 + 0x50)))
            break;
          }
          pbStack_8c = pbStack_8c + 1;
          pbStack_94 = pbStack_94 + 0x5c;
        } while ((int)pbStack_8c < *(int *)(param_1[0x3f] + 0x214));
      }
      pbVar7 = pbStack_94;
      if ((int)pbStack_8c < *(int *)(param_1[0x3f] + 0x214)) {
        if (abStack_84[0] == 0) goto LAB_004c5069;
      }
      else {
        abStack_84[0] = 0;
LAB_004c5069:
        FUN_004ce1b0_BuildActionName(abStack_84);
        iVar5 = (**(code **)(*param_1 + 0x1f0))(abStack_84);
        if (iVar5 == 0) {
          FUN_004ce1b0_BuildActionName(acStack_40);
          uVar10 = 0xffffffff;
          pcVar13 = (char *)(param_1[0x38] + 4);
          do {
            pcVar14 = pcVar13;
            if (uVar10 == 0) break;
            uVar10 = uVar10 - 1;
            pcVar14 = pcVar13 + 1;
            cVar1 = *pcVar13;
            pcVar13 = pcVar14;
          } while (cVar1 != '\0');
          uVar10 = ~uVar10;
          pbVar12 = (byte *)(pcVar14 + -uVar10);
          pbVar15 = abStack_84;
          for (uVar9 = uVar10 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
            *(undefined4 *)pbVar15 = *(undefined4 *)pbVar12;
            pbVar12 = pbVar12 + 4;
            pbVar15 = pbVar15 + 4;
          }
          for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
            *pbVar15 = *pbVar12;
            pbVar12 = pbVar12 + 1;
            pbVar15 = pbVar15 + 1;
          }
          iVar5 = -1;
          pbVar12 = abStack_84;
          do {
            pbVar15 = pbVar12;
            if (iVar5 == 0) break;
            iVar5 = iVar5 + -1;
            pbVar15 = pbVar12 + 1;
            bVar4 = *pbVar12;
            pbVar12 = pbVar15;
          } while (bVar4 != 0);
          *(undefined4 *)(pbVar15 + -1) = DAT_005df820;
          uVar10 = 0xffffffff;
          pbVar15[3] = DAT_005df824;
          pcVar13 = acStack_40;
          do {
            pcVar14 = pcVar13;
            if (uVar10 == 0) break;
            uVar10 = uVar10 - 1;
            pcVar14 = pcVar13 + 1;
            cVar1 = *pcVar13;
            pcVar13 = pcVar14;
          } while (cVar1 != '\0');
          uVar10 = ~uVar10;
          iVar5 = -1;
          pbVar12 = abStack_84;
          do {
            pbVar15 = pbVar12;
            if (iVar5 == 0) break;
            iVar5 = iVar5 + -1;
            pbVar15 = pbVar12 + 1;
            bVar4 = *pbVar12;
            pbVar12 = pbVar15;
          } while (bVar4 != 0);
          pbVar12 = (byte *)(pcVar14 + -uVar10);
          pbVar15 = pbVar15 + -1;
          for (uVar9 = uVar10 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
            *(undefined4 *)pbVar15 = *(undefined4 *)pbVar12;
            pbVar12 = pbVar12 + 4;
            pbVar15 = pbVar15 + 4;
          }
          iVar5 = *param_1;
          for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
            *pbVar15 = *pbVar12;
            pbVar12 = pbVar12 + 1;
            pbVar15 = pbVar15 + 1;
          }
          iVar5 = (**(code **)(iVar5 + 0x1f0))(abStack_84);
          if (iVar5 == 0) {
            uVar10 = 0xffffffff;
            pcVar13 = (char *)(param_1[0x38] + 4);
            do {
              pcVar14 = pcVar13;
              if (uVar10 == 0) break;
              uVar10 = uVar10 - 1;
              pcVar14 = pcVar13 + 1;
              cVar1 = *pcVar13;
              pcVar13 = pcVar14;
            } while (cVar1 != '\0');
            uVar10 = ~uVar10;
            pbVar12 = (byte *)(pcVar14 + -uVar10);
            pbVar15 = abStack_84;
            for (uVar9 = uVar10 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
              *(undefined4 *)pbVar15 = *(undefined4 *)pbVar12;
              pbVar12 = pbVar12 + 4;
              pbVar15 = pbVar15 + 4;
            }
            for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
              *pbVar15 = *pbVar12;
              pbVar12 = pbVar12 + 1;
              pbVar15 = pbVar15 + 1;
            }
            iVar5 = -1;
            pbVar12 = abStack_84;
            do {
              pbVar15 = pbVar12;
              if (iVar5 == 0) break;
              iVar5 = iVar5 + -1;
              pbVar15 = pbVar12 + 1;
              bVar4 = *pbVar12;
              pbVar12 = pbVar15;
            } while (bVar4 != 0);
            *(undefined4 *)(pbVar15 + -1) = DAT_005df828;
            iVar5 = -1;
            pbVar15[3] = DAT_005df82c;
            pbVar12 = abStack_84;
            do {
              pbVar15 = pbVar12;
              if (iVar5 == 0) break;
              iVar5 = iVar5 + -1;
              pbVar15 = pbVar12 + 1;
              bVar4 = *pbVar12;
              pbVar12 = pbVar15;
            } while (bVar4 != 0);
            iVar5 = *param_1;
            *(undefined4 *)(pbVar15 + -1) = DAT_005df830;
            pbVar15[3] = DAT_005df834;
            iVar5 = (**(code **)(iVar5 + 0x1f0))(abStack_84);
            if (iVar5 == 0) {
              uVar10 = 0xffffffff;
              pcVar13 = &DAT_005df838;
              do {
                pcVar14 = pcVar13;
                if (uVar10 == 0) break;
                uVar10 = uVar10 - 1;
                pcVar14 = pcVar13 + 1;
                cVar1 = *pcVar13;
                pcVar13 = pcVar14;
              } while (cVar1 != '\0');
              uVar10 = ~uVar10;
              iVar5 = *param_1;
              pbVar12 = (byte *)(pcVar14 + -uVar10);
              pbVar15 = abStack_84;
              for (uVar9 = uVar10 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
                *(undefined4 *)pbVar15 = *(undefined4 *)pbVar12;
                pbVar12 = pbVar12 + 4;
                pbVar15 = pbVar15 + 4;
              }
              for (uVar10 = uVar10 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
                *pbVar15 = *pbVar12;
                pbVar12 = pbVar12 + 1;
                pbVar15 = pbVar15 + 1;
              }
              iVar5 = (**(code **)(iVar5 + 0x1f0))(abStack_84);
              if (iVar5 == 0) {
                abStack_84[0] = 0;
                goto LAB_004c4df0;
              }
            }
          }
        }
        if (abStack_84[0] == 0) goto LAB_004c4df0;
      }
      iVar5 = FUN_00482fb0();
      uStack_4 = 0;
      if (iVar5 == 0) {
        uStack_90 = 0;
      }
      else {
        uStack_90 = FUN_004da9f0_TActionBlock_Ctor1(abStack_84);
      }
      uStack_4 = 0xffffffff;
      *(byte **)(uStack_90 + 0x4c) = pbStack_94;
    }
    else {
      pbStack_94 = (byte *)0x0;
      if ((pbVar7 == (byte *)0x0) &&
         (iVar5 = *(int *)(param_5 + 0x48), pbVar7 = pbStack_94, iVar5 != 0)) {
        pbVar7 = (byte *)(iVar5 + 0xf8);
        iVar6 = 0;
        if (0 < *(int *)(iVar5 + 0xe8)) {
          pbStack_8c = (byte *)(param_5 + 4);
          pbVar12 = pbStack_8c;
          pbVar15 = pbVar7;
LAB_004c4db3:
          do {
            bVar4 = *pbVar7;
            bVar16 = bVar4 < *pbVar12;
            if (bVar4 == *pbVar12) {
              if (bVar4 != 0) {
                bVar4 = pbVar7[1];
                bVar16 = bVar4 < pbVar12[1];
                if (bVar4 != pbVar12[1]) goto LAB_004c4dd7;
                pbVar7 = pbVar7 + 2;
                pbVar12 = pbVar12 + 2;
                if (bVar4 != 0) goto LAB_004c4db3;
              }
              iVar8 = 0;
            }
            else {
LAB_004c4dd7:
              iVar8 = (1 - (uint)bVar16) - (uint)(bVar16 != 0);
            }
            pbVar7 = pbVar15;
            if (iVar8 == 0) break;
            iVar6 = iVar6 + 1;
            pbVar7 = pbVar15 + 0x5c;
            pbVar12 = pbStack_8c;
            pbVar15 = pbVar7;
          } while (iVar6 < *(int *)(iVar5 + 0xe8));
        }
      }
    }
LAB_004c4df0:
    pbStack_94 = pbVar7;
    if (param_6 != (int *)0x0) {
      iVar5 = FUN_0046ea90_AngleTo();
      *(char *)((int)param_1 + 0x36) = (char)iVar5;
      param_1[0x2c] = iVar5;
    }
    if (uStack_90 != 0) {
      iVar5 = param_1[0x36];
      *(int *)(uStack_90 + 0x50) = param_2;
      *(undefined4 *)(uStack_90 + 0x44) = *(undefined4 *)(iVar5 + 0x44);
      *(uint *)(uStack_90 + 0x60) = *(uint *)(uStack_90 + 0x60) | 0x30;
      iVar5 = FUN_0058ad30(uStack_90 + 4);
      if (iVar5 == 0) {
        *(uint *)(uStack_90 + 0x60) = *(uint *)(uStack_90 + 0x60) | 0x800;
      }
      iVar5 = param_1[0x36];
      uVar3 = *(undefined4 *)(iVar5 + 0x2c);
      *(undefined4 *)(uStack_90 + 0x30) = uVar3;
      *(undefined4 *)(uStack_90 + 0x2c) = uVar3;
      if ((param_6 != (int *)0x0) && (param_6[0x36] != 0)) {
        *(undefined4 *)(uStack_90 + 0x48) = *(undefined4 *)(param_6[0x36] + 0x48);
      }
      if ((((*(int *)(iVar5 + 0x44) != 0) && (pbStack_94 != (byte *)0x0)) &&
          (*(int *)(pbStack_94 + 0x54) == 0)) && ((*(uint *)(pbStack_94 + 0x24) & 0x100) == 0)) {
        uVar9 = FUN_0046ea90_AngleTo();
        uVar10 = *(uint *)(pbStack_94 + 0x24);
        if ((uVar10 & 0x200) == 0) {
          if ((uVar10 & 0x400) != 0) {
            uVar9 = uVar9 + 0x40;
            goto LAB_004c524d;
          }
          if ((uVar10 & 0x800) != 0) {
            uVar9 = uVar9 - 0x40;
            goto LAB_004c524d;
          }
        }
        else {
          uVar9 = uVar9 - 0x80;
LAB_004c524d:
          uVar9 = uVar9 & 0xff;
        }
        *(uint *)(uStack_90 + 0x30) = uVar9;
        *(uint *)(uStack_90 + 0x2c) = uVar9;
      }
      param_5 = uStack_90;
      *(uint *)(param_1[0x36] + 0x60) = *(uint *)(param_1[0x36] + 0x60) & 0xffffffef;
      *(uint *)(param_1[0x37] + 0x60) = *(uint *)(param_1[0x37] + 0x60) & 0xffffffef;
      (**(code **)(*param_1 + 0x218))(param_1[0x38],0);
      (**(code **)(*param_1 + 0x218))(uStack_90,0,0);
    }
  }
  else {
    uVar10 = param_5;
    if (*(int *)param_1[0x36] == 8) {
      if (((param_5 == 0) || (*(int *)(param_5 + 0x4c) == 0)) ||
         (((*(uint *)(*(int *)(param_5 + 0x48) + 0x24) & 0x2000000) == 0 &&
          ((*(byte *)(*(int *)(param_5 + 0x4c) + 0x24) & 0x80) == 0)))) {
        abStack_84[0] = 0;
        FUN_004ce1b0_BuildActionName(abStack_84);
        iVar5 = (**(code **)(*param_1 + 0x1f0))(abStack_84);
        if (iVar5 == 0) {
          uVar9 = 0xffffffff;
          pcVar13 = s_blockimpact_005df854;
          do {
            pcVar14 = pcVar13;
            if (uVar9 == 0) break;
            uVar9 = uVar9 - 1;
            pcVar14 = pcVar13 + 1;
            cVar1 = *pcVar13;
            pcVar13 = pcVar14;
          } while (cVar1 != '\0');
          uVar9 = ~uVar9;
          iVar5 = *param_1;
          pbVar7 = (byte *)(pcVar14 + -uVar9);
          pbVar12 = abStack_84;
          for (uVar11 = uVar9 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
            *(undefined4 *)pbVar12 = *(undefined4 *)pbVar7;
            pbVar7 = pbVar7 + 4;
            pbVar12 = pbVar12 + 4;
          }
          for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
            *pbVar12 = *pbVar7;
            pbVar7 = pbVar7 + 1;
            pbVar12 = pbVar12 + 1;
          }
          iVar5 = (**(code **)(iVar5 + 0x1f0))(abStack_84);
          if (iVar5 == 0) {
            abStack_84[0] = 0;
            goto LAB_004c5354;
          }
        }
        if (abStack_84[0] != 0) {
          if (param_5 != 0) {
            if (*(int *)(param_5 + 0x5c) != 0) {
              FUN_00482f80();
            }
            FUN_004830f0();
          }
          iVar5 = FUN_00482fb0();
          uStack_4 = 1;
          if (iVar5 == 0) {
            uVar10 = 0;
          }
          else {
            uVar10 = FUN_004da9f0_TActionBlock_Ctor1(abStack_84);
          }
          uStack_4 = 0xffffffff;
          bVar4 = *(byte *)((int)param_1 + 0x36);
          *(undefined4 *)(uVar10 + 0x4c) = 0;
          *(uint *)(uVar10 + 0x60) = *(uint *)(uVar10 + 0x60) | 0x10;
          *(uint *)(uVar10 + 0x2c) = (uint)bVar4;
        }
      }
    }
    else if ((param_5 == 0) ||
            ((pbStack_94 = *(byte **)(param_5 + 0x4c), pbStack_94 != (byte *)0x0 &&
             ((pbStack_94[0x24] & 4) != 0)))) {
      iVar5 = 0;
      pbStack_8c = (byte *)s_impact_005df860;
      pbStack_94 = (byte *)(param_1[0x3f] + 0x218);
      pbVar7 = pbStack_8c;
      if (0 < *(int *)(param_1[0x3f] + 0x214)) {
        do {
          iVar8 = (**(code **)(*param_1 + 0x1c0))();
          iVar6 = param_2;
          if (param_2 < iVar8) {
            iVar6 = (**(code **)(*param_1 + 0x1c0))();
          }
          if (iVar6 < 1) {
            iVar6 = 1;
          }
          else {
            iVar8 = (**(code **)(*param_1 + 0x1c0))();
            iVar6 = param_2;
            if (param_2 < iVar8) {
              iVar6 = (**(code **)(*param_1 + 0x1c0))();
            }
          }
          iVar6 = (param_2 * 100) / iVar6;
          if (((((*(int *)(pbStack_94 + 0x4c) <= iVar6) && (iVar6 <= *(int *)(pbStack_94 + 0x50)))
               && (uVar9 = *(uint *)(pbStack_94 + 0x24), (uVar9 & uStack_98) == uStack_98)) &&
              (((uVar9 & 4) == 0 && (((uVar9 & 8) == 0 || (*(int *)param_1[0x36] == 0xd)))))) &&
             ((((uVar9 & 0x10) == 0 || (*(int *)param_1[0x36] == 0xe)) &&
              ((iVar6 = (**(code **)(*param_1 + 0x1f0))(pbStack_94), iVar6 != 0 &&
               ((pbVar7 = pbStack_94, pbStack_94[0x28] == 0 ||
                (iVar6 = (**(code **)(*param_1 + 0x1f0))(pbStack_94 + 0x28), pbVar7 = pbStack_94,
                iVar6 != 0)))))))) break;
          iVar5 = iVar5 + 1;
          pbStack_94 = pbStack_94 + 0x5c;
          pbVar7 = pbStack_8c;
        } while (iVar5 < *(int *)(param_1[0x3f] + 0x214));
      }
      pbStack_8c = pbVar7;
      if (iVar5 < *(int *)(param_1[0x3f] + 0x214)) {
        if (pbStack_94 == (byte *)0x0) goto LAB_004c55eb;
      }
      else {
        pbStack_94 = (byte *)0x0;
LAB_004c55eb:
        iVar5 = (**(code **)(*param_1 + 0x1f0))(pbStack_8c);
        if (iVar5 == 0) goto LAB_004c5354;
      }
      if (pbStack_8c != (byte *)0x0) {
        if (param_5 != 0) {
          if (*(int *)(param_5 + 0x5c) != 0) {
            FUN_00482f80();
          }
          FUN_004830f0();
        }
        iVar5 = FUN_00482fb0();
        uStack_4 = 2;
        if (iVar5 == 0) {
          uVar10 = 0;
        }
        else {
          uVar10 = FUN_004da9f0_TActionBlock_Ctor1(pbStack_8c);
        }
        uStack_4 = 0xffffffff;
        *(byte **)(uVar10 + 0x4c) = pbStack_94;
        if (pbStack_94 != (byte *)0x0) {
          *(undefined4 *)(uVar10 + 0x28) = *(undefined4 *)(pbStack_94 + 0x48);
        }
      }
    }
LAB_004c5354:
    if (uVar10 != 0) {
      iVar5 = param_1[0x36];
      *(int *)(uVar10 + 0x50) = param_2;
      iVar5 = *(int *)(iVar5 + 0x44);
      *(int *)(uVar10 + 0x44) = iVar5;
      *(uint *)(uVar10 + 0x60) = *(uint *)(uVar10 + 0x60) | 0x20;
      if ((param_6 != (int *)0x0) && (param_6[0x36] != 0)) {
        *(undefined4 *)(uVar10 + 0x48) = *(undefined4 *)(param_6[0x36] + 0x48);
      }
      if ((((iVar5 != 0) && (pbStack_94 != (byte *)0x0)) && (*(int *)(pbStack_94 + 0x54) == 0)) &&
         ((*(uint *)(pbStack_94 + 0x24) & 0x100) == 0)) {
        uVar11 = FUN_0046ea90_AngleTo();
        uVar9 = *(uint *)(pbStack_94 + 0x24);
        if ((uVar9 & 0x200) == 0) {
          if ((uVar9 & 0x400) != 0) {
            uVar11 = uVar11 + 0x40;
            goto LAB_004c56a5;
          }
          if ((uVar9 & 0x800) != 0) {
            uVar11 = uVar11 - 0x40;
            goto LAB_004c56a5;
          }
        }
        else {
          uVar11 = uVar11 - 0x80;
LAB_004c56a5:
          uVar11 = uVar11 & 0xff;
        }
        *(uint *)(uVar10 + 0x30) = uVar11;
        *(uint *)(uVar10 + 0x2c) = uVar11;
      }
      *(uint *)(param_1[0x36] + 0x60) = *(uint *)(param_1[0x36] + 0x60) & 0xffffffef;
      iVar5 = param_1[0x38];
      *(uint *)(param_1[0x37] + 0x60) = *(uint *)(param_1[0x37] + 0x60) & 0xffffffef;
      (**(code **)(*param_1 + 0x218))(iVar5,0);
      (**(code **)(*param_1 + 0x218))(uVar10,0,0);
      iVar5 = *(int *)(uVar10 + 0x2c);
      *(char *)((int)param_1 + 0x36) = (char)iVar5;
      param_1[0x2c] = iVar5;
      param_5 = uVar10;
    }
  }
  if (((pbStack_94 != (byte *)0x0) && (iVar5 = *(int *)(pbStack_94 + 0x54), 0 < iVar5)) &&
     (param_6 != (int *)0x0)) {
    param_1[0x88] = *(int *)(pbStack_94 + 0x58);
    bVar4 = *(char *)((int)param_6 + 0x36) + 0x80;
    *(byte *)((int)param_1 + 0x36) = bVar4;
    param_1[0x2c] = (uint)bVar4;
    FUN_0046f010(param_6,iVar5);
    (**(code **)(*param_1 + 0xc))();
  }
LAB_004c5769:
  if (DAT_0067682c != 0) {
    FUN_00584b30(param_1,uStack_50,param_2,param_3,param_4,(param_1[0x36] != param_5) - 1 & param_5)
    ;
  }
  iVar5 = (**(code **)(*param_1 + 0x1c0))();
  if ((iVar5 <= *(int *)(param_1[0x3f] + 0x440)) &&
     (iVar5 = (**(code **)(*param_1 + 0x1c0))(), 0 < iVar5)) {
    param_1[0x97] = *(int *)(param_1[0x3f] + 0x448);
  }
  ExceptionList = pvStack_c;
  return;
}


