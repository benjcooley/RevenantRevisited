// FUN_0054d4a0 @ 0054d4a0 size=600

void __thiscall FUN_0054d4a0(int param_1,int param_2,int param_3)

{
  char cVar1;
  int *piVar2;
  int iVar3;
  undefined4 uVar4;
  char *pcVar5;
  char *pcVar6;
  uint uVar7;
  int iVar8;
  
  iVar8 = param_2;
  if ((DAT_0065d0d0 == 0) ||
     ((((DAT_0066829c == 0 && (DAT_00667fcc != 0)) &&
       (piVar2 = *(int **)(DAT_00667fcc + 0xe0), piVar2 != (int *)0x0)) &&
      ((*piVar2 == 3 || ((piVar2 != (int *)0x0 && (*piVar2 == 0x19)))))))) {
    if (*(int *)(param_1 + 0xa0) == 0) {
      return;
    }
  }
  else {
    if (param_3 == 0) {
      return;
    }
    if (*(int *)(param_1 + 0xa0) == 0) {
      if (param_2 != 0xd) {
        return;
      }
      DAT_0065a9c8 = DAT_0065a9c8 | DAT_0065a9c4;
      DAT_0065a9c4 = 0;
      if (DAT_00667fcc != 0) {
        FUN_004cee70(0);
        FUN_004cf000();
      }
      pcVar6 = (char *)FUN_0049d800(s_msgprefix_005e5868);
      uVar7 = 0xffffffff;
      pcVar5 = pcVar6;
      do {
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar5 + 1;
      } while (cVar1 != '\0');
      iVar8 = ~uVar7 - 1;
      *(int *)(param_1 + 0xa4) = 0x50 - iVar8;
      *(undefined1 *)(param_1 + 0xd0) = 0;
      FUN_00444e20(0);
      FUN_0054d0c0(0x20,iVar8,pcVar6);
      *(undefined4 *)(param_1 + 0xa0) = 1;
      return;
    }
    if (param_2 != 0xd) {
      if (param_2 == 8) {
        uVar7 = 0xffffffff;
        pcVar5 = (char *)(param_1 + 0xd0);
        do {
          if (uVar7 == 0) break;
          uVar7 = uVar7 - 1;
          cVar1 = *pcVar5;
          pcVar5 = pcVar5 + 1;
        } while (cVar1 != '\0');
        uVar7 = ~uVar7;
        if (((int)(uVar7 - 1) < 2) || ((*(byte *)(uVar7 + 0xcd + param_1) & 0x80) == 0)) {
          if (0 < (int)(uVar7 - 1)) {
            *(undefined1 *)(uVar7 + 0xce + param_1) = 0;
          }
        }
        else {
          *(undefined1 *)(uVar7 + 0xce + param_1) = 0;
          *(undefined1 *)(uVar7 + 0xcd + param_1) = 0;
        }
      }
      else {
        if (param_2 < 0x100) {
          if (param_2 < 0x20) goto LAB_0054d5f1;
          uVar7 = 0xffffffff;
          param_2._0_2_ = (ushort)(byte)param_2;
          pcVar5 = (char *)(param_1 + 0xd0);
          do {
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            cVar1 = *pcVar5;
            pcVar5 = pcVar5 + 1;
          } while (cVar1 != '\0');
          iVar8 = *(int *)(param_1 + 0xa4) - (~uVar7 - 1);
          pcVar5 = (char *)(param_1 + 0xd0) + (~uVar7 - 1);
        }
        else {
          param_2 = CONCAT31(CONCAT21(param_2._2_2_,(byte)param_2),(char)((uint)param_2 >> 8));
          uVar4 = param_2;
          uVar7 = 0xffffffff;
          param_2._3_1_ = SUB41(iVar8,3);
          param_2._0_2_ = (ushort)uVar4;
          param_2._0_3_ = (uint3)(ushort)param_2;
          pcVar5 = (char *)(param_1 + 0xd0);
          do {
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            cVar1 = *pcVar5;
            pcVar5 = pcVar5 + 1;
          } while (cVar1 != '\0');
          iVar8 = *(int *)(param_1 + 0xa4) - (~uVar7 - 1);
          pcVar5 = (char *)(param_1 + 0xd0) + (~uVar7 - 1);
        }
        _strncpy(pcVar5,(char *)&param_2,iVar8 - 1);
        pcVar5[iVar8 + -1] = '\0';
      }
LAB_0054d5f1:
      pcVar5 = (char *)FUN_0049d800(s_msgprefix_005e5878);
      iVar8 = *(int *)(param_1 + 0x6c);
      _strncpy((char *)(iVar8 + 0xc),pcVar5,0x4f);
      iVar3 = *(int *)(param_1 + 0x6c);
      *(undefined1 *)(iVar8 + 0x5b) = 0;
      pcVar5 = (char *)(iVar3 + 0xc);
      uVar7 = 0xffffffff;
      pcVar6 = pcVar5;
      do {
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar1 != '\0');
      iVar8 = -(~uVar7 - 1);
      pcVar5 = pcVar5 + (~uVar7 - 1);
      _strncpy(pcVar5,(char *)(param_1 + 0xd0),iVar8 + 0x4f);
      pcVar5[iVar8 + 0x4f] = '\0';
      FUN_0054cd40(0);
      return;
    }
  }
  FUN_0054d390();
  return;
}


