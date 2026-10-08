// FUN_00420140 @ 00420140 size=876

undefined4 FUN_00420140(undefined4 param_1,int param_2,int param_3)

{
  char cVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  char *pcVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  char *pcVar11;
  char *pcVar12;
  undefined4 *puVar13;
  int iStack_58;
  undefined4 uStack_54;
  char acStack_48 [2];
  undefined4 uStack_46;
  undefined2 uStack_42;
  char acStack_40 [32];
  char acStack_20 [32];
  
  iVar3 = param_2;
  acStack_40[0] = '\0';
  acStack_20[0] = '\0';
  iVar4 = FUN_00479700(s_nowait_005cae68,0);
  if (iVar4 != 0) {
    FUN_00479580();
  }
  uStack_54 = 0xffffffff;
  if (*(int *)(param_2 + 0x10) != 8) goto LAB_0042019b;
  uStack_54 = *(undefined4 *)(param_2 + 0x14);
LAB_00420194:
  do {
    FUN_00479580();
LAB_0042019b:
    do {
      iVar5 = FUN_00479700(&DAT_005cae70,0);
      if ((iVar5 == 0) && (iVar5 = FUN_00479700(s_sound_005cae78,0), iVar5 == 0)) {
        param_2 = -1;
        iVar5 = FUN_00479700(s_choice_005cae90,0);
        if (iVar5 != 0) {
          if ((DAT_00667ea4 < 0) ||
             (puVar13 = *(undefined4 **)(&DAT_00667e60 + DAT_00667ea4 * 4),
             puVar13 == (undefined4 *)0x0)) goto LAB_004202b8;
          goto LAB_004202aa;
        }
        if (*(int *)(iVar3 + 0x10) == 2) {
          uVar9 = 0xffffffff;
          param_2 = -1;
          pcVar6 = *(char **)(iVar3 + 0x28);
          goto code_r0x0042025d;
        }
        if (*(int *)(iVar3 + 0x10) != 4) {
          return 4;
        }
        uVar9 = 0xffffffff;
        puVar13 = &DAT_00654a88;
        pcVar6 = *(char **)(iVar3 + 0x28);
        goto code_r0x0042028f;
      }
      iVar5 = FUN_00479700(&DAT_005cae80,0);
      if (iVar5 != 0) {
        FUN_00479580();
        _strncpy(acStack_40,*(char **)(param_2 + 0x28),0x1f);
        goto LAB_00420194;
      }
      iVar5 = FUN_00479700(s_sound_005cae88,0);
    } while (iVar5 == 0);
    FUN_00479580();
    _strncpy(acStack_20,*(char **)(param_2 + 0x28),0x1f);
  } while( true );
  while( true ) {
    uVar9 = uVar9 - 1;
    pcVar12 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar12;
    if (cVar1 == '\0') break;
code_r0x0042025d:
    pcVar12 = pcVar6;
    if (uVar9 == 0) break;
  }
  uVar9 = ~uVar9;
  pcVar6 = pcVar12 + -uVar9;
  pcVar12 = (char *)&DAT_00654a88;
  for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *(undefined4 *)pcVar12 = *(undefined4 *)pcVar6;
    pcVar6 = pcVar6 + 4;
    pcVar12 = pcVar12 + 4;
  }
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *pcVar12 = *pcVar6;
    pcVar6 = pcVar6 + 1;
    pcVar12 = pcVar12 + 1;
  }
  goto LAB_004202b8;
  while( true ) {
    uVar9 = uVar9 - 1;
    pcVar12 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar12;
    if (cVar1 == '\0') break;
code_r0x0042028f:
    pcVar12 = pcVar6;
    if (uVar9 == 0) break;
  }
  uVar9 = ~uVar9;
  pcVar6 = pcVar12 + -uVar9;
  pcVar12 = (char *)&DAT_00654a88;
  for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *(undefined4 *)pcVar12 = *(undefined4 *)pcVar6;
    pcVar6 = pcVar6 + 4;
    pcVar12 = pcVar12 + 4;
  }
  for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
    *pcVar12 = *pcVar6;
    pcVar6 = pcVar6 + 1;
    pcVar12 = pcVar12 + 1;
  }
LAB_004202aa:
  param_2 = FUN_0049d6d0(puVar13);
