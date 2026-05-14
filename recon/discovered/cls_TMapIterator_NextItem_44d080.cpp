// FUN_0044d080_TMapIterator_NextItem @ 0044d080 size=859

uint __fastcall FUN_0044d080_TMapIterator_NextItem(uint *param_1)

{
  uint uVar1;
  bool bVar2;
  int iVar3;
  int *piVar4;
  undefined4 *puVar5;
  uint uVar6;
  int local_38;
  int local_30;
  int iStack_2c;
  int iStack_28;
  int iStack_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  if ((*param_1 & 0x20) == 0) {
    if ((param_1[3] != 0) && (iVar3 = FUN_00470040(), 0 < iVar3)) {
      param_1[8] = 0;
      param_1[7] = param_1[3];
    }
    uVar6 = param_1[7];
    param_1[3] = 0;
    while (uVar6 != 0) {
      if (param_1[3] != 0) goto LAB_0044d3c6;
      uVar6 = param_1[8];
      iVar3 = FUN_00470040();
      if ((int)uVar6 < iVar3) {
        param_1[8] = uVar6 + 1;
        uVar6 = FUN_00470110(uVar6);
        param_1[3] = uVar6;
      }
      else {
        uVar6 = *(uint *)(param_1[7] + 100);
        param_1[8] = (int)*(short *)(param_1[7] + 0x7e) + 1;
        param_1[7] = uVar6;
      }
      uVar6 = param_1[7];
    }
  }
  else {
    param_1[3] = 0;
  }
  uVar6 = param_1[3];
  do {
    if (uVar6 != 0) {
LAB_0044d3c6:
      return param_1[3];
    }
    uVar6 = param_1[2];
    param_1[2] = uVar6 + 1;
    if ((int)param_1[6] <= (int)(uVar6 + 1)) {
      do {
        iVar3 = DAT_00668578;
        uVar6 = param_1[4] + 1;
        param_1[4] = uVar6;
        if (iVar3 <= (int)uVar6) {
          return 0;
        }
        uVar6 = *(uint *)(DAT_00668588 + uVar6 * 4);
        if ((uVar6 != 0) &&
           ((uVar1 = *param_1, (uVar1 & 0x200) != 0 || (*(int *)(uVar6 + 0x98) != 0)))) {
          if (((uVar1 & 0x100) == 0) && ((uVar1 & 0x80) != 0)) {
            if (((uVar1 & 0x400) == 0) || (param_1[9] == *(uint *)(uVar6 + 4))) {
LAB_0044d1b1:
              if ((uVar1 & 1) == 0) {
                if (((uVar1 & 0x40) == 0) ||
                   (((FUN_00498f60_TSector_GetMaxMapRect(&local_10), (int)param_1[10] <= local_8 &&
                     (local_10 <= (int)param_1[0xc])) &&
                    (((int)param_1[0xb] <= local_4 && (local_c <= (int)param_1[0xd]))))))
                goto LAB_0044d231;
              }
              else {
                FUN_00498eb0_TSector_GetMaxScreenRect(&local_20);
                if (((((int)param_1[10] <= local_18) && (local_20 <= (int)param_1[0xc])) &&
                    ((int)param_1[0xb] <= local_14)) && (local_1c <= (int)param_1[0xd])) {
LAB_0044d231:
                  param_1[5] = uVar6;
                  if (param_1[1] == 0) {
                    uVar6 = *(uint *)(uVar6 + 0xb8);
                  }
                  else {
                    uVar6 = *(uint *)(uVar6 + 0xb4 + param_1[1] * 0x18);
                  }
                  param_1[6] = uVar6;
                  param_1[2] = 0;
                }
              }
            }
          }
          else if (((((int)param_1[0xe] <= *(int *)(uVar6 + 8)) &&
                    (*(int *)(uVar6 + 8) <= (int)param_1[0x10])) &&
                   ((int)param_1[0xf] <= *(int *)(uVar6 + 0xc))) &&
                  ((*(int *)(uVar6 + 0xc) <= (int)param_1[0x11] &&
                   (param_1[9] == *(uint *)(uVar6 + 4))))) goto LAB_0044d1b1;
        }
      } while ((int)param_1[6] <= (int)param_1[2]);
    }
    uVar6 = param_1[1];
    uVar1 = param_1[5];
    if (uVar6 == 0) {
      piVar4 = *(int **)(*(int *)(uVar1 + 200) + param_1[2] * 4);
    }
    else {
      piVar4 = *(int **)(*(int *)(uVar1 + 0xc4 + uVar6 * 0x18) + param_1[2] * 4);
      if (piVar4 == (int *)0x0) {
        piVar4 = *(int **)(uVar1 + uVar6 * 0x18 + 200);
      }
      piVar4 = *(int **)(*(int *)(uVar1 + 200) + *piVar4 * 4);
    }
    param_1[3] = (uint)piVar4;
    if ((piVar4 == (int *)0x0) || (uVar6 = *param_1, (uVar6 & 0x5e) == 0)) goto LAB_0044d3bd;
    if ((((uVar6 & 0x10) == 0) || ((*(byte *)(piVar4 + 2) & 8) == 0)) &&
       (((uVar6 & 8) == 0 || ((*(byte *)(piVar4 + 2) & 0x80) == 0)))) {
      bVar2 = false;
      if ((uVar6 & 2) == 0) {
        if ((uVar6 & 0x40) != 0) {
          bVar2 = true;
          puVar5 = (undefined4 *)FUN_0045f660(piVar4[4],piVar4[5]);
          local_38 = FUN_0041c720(*puVar5,puVar5[1]);
        }
      }
      else {
        bVar2 = true;
        (**(code **)(*piVar4 + 0xf4))(&local_30);
        if ((((iStack_28 < (int)param_1[10]) || ((int)param_1[0xc] < local_30)) ||
            (iStack_24 < (int)param_1[0xb])) || ((int)param_1[0xd] < iStack_2c)) {
          local_38 = 0;
        }
        else {
          local_38 = 1;
        }
      }
      if ((((*param_1 & 4) == 0) || ((*(byte *)(param_1[3] + 8) & 4) == 0)) ||
         ((*(char *)(param_1[3] + 0x89) == '\0' || ((bVar2 && (local_38 != 0)))))) {
        if ((bVar2) && (local_38 == 0)) goto LAB_0044d3ba;
      }
      else {
        FUN_00471430(&local_30);
        if ((((iStack_28 < (int)param_1[10]) || ((int)param_1[0xc] < local_30)) ||
            (iStack_24 < (int)param_1[0xb])) || ((int)param_1[0xd] < iStack_2c)) {
          local_38 = 0;
          goto LAB_0044d3ba;
        }
        local_38 = 1;
      }
    }
    else {
LAB_0044d3ba:
      param_1[3] = 0;
    }
LAB_0044d3bd:
    uVar6 = param_1[3];
  } while( true );
}


