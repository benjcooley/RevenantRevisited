// FUN_00545f10 @ 00545f10 size=1149

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00545f10(int param_1,int param_2,int param_3,int param_4)

{
  byte bVar1;
  char cVar2;
  int *piVar3;
  byte *pbVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  byte *pbVar11;
  byte *pbVar12;
  char *pcVar13;
  bool bVar14;
  char *pcVar15;
  
  if (DAT_0065d0d0 == 0) {
    return;
  }
  *(int *)(param_1 + 0x1ac) = param_3;
  *(int *)(param_1 + 0x1b0) = param_4;
  FUN_00436530(param_2,param_3,param_4);
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
    param_2 = 0;
    iVar8 = 0;
    if (*(int *)(DAT_00667fcc + 0x2ec) < 1) {
      return;
    }
    do {
      iVar7 = iVar8 + *(int *)(*(int *)(param_1 + 0x1a8) + param_2 * 4);
      iVar10 = *(int *)(param_1 + 0x17c) + -0x2f + param_4;
      if ((((iVar8 < iVar10) && (iVar10 < iVar7)) && (0xc < param_3 + -0x12)) &&
         (param_3 + -0x12 < 0x34)) {
        iVar8 = iVar8 - *(int *)(param_1 + 0x17c);
        if ((iVar8 < param_4 + -0x2f) && (param_4 + -0x2f < iVar8 + 0x28)) {
          piVar3 = *(int **)(param_1 + 0x180);
          *(int *)(param_1 + 0x1c8) = param_2;
          (**(code **)(*piVar3 + 0x44))(piVar3[5],piVar3[6],0x94,0xe5);
          _DAT_0065aa28 = 1;
          (**(code **)(DAT_0065a9d8 + 0x90))();
        }
      }
      param_2 = param_2 + 1;
      iVar8 = iVar7;
    } while (param_2 < *(int *)(DAT_00667fcc + 0x2ec));
    return;
  }
  if (param_2 != 4) {
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
  pbVar4 = (byte *)FUN_0043a410();
  if (pbVar4 != (byte *)0x0) {
    iVar8 = -1;
    pbVar11 = pbVar4;
    do {
      if (iVar8 == 0) break;
      iVar8 = iVar8 + -1;
      bVar1 = *pbVar11;
      pbVar11 = pbVar11 + 1;
    } while (bVar1 != 0);
    if (iVar8 != -2) {
      iVar8 = FUN_0053ed70(pbVar4);
      if (iVar8 != 0) {
        iVar8 = 0;
        iVar10 = 0;
        if (0 < *(int *)(DAT_00667fcc + 0x2ec)) {
          do {
            pbVar11 = *(byte **)(*(int *)(DAT_00667fcc + 0x2fc) + iVar10 * 4);
            pbVar12 = pbVar4;
            do {
              bVar1 = *pbVar11;
              bVar14 = bVar1 < *pbVar12;
              if (bVar1 != *pbVar12) {
LAB_005460fc:
                iVar7 = (1 - (uint)bVar14) - (uint)(bVar14 != 0);
                goto LAB_00546101;
              }
              if (bVar1 == 0) break;
              bVar1 = pbVar11[1];
              bVar14 = bVar1 < pbVar12[1];
              if (bVar1 != pbVar12[1]) goto LAB_005460fc;
              pbVar11 = pbVar11 + 2;
              pbVar12 = pbVar12 + 2;
            } while (bVar1 != 0);
            iVar7 = 0;
LAB_00546101:
            if (iVar7 == 0) {
              *(int *)(param_1 + 0x1d0) = iVar8 - *(int *)(param_1 + 0x17c);
              break;
            }
            iVar8 = iVar8 + *(int *)(*(int *)(param_1 + 0x1a8) + iVar10 * 4);
            iVar10 = iVar10 + 1;
          } while (iVar10 < *(int *)(DAT_00667fcc + 0x2ec));
        }
      }
      goto LAB_005462fc;
    }
  }
  if (*(int *)(param_1 + 0x1c8) < 0) goto LAB_005462fc;
  iVar10 = 0;
  iVar8 = *(int *)(DAT_00667fcc + 0x2ec);
  if (0 < iVar8) {
    iVar7 = *(int *)(param_1 + 0x17c) + -0x2f + param_4;
    iVar5 = 0;
    do {
      iVar9 = iVar5 + *(int *)(*(int *)(param_1 + 0x1a8) + iVar10 * 4);
      if (((iVar5 < iVar7) && (iVar7 < iVar9)) &&
         ((0xc < param_3 + -0x12 && (param_3 + -0x12 < 0x34)))) {
        iVar5 = iVar5 - *(int *)(param_1 + 0x17c);
        if ((iVar5 < param_4 + -0x2f) && (param_4 + -0x2f < iVar5 + 0x28)) {
          *(int *)(param_1 + 0x1cc) = iVar10;
        }
      }
      iVar10 = iVar10 + 1;
      iVar5 = iVar9;
    } while (iVar10 < iVar8);
  }
  if (*(int *)(param_1 + 0x1cc) != *(int *)(param_1 + 0x1c8)) goto LAB_005462fc;
  pcVar15 = *(char **)(*(int *)(DAT_00667fcc + 0x2fc) + *(int *)(param_1 + 0x1c8) * 4);
  if (pcVar15 == (char *)0x0) {
LAB_005462d4:
    if ((DAT_0066829c != 0) && (DAT_0067682c == 0)) goto LAB_005462fc;
    pcVar15 = s_Fizzle_005e536c;
  }
  else {
    iVar8 = -1;
    pcVar13 = pcVar15;
    do {
      if (iVar8 == 0) break;
      iVar8 = iVar8 + -1;
      cVar2 = *pcVar13;
      pcVar13 = pcVar13 + 1;
    } while (cVar2 != '\0');
    if (iVar8 == -2) goto LAB_005462d4;
    iVar8 = FUN_0051b7c0(pcVar15);
    if (iVar8 != 0) {
      iVar8 = FUN_004d5c20(pcVar15,0,0,0);
      if (iVar8 == 0) {
        if ((DAT_0066829c == 0) || (DAT_0067682c != 0)) {
          FUN_004d5b90(s_Fizzle_005e532c,0,0,0);
          uVar6 = FUN_0049d800(s_SPLCASTFAIL_005e5334);
          FUN_0054d170(&DAT_0065c5d0,&DAT_005e5340,uVar6);
        }
      }
      else if ((DAT_0066829c == 0) || (DAT_0067682c != 0)) {
        uVar6 = FUN_0049d800(s_SPLCASTOK_005e5344);
        FUN_0054d170(&DAT_0065c5d0,&DAT_005e5350,uVar6);
      }
      goto LAB_005462fc;
    }
    if ((DAT_0066829c != 0) && (DAT_0067682c == 0)) goto LAB_005462fc;
    uVar6 = FUN_0049d800(s_SPLMISTAL_005e5354);
    FUN_0054d170(&DAT_0065c5d0,&DAT_005e5360,uVar6);
    pcVar15 = s_Fizzle_005e5364;
  }
  FUN_004d5b90(pcVar15,0,0,0);
LAB_005462fc:
  if (*(int *)(param_1 + 0x1c8) != -1) {
    piVar3 = *(int **)(param_1 + 0x180);
    *(undefined4 *)(param_1 + 0x1c8) = 0xffffffff;
    (**(code **)(*piVar3 + 0x44))(piVar3[5],piVar3[6],0x94,0xe5);
    _DAT_0065aa28 = 1;
    (**(code **)(DAT_0065a9d8 + 0x90))();
  }
  *(undefined4 *)(param_1 + 0x1cc) = 0xffffffff;
  FUN_0043a240(0,0,0);
  FUN_0043a100(0,0,0);
  FUN_0043a170(&DAT_00670644);
  FUN_0043a140(0);
  return;
}


