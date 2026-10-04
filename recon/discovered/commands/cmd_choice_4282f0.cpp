// FUN_004282f0 @ 004282f0 size=592

undefined4 FUN_004282f0(undefined4 param_1,int param_2,undefined4 param_3,uint *param_4)

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  char *pcVar9;
  char *pcVar10;
  char acStack_160 [2];
  undefined4 uStack_15e;
  undefined2 uStack_15a;
  int iStack_158;
  int iStack_154;
  char acStack_150 [80];
  char acStack_100 [256];
  
  if (*(int *)(param_2 + 0x10) != 4) {
    return 4;
  }
  uVar7 = 0xffffffff;
  acStack_100[0] = '\0';
  pcVar3 = *(char **)(param_2 + 0x28);
  do {
    pcVar10 = pcVar3;
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    pcVar10 = pcVar3 + 1;
    cVar1 = *pcVar3;
    pcVar3 = pcVar10;
  } while (cVar1 != '\0');
  uVar7 = ~uVar7;
  pcVar3 = pcVar10 + -uVar7;
  pcVar10 = acStack_150;
  for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
    *(undefined4 *)pcVar10 = *(undefined4 *)pcVar3;
    pcVar3 = pcVar3 + 4;
    pcVar10 = pcVar10 + 4;
  }
  for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
    *pcVar10 = *pcVar3;
    pcVar3 = pcVar3 + 1;
    pcVar10 = pcVar10 + 1;
  }
  FUN_00479580();
  iVar2 = *(int *)(param_2 + 0x10);
  if ((iVar2 != 2) && (iVar2 != 4)) {
    return 4;
  }
  do {
    if ((iVar2 == 9) || (iVar2 == 10)) {
      if (param_4 == (uint *)0x0) {
        FUN_00535870(acStack_150,acStack_100);
      }
      else {
        FUN_004932a0(acStack_150,acStack_100);
        *param_4 = *param_4 | 4;
      }
      iVar2 = *(int *)(param_2 + 0x10);
      while ((iVar2 != 9 && (iVar2 != 10))) {
        FUN_00478a10();
        iVar2 = *(int *)(param_2 + 0x10);
      }
      return 0;
    }
    uStack_15e = 0;
    acStack_160[0] = ' ';
    acStack_160[1] = 0;
    uStack_15a = 0;
    iStack_154 = 0;
    if (iVar2 == 2) {
      pcVar3 = *(char **)(param_2 + 0x28);
LAB_004284a7:
      uVar7 = 0xffffffff;
      do {
        pcVar10 = pcVar3;
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        pcVar10 = pcVar3 + 1;
        cVar1 = *pcVar3;
        pcVar3 = pcVar10;
      } while (cVar1 != '\0');
      uVar7 = ~uVar7;
      iVar2 = -1;
      pcVar3 = acStack_100;
      do {
        pcVar9 = pcVar3;
        if (iVar2 == 0) break;
        iVar2 = iVar2 + -1;
        pcVar9 = pcVar3 + 1;
        cVar1 = *pcVar3;
        pcVar3 = pcVar9;
      } while (cVar1 != '\0');
      pcVar3 = pcVar10 + -uVar7;
      pcVar10 = pcVar9 + -1;
      for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
        *(undefined4 *)pcVar10 = *(undefined4 *)pcVar3;
        pcVar3 = pcVar3 + 4;
        pcVar10 = pcVar10 + 4;
      }
      for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
        *pcVar10 = *pcVar3;
        pcVar3 = pcVar3 + 1;
        pcVar10 = pcVar10 + 1;
      }
    }
    else if (iVar2 == 4) {
      iVar2 = FUN_00497b40(*(undefined4 *)(param_2 + 0x28),param_1);
      if (iVar2 == 0) {
        iVar4 = FUN_00497800(*(undefined4 *)(param_2 + 0x28),0);
        iStack_158 = 4;
        iVar2 = 0;
        do {
          FUN_0058bad0();
          iVar5 = __ftol();
          iVar6 = iVar4 / iVar5;
          iVar4 = iVar4 - iVar5 * iVar6;
          if (iVar6 == 0) {
            if (iVar2 != 0) {
              acStack_160[0] = '0';
              goto LAB_00428455;
            }
          }
          else {
            if (iVar2 == 0) {
              iStack_154 = 1;
            }
            acStack_160[0] = (char)iVar6 + '0';
LAB_00428455:
            uVar7 = 0xffffffff;
            pcVar3 = acStack_160;
            do {
              pcVar10 = pcVar3;
              if (uVar7 == 0) break;
              uVar7 = uVar7 - 1;
              pcVar10 = pcVar3 + 1;
              cVar1 = *pcVar3;
              pcVar3 = pcVar10;
            } while (cVar1 != '\0');
            uVar7 = ~uVar7;
            iVar2 = -1;
            pcVar3 = acStack_100;
            do {
              pcVar9 = pcVar3;
              if (iVar2 == 0) break;
              iVar2 = iVar2 + -1;
              pcVar9 = pcVar3 + 1;
              cVar1 = *pcVar3;
              pcVar3 = pcVar9;
            } while (cVar1 != '\0');
            pcVar3 = pcVar10 + -uVar7;
            pcVar10 = pcVar9 + -1;
            for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
              *(undefined4 *)pcVar10 = *(undefined4 *)pcVar3;
              pcVar3 = pcVar3 + 4;
              pcVar10 = pcVar10 + 4;
            }
            for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
              *pcVar10 = *pcVar3;
              pcVar3 = pcVar3 + 1;
              pcVar10 = pcVar10 + 1;
            }
          }
          iStack_158 = iStack_158 + -1;
          iVar2 = iStack_154;
        } while (0 < iStack_158);
        pcVar3 = acStack_160;
        acStack_160[0] = (char)iVar4 + '0';
      }
      else {
        if (iVar2 != 1) {
          FUN_00533dd0(*(undefined4 *)(param_2 + 0x28),acStack_100,0x100);
          goto LAB_004284cc;
        }
        pcVar3 = (char *)FUN_00497a30(*(undefined4 *)(param_2 + 0x28),0);
      }
      goto LAB_004284a7;
    }
LAB_004284cc:
    FUN_00479580();
    iVar2 = *(int *)(param_2 + 0x10);
  } while( true );
}


