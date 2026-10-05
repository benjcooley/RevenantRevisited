// FUN_00472430 @ 00472430 size=1356

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00472430(int *param_1,int param_2,int param_3)

{
  byte bVar1;
  byte bVar2;
  undefined1 uVar3;
  short sVar4;
  undefined2 uVar5;
  byte *pbVar6;
  uint *puVar7;
  uint uVar8;
  undefined2 *puVar9;
  int iVar10;
  undefined4 *puVar11;
  undefined1 *puVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  int iVar16;
  undefined4 uVar17;
  int iVar18;
  uint uVar19;
  uint local_10;
  int local_c;
  
  iVar16 = param_2;
  bVar1 = **(byte **)(param_2 + 4);
  *(byte **)(param_2 + 4) = *(byte **)(param_2 + 4) + 1;
  if (bVar1 != 0) {
    iVar13 = FUN_00482ef0(bVar1 + 1);
    iVar18 = 0;
    param_1[0xe] = iVar13;
    if (bVar1 != 0) {
      do {
        pbVar6 = *(byte **)(param_2 + 4);
        bVar2 = *pbVar6;
        *(byte *)(iVar13 + iVar18) = bVar2;
        *(byte *)(iVar13 + iVar18) = bVar2 & 0x7f;
        iVar18 = iVar18 + 1;
        *(byte **)(param_2 + 4) = pbVar6 + 1;
      } while (iVar18 < (int)(uint)bVar1);
    }
    *(undefined1 *)(iVar13 + iVar18) = 0;
  }
  puVar7 = *(uint **)(param_2 + 4);
  uVar14 = puVar7[2];
  uVar8 = *puVar7;
  param_1[4] = puVar7[1];
  uVar19 = puVar7[3];
  param_1[5] = uVar14;
  uVar14 = param_1[2];
  param_1[6] = uVar19;
  *(uint **)(param_2 + 4) = puVar7 + 4;
  uVar14 = (uVar14 ^ uVar8) & 0x3ff1ffd7 ^ uVar14;
  param_1[2] = uVar14;
  if ((param_3 < 6) || ((uVar14 & 1) == 0)) {
    param_1[7] = puVar7[4];
    param_1[8] = puVar7[5];
    param_1[9] = puVar7[6];
    *(uint **)(param_2 + 4) = puVar7 + 7;
  }
  pbVar6 = *(byte **)(param_2 + 4);
  if (param_3 < 9) {
    bVar2 = *pbVar6;
    *(byte **)(param_2 + 4) = pbVar6 + 1;
    *(ushort *)(param_1 + 3) = (ushort)bVar2;
  }
  else {
    *(undefined2 *)(param_1 + 3) = *(undefined2 *)pbVar6;
    *(byte **)(param_2 + 4) = pbVar6 + 2;
  }
  if ((param_3 < 6) || ((uVar14 & 0x80000) == 0)) {
    *(undefined2 *)((int)param_1 + 0xe) = 0;
  }
  else {
    pbVar6 = *(byte **)(param_2 + 4);
    if (param_3 < 9) {
      bVar2 = *pbVar6;
      *(byte **)(param_2 + 4) = pbVar6 + 1;
      *(ushort *)((int)param_1 + 0xe) = (ushort)bVar2;
    }
    else {
      *(undefined2 *)((int)param_1 + 0xe) = *(undefined2 *)pbVar6;
      *(byte **)(param_2 + 4) = pbVar6 + 2;
    }
  }
  if (param_3 < 5) {
    bVar1 = **(byte **)(param_2 + 4);
    *(byte **)(param_2 + 4) = *(byte **)(param_2 + 4) + 1;
  }
  puVar9 = *(undefined2 **)(param_2 + 4);
  if (param_3 < 3) {
    *(undefined1 *)((int)param_1 + 0x36) = *(undefined1 *)puVar9;
    iVar13 = *(int *)((int)puVar9 + 7);
    *(undefined2 **)(param_2 + 4) = puVar9 + 6;
    param_1[0x14] = iVar13;
    *(undefined2 *)(param_1 + 0x1f) = 0xffff;
    param_1[0x10] = -1;
  }
  else {
    *(undefined2 *)(param_1 + 0x1f) = *puVar9;
    *(undefined2 *)((int)param_1 + 0x7e) = puVar9[1];
    param_1[0x14] = *(int *)(puVar9 + 2);
    *(undefined1 *)(param_1 + 0xd) = *(undefined1 *)(puVar9 + 4);
    *(undefined1 *)((int)param_1 + 0x35) = *(undefined1 *)((int)puVar9 + 9);
    *(undefined1 *)((int)param_1 + 0x36) = *(undefined1 *)(puVar9 + 5);
    param_1[0x10] = *(int *)((int)puVar9 + 0xb);
    *(undefined1 **)(param_2 + 4) = (undefined1 *)((int)puVar9 + 0xf);
  }
  param_1[0x2c] = (uint)*(byte *)((int)param_1 + 0x36);
  if (param_3 < 5) {
    *(undefined2 *)(param_1 + 0x17) = 0;
    *(undefined2 *)((int)param_1 + 0x5e) = 1;
    *(undefined1 *)((int)param_1 + 0x37) = 0;
    sVar4 = *(short *)(param_1[0x12] + 0x1c);
    if (0 < sVar4) {
      FUN_004785e0((int)sVar4);
      *(short *)(param_1 + 0x2a) = sVar4;
      iVar13 = 0;
      if (0 < *(short *)(param_1[0x12] + 0x1c)) {
        do {
          iVar18 = iVar13 * 4;
          iVar15 = FUN_0044ce10((int)*(short *)((int)param_1 + 6));
          iVar13 = iVar13 + 1;
          iVar10 = param_1[0x12];
          *(undefined4 *)(iVar18 + param_1[0x2b]) =
               *(undefined4 *)(*(int *)(iVar15 + 0x18) + iVar18);
        } while (iVar13 < *(short *)(iVar10 + 0x1c));
      }
    }
    (**(code **)(*param_1 + 0x1c4))(bVar1);
  }
  else {
    if ((param_3 < 6) || ((uVar14 & 0x4000) != 0)) {
      puVar9 = *(undefined2 **)(param_2 + 4);
      uVar5 = puVar9[1];
      *(undefined2 *)(param_1 + 0x17) = *puVar9;
      *(undefined2 *)((int)param_1 + 0x5e) = uVar5;
      *(undefined2 **)(param_2 + 4) = puVar9 + 2;
    }
    iVar13 = param_1[0x12];
    uVar3 = **(undefined1 **)(param_2 + 4);
    *(undefined1 **)(param_2 + 4) = *(undefined1 **)(param_2 + 4) + 1;
    sVar4 = *(short *)(iVar13 + 0x1c);
    *(undefined1 *)((int)param_1 + 0x37) = uVar3;
    if (0 < sVar4) {
      FUN_004785e0((int)sVar4);
      *(short *)(param_1 + 0x2a) = sVar4;
    }
    bVar1 = **(byte **)(param_2 + 4);
    *(byte **)(param_2 + 4) = *(byte **)(param_2 + 4) + 1;
    if (bVar1 != 0) {
      uVar14 = 0;
      local_10 = 0;
      local_c = 0;
      if (bVar1 != 0) {
        param_2 = 0;
        do {
          if ((int)(short)param_1[0x2a] <= (int)uVar14) break;
          puVar11 = *(undefined4 **)(iVar16 + 4);
          uVar17 = *puVar11;
          uVar8 = puVar11[1];
          *(undefined4 **)(iVar16 + 4) = puVar11 + 2;
          iVar13 = param_1[0x12];
          if (uVar14 < (uint)(int)*(short *)(iVar13 + 0x1c)) {
            uVar19 = *(uint *)(*(int *)(iVar13 + 0x20) + 0x4c + param_2);
          }
          else {
            uVar19 = 0;
          }
          if ((uVar8 & 0x7f7f7f7f) == uVar19) {
            local_10 = uVar14 + 1;
            *(undefined4 *)(param_1[0x2b] + -4 + local_10 * 4) = uVar17;
            param_2 = param_2 + 0x50;
            uVar14 = local_10;
          }
          else {
            uVar19 = 0;
            if (0 < *(short *)(iVar13 + 0x1c)) {
              iVar18 = 0;
              do {
                if (uVar19 < (uint)(int)*(short *)(iVar13 + 0x1c)) {
                  uVar14 = *(uint *)(*(int *)(iVar13 + 0x20) + 0x4c + iVar18);
                }
                else {
                  uVar14 = 0;
                }
                if ((uVar8 & 0x7f7f7f7f) == uVar14) {
                  *(undefined4 *)(param_1[0x2b] + uVar19 * 4) = uVar17;
                }
                uVar19 = uVar19 + 1;
                iVar18 = iVar18 + 0x50;
                uVar14 = local_10;
              } while ((int)uVar19 < (int)*(short *)(iVar13 + 0x1c));
            }
          }
          local_c = local_c + 1;
        } while (local_c < (int)(uint)bVar1);
      }
    }
    if (((param_1[2] & 0x4000000U) != 0) && (iVar13 = 0, 0 < *(short *)(param_1[0x12] + 0x1c))) {
      do {
        if ((iVar13 < 3) || (5 < iVar13)) {
          iVar18 = FUN_0044ce10((int)*(short *)((int)param_1 + 6));
          *(undefined4 *)(param_1[0x2b] + iVar13 * 4) =
               *(undefined4 *)(*(int *)(iVar18 + 0x18) + iVar13 * 4);
        }
        iVar13 = iVar13 + 1;
      } while (iVar13 < *(short *)(param_1[0x12] + 0x1c));
    }
  }
  uVar14 = param_1[2];
  if ((uVar14 & 4) != 0) {
    puVar12 = *(undefined1 **)(iVar16 + 4);
    *(undefined1 *)(param_1 + 0x22) = *puVar12;
    param_1[0x24] = *(int *)(puVar12 + 1);
    param_1[0x25] = *(int *)(puVar12 + 5);
    param_1[0x26] = *(int *)(puVar12 + 9);
    *(undefined1 *)((int)param_1 + 0x8e) = puVar12[0xd];
    *(undefined1 *)((int)param_1 + 0x8d) = puVar12[0xe];
    *(undefined1 *)(param_1 + 0x23) = puVar12[0xf];
    *(undefined1 *)((int)param_1 + 0x89) = puVar12[0x10];
    *(undefined2 *)((int)param_1 + 0x8a) = *(undefined2 *)(puVar12 + 0x11);
    *(undefined1 **)(iVar16 + 4) = puVar12 + 0x13;
    param_1[2] = uVar14 | 0x4004;
  }
  if ((uint)(int)(short)param_1[1] < DAT_0065a258) {
    iVar16 = (&DAT_0065a148)[(short)param_1[1]];
  }
  else {
    iVar16 = 0;
  }
  param_1[0x12] = iVar16;
  if (iVar16 == 0) {
    FUN_00481c10(s_Bad_object_class__005d4a0c,0);
  }
  iVar16 = param_1[0x12];
  uVar14 = (uint)*(short *)((int)param_1 + 6);
  if (((*(int *)(iVar16 + 0x34) == 0) || (*(uint *)(iVar16 + 0x24) <= uVar14)) ||
     (*(int *)(*(int *)(iVar16 + 0x34) + uVar14 * 4) == 0)) {
    iVar13 = 0;
  }
  else {
    iVar13 = *(int *)(*(int *)(iVar16 + 0x34) + uVar14 * 4);
    if (iVar13 == 0) {
      iVar13 = *(int *)(iVar16 + 0x38);
    }
  }
  iVar16 = param_1[0x21];
  param_1[0x13] = iVar13;
  if (iVar16 != 0) {
    *(undefined4 *)(iVar16 + 0xc) = 0;
    FUN_004922c0();
    FUN_004830f0(iVar16);
    param_1[0x21] = 0;
  }
  if (param_1 == DAT_0065d674) {
    _DAT_0065d548 = 1;
    (**(code **)(DAT_0065d4f8 + 0x90))();
  }
  if (param_1 == DAT_0065b088) {
    _DAT_0065b078 = 1;
  }
  if (param_3 < 0xc) {
    param_1[2] = param_1[2] & 0x3fffdff;
  }
  uVar14 = param_1[2];
  param_1[2] = uVar14 & 0xf7ffffff;
  if (((short)param_1[1] == 9) && ((uVar14 & 4) != 0)) {
    param_1[2] = uVar14 & 0xf7ff7fff;
  }
  if ((param_1[2] & 0x40000000U) != 0) {
    param_1[2] = param_1[2] | 0xc000;
  }
  if (((param_1[2] & 0x40000000U) == 0) &&
     (((short)param_1[1] != 9 || (param_1[0xe] != *(int *)param_1[0x13])))) {
    uVar17 = FUN_00497370(param_1);
    FUN_00471150(uVar17);
  }
  return;
}