LAB_004202b8:
  FUN_00479580();
  iVar5 = *(int *)(iVar3 + 0x10);
  do {
    if ((iVar5 == 9) || (iVar5 == 10)) {
      if (param_2 < 0) {
        if (acStack_20[0] != '\0') {
          uStack_54 = 0xffffffff;
        }
        FUN_004d0950(&DAT_00654a88,uStack_54,-(uint)(acStack_40[0] != '\0') & (uint)acStack_40,
                     -(uint)(acStack_20[0] != '\0') & (uint)acStack_20);
      }
      else {
        FUN_004d09b0(param_2,uStack_54,-(uint)(acStack_40[0] != '\0') & (uint)acStack_40);
      }
      if ((iVar4 == 0) && (param_3 != 0)) {
        FUN_00471330(param_1);
      }
      return 0;
    }
    uStack_46 = 0;
    acStack_48[0] = ' ';
    acStack_48[1] = 0;
    uStack_42 = 0;
    bVar2 = false;
    if (iVar5 == 2) {
      pcVar6 = *(char **)(iVar3 + 0x28);
LAB_004203de:
      uVar9 = 0xffffffff;
      do {
        pcVar12 = pcVar6;
        if (uVar9 == 0) break;
        uVar9 = uVar9 - 1;
        pcVar12 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar12;
      } while (cVar1 != '\0');
      uVar9 = ~uVar9;
      iVar5 = -1;
      pcVar6 = (char *)&DAT_00654a88;
      do {
        pcVar11 = pcVar6;
        if (iVar5 == 0) break;
        iVar5 = iVar5 + -1;
        pcVar11 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar11;
      } while (cVar1 != '\0');
      pcVar6 = pcVar12 + -uVar9;
      pcVar12 = pcVar11 + -1;
      for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
        *(undefined4 *)pcVar12 = *(undefined4 *)pcVar6;
        pcVar6 = pcVar6 + 4;
        pcVar12 = pcVar12 + 4;
      }
      for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
        *pcVar12 = *pcVar6;
        pcVar6 = pcVar6 + 1;
        pcVar12 = pcVar12 + 1;
      }
    }
    else if (iVar5 == 4) {
      iVar5 = FUN_00497b40(*(undefined4 *)(iVar3 + 0x28),param_1);
      if (iVar5 == 0) {
        iVar5 = FUN_00497800(*(undefined4 *)(iVar3 + 0x28),param_1);
        iStack_58 = 4;
        do {
          FUN_0058bad0();
          iVar7 = __ftol();
          iVar8 = iVar5 / iVar7;
          iVar5 = iVar5 - iVar7 * iVar8;
          if (iVar8 == 0) {
            if (bVar2) {
              acStack_48[0] = '0';
              goto LAB_00420395;
            }
          }
          else {
            if (!bVar2) {
              bVar2 = true;
            }
            acStack_48[0] = (char)iVar8 + '0';
LAB_00420395:
            uVar9 = 0xffffffff;
            pcVar6 = acStack_48;
            do {
              pcVar12 = pcVar6;
              if (uVar9 == 0) break;
              uVar9 = uVar9 - 1;
              pcVar12 = pcVar6 + 1;
              cVar1 = *pcVar6;
              pcVar6 = pcVar12;
            } while (cVar1 != '\0');
            uVar9 = ~uVar9;
            iVar7 = -1;
            pcVar6 = (char *)&DAT_00654a88;
            do {
              pcVar11 = pcVar6;
              if (iVar7 == 0) break;
              iVar7 = iVar7 + -1;
              pcVar11 = pcVar6 + 1;
              cVar1 = *pcVar6;
              pcVar6 = pcVar11;
            } while (cVar1 != '\0');
            pcVar6 = pcVar12 + -uVar9;
            pcVar12 = pcVar11 + -1;
            for (uVar10 = uVar9 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
              *(undefined4 *)pcVar12 = *(undefined4 *)pcVar6;
              pcVar6 = pcVar6 + 4;
              pcVar12 = pcVar12 + 4;
            }
            for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
              *pcVar12 = *pcVar6;
              pcVar6 = pcVar6 + 1;
              pcVar12 = pcVar12 + 1;
            }
          }
          iStack_58 = iStack_58 + -1;
        } while (0 < iStack_58);
        pcVar6 = acStack_48;
        acStack_48[0] = (char)iVar5 + '0';
      }
      else {
        if (iVar5 != 1) goto LAB_00420406;
        pcVar6 = (char *)FUN_00497a30(*(undefined4 *)(iVar3 + 0x28),param_1);
      }
      goto LAB_004203de;
    }
LAB_00420406:
    FUN_00479580();
    iVar5 = *(int *)(iVar3 + 0x10);
  } while( true );
}


