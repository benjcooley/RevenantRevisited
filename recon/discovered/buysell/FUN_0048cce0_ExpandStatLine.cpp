// FUN_0048cce0 @ 0048cce0 size=1214

char * FUN_0048cce0(char *param_1)

{
  char *pcVar1;
  char *pcVar2;
  char *pcVar3;
  int iVar4;
  char *pcVar5;
  char cVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  char *pcVar10;
  char *local_20;
  int local_1c;
  int local_14;
  int local_10;
  char *local_c;
  
  local_14 = 0;
  if ((param_1 != (char *)0x0) && (*param_1 != '\0')) {
    uVar7 = 0xffffffff;
    pcVar1 = param_1;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar6 = *pcVar1;
      pcVar1 = pcVar1 + 1;
    } while (cVar6 != '\0');
    iVar8 = ~uVar7 - 1;
    if (iVar8 != 0) {
      pcVar1 = (char *)FUN_00482fb0(0x80);
      cVar6 = *param_1;
      local_20 = param_1;
      while (cVar6 == ' ') {
        pcVar2 = local_20 + 1;
        local_20 = local_20 + 1;
        cVar6 = *pcVar2;
      }
      cVar6 = *local_20;
      local_1c = iVar8;
      do {
        if ((cVar6 == '\0') || (iVar8 <= (int)local_20 - (int)param_1)) {
          pcVar1[local_14] = '\0';
          return pcVar1;
        }
        local_10 = 0x80;
        local_c = (char *)0x7;
        pcVar2 = (char *)FUN_00482fb0(0x80);
        uVar7 = 0xffffffff;
        pcVar5 = &DAT_005d9c70;
        do {
          pcVar3 = pcVar5;
          if (uVar7 == 0) break;
          uVar7 = uVar7 - 1;
          pcVar3 = pcVar5 + 1;
          cVar6 = *pcVar5;
          pcVar5 = pcVar3;
        } while (cVar6 != '\0');
        uVar7 = ~uVar7;
        pcVar5 = pcVar3 + -uVar7;
        pcVar3 = pcVar2;
        for (uVar9 = uVar7 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
          *(undefined4 *)pcVar3 = *(undefined4 *)pcVar5;
          pcVar5 = pcVar5 + 4;
          pcVar3 = pcVar3 + 4;
        }
        for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
          *pcVar3 = *pcVar5;
          pcVar5 = pcVar5 + 1;
          pcVar3 = pcVar3 + 1;
        }
        cVar6 = *local_20;
        while ((cVar6 != ' ' && (cVar6 != '\0'))) {
          local_20 = local_20 + 1;
          pcVar2[(int)local_c] = cVar6;
          local_c = (char *)((int)local_c + 1);
          if (local_10 + -2 <= (int)local_c) {
            local_10 = local_10 + 0x20;
            pcVar3 = (char *)FUN_00482fb0(local_1c);
            uVar7 = 0xffffffff;
            pcVar2[(int)local_c] = '\0';
            pcVar5 = pcVar2;
            do {
              pcVar10 = pcVar5;
              if (uVar7 == 0) break;
              uVar7 = uVar7 - 1;
              pcVar10 = pcVar5 + 1;
              cVar6 = *pcVar5;
              pcVar5 = pcVar10;
            } while (cVar6 != '\0');
            uVar7 = ~uVar7;
            pcVar5 = pcVar10 + -uVar7;
            pcVar10 = pcVar3;
            for (uVar9 = uVar7 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
              *(undefined4 *)pcVar10 = *(undefined4 *)pcVar5;
              pcVar5 = pcVar5 + 4;
              pcVar10 = pcVar10 + 4;
            }
            for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
              *pcVar10 = *pcVar5;
              pcVar5 = pcVar5 + 1;
              pcVar10 = pcVar10 + 1;
            }
            FUN_004830f0(pcVar2);
            pcVar2 = (char *)FUN_00482fb0(local_10);
            uVar7 = 0xffffffff;
            pcVar5 = pcVar3;
            do {
              pcVar10 = pcVar5;
              if (uVar7 == 0) break;
              uVar7 = uVar7 - 1;
              pcVar10 = pcVar5 + 1;
              cVar6 = *pcVar5;
              pcVar5 = pcVar10;
            } while (cVar6 != '\0');
            uVar7 = ~uVar7;
            pcVar5 = pcVar10 + -uVar7;
            pcVar10 = pcVar2;
            for (uVar9 = uVar7 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
              *(undefined4 *)pcVar10 = *(undefined4 *)pcVar5;
              pcVar5 = pcVar5 + 4;
              pcVar10 = pcVar10 + 4;
            }
            for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
              *pcVar10 = *pcVar5;
              pcVar5 = pcVar5 + 1;
              pcVar10 = pcVar10 + 1;
            }
            FUN_004830f0(pcVar3);
          }
          cVar6 = *local_20;
        }
        pcVar2[(int)local_c] = '\0';
        local_c = (char *)FUN_0049d800(pcVar2);
        FUN_004830f0(pcVar2);
        cVar6 = *local_20;
        while (cVar6 == ' ') {
          pcVar2 = local_20 + 1;
          local_20 = local_20 + 1;
          cVar6 = *pcVar2;
        }
        cVar6 = *local_c;
        if (cVar6 != '\0') {
          iVar4 = local_1c + -2;
          do {
            if (iVar8 <= (int)local_20 - (int)param_1) break;
            pcVar1[local_14] = cVar6;
            local_c = local_c + 1;
            local_14 = local_14 + 1;
            if (iVar4 <= local_14) {
              local_1c = local_1c + 0x20;
              iVar4 = iVar4 + 0x20;
              pcVar5 = (char *)FUN_00482fb0(local_1c);
              uVar7 = 0xffffffff;
              pcVar1[local_14] = '\0';
              pcVar2 = pcVar1;
              do {
                pcVar3 = pcVar2;
                if (uVar7 == 0) break;
                uVar7 = uVar7 - 1;
                pcVar3 = pcVar2 + 1;
                cVar6 = *pcVar2;
                pcVar2 = pcVar3;
              } while (cVar6 != '\0');
              uVar7 = ~uVar7;
              pcVar2 = pcVar3 + -uVar7;
              pcVar3 = pcVar5;
              for (uVar9 = uVar7 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
                *(undefined4 *)pcVar3 = *(undefined4 *)pcVar2;
                pcVar2 = pcVar2 + 4;
                pcVar3 = pcVar3 + 4;
              }
              for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
                *pcVar3 = *pcVar2;
                pcVar2 = pcVar2 + 1;
                pcVar3 = pcVar3 + 1;
              }
              FUN_004830f0(pcVar1);
              pcVar1 = (char *)FUN_00482fb0(local_1c);
              uVar7 = 0xffffffff;
              pcVar2 = pcVar5;
              do {
                pcVar3 = pcVar2;
                if (uVar7 == 0) break;
                uVar7 = uVar7 - 1;
                pcVar3 = pcVar2 + 1;
                cVar6 = *pcVar2;
                pcVar2 = pcVar3;
              } while (cVar6 != '\0');
              uVar7 = ~uVar7;
              pcVar2 = pcVar3 + -uVar7;
              pcVar3 = pcVar1;
              for (uVar9 = uVar7 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
                *(undefined4 *)pcVar3 = *(undefined4 *)pcVar2;
                pcVar2 = pcVar2 + 4;
                pcVar3 = pcVar3 + 4;
              }
              for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
                *pcVar3 = *pcVar2;
                pcVar2 = pcVar2 + 1;
                pcVar3 = pcVar3 + 1;
              }
              FUN_004830f0(pcVar5);
            }
            cVar6 = *local_c;
          } while (cVar6 != '\0');
        }
        pcVar1[local_14] = ' ';
        local_14 = local_14 + 1;
        if (local_1c + -2 <= local_14) {
          local_1c = local_1c + 0x20;
          pcVar5 = (char *)FUN_00482fb0(local_1c);
          uVar7 = 0xffffffff;
          pcVar1[local_14] = '\0';
          pcVar2 = pcVar1;
          do {
            pcVar3 = pcVar2;
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            pcVar3 = pcVar2 + 1;
            cVar6 = *pcVar2;
            pcVar2 = pcVar3;
          } while (cVar6 != '\0');
          uVar7 = ~uVar7;
          pcVar2 = pcVar3 + -uVar7;
          pcVar3 = pcVar5;
          for (uVar9 = uVar7 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
            *(undefined4 *)pcVar3 = *(undefined4 *)pcVar2;
            pcVar2 = pcVar2 + 4;
            pcVar3 = pcVar3 + 4;
          }
          for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
            *pcVar3 = *pcVar2;
            pcVar2 = pcVar2 + 1;
            pcVar3 = pcVar3 + 1;
          }
          FUN_004830f0(pcVar1);
          pcVar1 = (char *)FUN_00482fb0(local_1c);
          uVar7 = 0xffffffff;
          pcVar2 = pcVar5;
          do {
            pcVar3 = pcVar2;
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            pcVar3 = pcVar2 + 1;
            cVar6 = *pcVar2;
            pcVar2 = pcVar3;
          } while (cVar6 != '\0');
          uVar7 = ~uVar7;
          pcVar2 = pcVar3 + -uVar7;
          pcVar3 = pcVar1;
          for (uVar9 = uVar7 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
            *(undefined4 *)pcVar3 = *(undefined4 *)pcVar2;
            pcVar2 = pcVar2 + 4;
            pcVar3 = pcVar3 + 4;
          }
          for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
            *pcVar3 = *pcVar2;
            pcVar2 = pcVar2 + 1;
            pcVar3 = pcVar3 + 1;
          }
          FUN_004830f0(pcVar5);
        }
        iVar4 = local_1c + -2;
        while (((((cVar6 = *local_20, '/' < cVar6 && (cVar6 < ':')) || (cVar6 == '-')) ||
                ((cVar6 == '+' || (cVar6 == '%')))) &&
               ((cVar6 != '\0' && ((int)local_20 - (int)param_1 < iVar8))))) {
          pcVar1[local_14] = cVar6;
          local_20 = local_20 + 1;
          local_14 = local_14 + 1;
          if (iVar4 <= local_14) {
            local_1c = local_1c + 0x20;
            iVar4 = iVar4 + 0x20;
            pcVar5 = (char *)FUN_00482fb0(local_1c);
            uVar7 = 0xffffffff;
            pcVar1[local_14] = '\0';
            pcVar2 = pcVar1;
            do {
              pcVar3 = pcVar2;
              if (uVar7 == 0) break;
              uVar7 = uVar7 - 1;
              pcVar3 = pcVar2 + 1;
              cVar6 = *pcVar2;
              pcVar2 = pcVar3;
            } while (cVar6 != '\0');
            uVar7 = ~uVar7;
            pcVar2 = pcVar3 + -uVar7;
            pcVar3 = pcVar5;
            for (uVar9 = uVar7 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
              *(undefined4 *)pcVar3 = *(undefined4 *)pcVar2;
              pcVar2 = pcVar2 + 4;
              pcVar3 = pcVar3 + 4;
            }
            for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
              *pcVar3 = *pcVar2;
              pcVar2 = pcVar2 + 1;
              pcVar3 = pcVar3 + 1;
            }
            FUN_004830f0(pcVar1);
            pcVar1 = (char *)FUN_00482fb0(local_1c);
            uVar7 = 0xffffffff;
            pcVar2 = pcVar5;
            do {
              pcVar3 = pcVar2;
              if (uVar7 == 0) break;
              uVar7 = uVar7 - 1;
              pcVar3 = pcVar2 + 1;
              cVar6 = *pcVar2;
              pcVar2 = pcVar3;
            } while (cVar6 != '\0');
            uVar7 = ~uVar7;
            pcVar2 = pcVar3 + -uVar7;
            pcVar3 = pcVar1;
            for (uVar9 = uVar7 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
              *(undefined4 *)pcVar3 = *(undefined4 *)pcVar2;
              pcVar2 = pcVar2 + 4;
              pcVar3 = pcVar3 + 4;
            }
            for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
              *pcVar3 = *pcVar2;
              pcVar2 = pcVar2 + 1;
              pcVar3 = pcVar3 + 1;
            }
            FUN_004830f0(pcVar5);
          }
          cVar6 = *local_20;
          while (cVar6 == ' ') {
            pcVar2 = local_20 + 1;
            local_20 = local_20 + 1;
            cVar6 = *pcVar2;
          }
        }
        pcVar1[local_14] = ' ';
        local_14 = local_14 + 1;
        if (local_1c + -2 <= local_14) {
          local_1c = local_1c + 0x20;
          pcVar5 = (char *)FUN_00482fb0(local_1c);
          uVar7 = 0xffffffff;
          pcVar1[local_14] = '\0';
          pcVar2 = pcVar1;
          do {
            pcVar3 = pcVar2;
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            pcVar3 = pcVar2 + 1;
            cVar6 = *pcVar2;
            pcVar2 = pcVar3;
          } while (cVar6 != '\0');
          uVar7 = ~uVar7;
          pcVar2 = pcVar3 + -uVar7;
          pcVar3 = pcVar5;
          for (uVar9 = uVar7 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
            *(undefined4 *)pcVar3 = *(undefined4 *)pcVar2;
            pcVar2 = pcVar2 + 4;
            pcVar3 = pcVar3 + 4;
          }
          for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
            *pcVar3 = *pcVar2;
            pcVar2 = pcVar2 + 1;
            pcVar3 = pcVar3 + 1;
          }
          FUN_004830f0(pcVar1);
          pcVar1 = (char *)FUN_00482fb0(local_1c);
          uVar7 = 0xffffffff;
          pcVar2 = pcVar5;
          do {
            pcVar3 = pcVar2;
            if (uVar7 == 0) break;
            uVar7 = uVar7 - 1;
            pcVar3 = pcVar2 + 1;
            cVar6 = *pcVar2;
            pcVar2 = pcVar3;
          } while (cVar6 != '\0');
          uVar7 = ~uVar7;
          pcVar2 = pcVar3 + -uVar7;
          pcVar3 = pcVar1;
          for (uVar9 = uVar7 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
            *(undefined4 *)pcVar3 = *(undefined4 *)pcVar2;
            pcVar2 = pcVar2 + 4;
            pcVar3 = pcVar3 + 4;
          }
          for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
            *pcVar3 = *pcVar2;
            pcVar2 = pcVar2 + 1;
            pcVar3 = pcVar3 + 1;
          }
          FUN_004830f0(pcVar5);
        }
        cVar6 = *local_20;
        while (cVar6 == ' ') {
          pcVar2 = local_20 + 1;
          local_20 = local_20 + 1;
          cVar6 = *pcVar2;
        }
        cVar6 = *local_20;
      } while( true );
    }
  }
  return (char *)0x0;
}


