// FUN_004a2960 @ 004a2960 size=890

undefined4 __thiscall FUN_004a2960(int *param_1,ushort *param_2,int param_3)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  undefined4 *puVar4;
  int iVar5;
  uint uVar6;
  undefined4 *puVar7;
  uint uVar8;
  ushort *puVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint *puVar14;
  uint uVar15;
  int local_68;
  int local_64;
  int local_60;
  uint local_54;
  int local_48;
  undefined2 local_38;
  int iStack_36;
  undefined2 uStack_32;
  undefined2 local_30;
  undefined2 uStack_2e;
  undefined2 local_2c;
  uint local_28 [3];
  undefined2 local_1c;
  undefined2 local_1a;
  undefined4 local_18;
  undefined4 local_14;
  
  if (((0 < *param_1) && (0 < param_1[1])) && ((*(byte *)(param_1 + 4) & 6) != 0)) {
    if (param_3 < 1) {
      param_3 = 1;
    }
    else if (8 < param_3) {
      param_3 = 8;
    }
    uVar11 = *param_1 / param_3 + 3U & 0xfffffffc;
    iVar3 = FUN_0058b5db(param_2,&DAT_005dad44);
    if (iVar3 != 0) {
      local_38 = 0x4d42;
      puVar14 = local_28;
      for (iVar5 = 10; iVar5 != 0; iVar5 = iVar5 + -1) {
        *puVar14 = 0;
        puVar14 = puVar14 + 1;
      }
      uStack_2e = 0x36;
      local_2c = 0;
      iStack_36 = ((param_1[1] / param_3) * uVar11 + 0x12) * 3;
      uStack_32 = 0;
      local_30 = 0;
      iVar5 = FUN_0058beb8(&local_38,0xe,1,iVar3);
      if (iVar5 == 1) {
        local_28[2] = param_1[1] / param_3;
        local_28[0] = 0x28;
        local_1c = 1;
        local_1a = 0x18;
        local_18 = 0;
        local_14 = 0;
        local_28[1] = uVar11;
        iVar5 = FUN_0058beb8(local_28,0x28,1,iVar3);
        if (iVar5 == 1) {
          uVar2 = uVar11 * 3;
          puVar4 = (undefined4 *)FUN_00482fb0(uVar2);
          if (puVar4 == (undefined4 *)0x0) {
            FUN_0058b4f1(iVar3);
            return 0;
          }
          puVar7 = puVar4;
          for (uVar6 = uVar2 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
            *puVar7 = 0;
            puVar7 = puVar7 + 1;
          }
          local_48 = 0;
          for (iVar5 = 0; iVar5 != 0; iVar5 = iVar5 + -1) {
            *(undefined1 *)puVar7 = 0;
            puVar7 = (undefined4 *)((int)puVar7 + 1);
          }
          local_68 = (int)param_1 + (param_1[1] + -1) * *param_1 * 2 + 0x48;
          uVar6 = param_3 * param_3;
          if (0 < param_1[1] / param_3) {
            do {
              iVar5 = *param_1;
              param_2 = (ushort *)(local_68 + (param_3 + -1) * iVar5 * -4);
              if ((int)param_2 < (int)(param_1 + 0x12)) {
                param_2 = (ushort *)(param_1 + 0x12);
              }
              puVar7 = puVar4;
              local_54 = uVar11;
              if (0 < (int)uVar11) {
                do {
                  uVar13 = 0;
                  uVar15 = 0;
                  uVar12 = 0;
                  local_60 = param_3;
                  if (0 < param_3) {
                    do {
                      puVar9 = param_2;
                      local_64 = param_3;
                      do {
                        uVar1 = *puVar9;
                        if ((param_1[4] & 4U) == 0) {
                          uVar10 = (uint)(uVar1 >> 7);
                          uVar8 = (uVar1 & 0x3e0) >> 2;
                        }
                        else {
                          uVar10 = (uint)(byte)(uVar1 >> 8);
                          uVar8 = (uVar1 & 0x7e0) >> 3;
                        }
                        uVar13 = uVar13 + (uVar10 & 0xf8);
                        uVar15 = uVar15 + uVar8;
                        uVar12 = uVar12 + (byte)((char)*puVar9 << 3);
                        puVar9 = puVar9 + iVar5;
                        local_64 = local_64 + -1;
                      } while (local_64 != 0);
                      param_2 = param_2 + 1;
                      local_60 = local_60 + -1;
                    } while (local_60 != 0);
                  }
                  *(char *)puVar7 = (char)(uVar12 / uVar6);
                  *(char *)((int)puVar7 + 1) = (char)(uVar15 / uVar6);
                  *(char *)((int)puVar7 + 2) = (char)(uVar13 / uVar6);
                  local_54 = local_54 - 1;
                  puVar7 = (undefined4 *)((int)puVar7 + 3);
                } while (local_54 != 0);
              }
              local_68 = local_68 + iVar5 * param_3 * -2;
              iVar5 = FUN_0058beb8(puVar4,uVar2,1,iVar3);
              if (iVar5 != 1) {
                FUN_004830f0(puVar4);
                FUN_0058b4f1(iVar3);
                return 0;
              }
              local_48 = local_48 + 1;
            } while (local_48 < param_1[1] / param_3);
          }
          FUN_004830f0(puVar4);
          FUN_0058b4f1(iVar3);
          return 1;
        }
      }
      FUN_0058b4f1(iVar3);
    }
  }
  return 0;
}


