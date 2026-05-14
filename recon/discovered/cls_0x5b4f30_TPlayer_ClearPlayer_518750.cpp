// FUN_00518750_TPlayer_ClearPlayer @ 00518750 size=844

void __fastcall FUN_00518750_TPlayer_ClearPlayer(int *param_1)

{
  short sVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  
  *(undefined1 *)(param_1 + 0xde) = 0;
  param_1[0xe6] = 0;
  param_1[0xe7] = 0;
  param_1[0x199] = -1;
  piVar2 = param_1 + 0x124;
  for (iVar4 = 0x18; iVar4 != 0; iVar4 = iVar4 + -1) {
    *piVar2 = 0;
    piVar2 = piVar2 + 1;
  }
  piVar2 = param_1 + 0x15c;
  for (iVar4 = 0x38; iVar4 != 0; iVar4 = iVar4 + -1) {
    *piVar2 = 0;
    piVar2 = piVar2 + 1;
  }
  param_1[0x197] = 0;
  param_1[0x196] = 0;
  param_1[0x195] = 0;
  param_1[0x194] = 0;
  piVar2 = param_1 + 0xa8;
  for (iVar4 = 0xb; iVar4 != 0; iVar4 = iVar4 + -1) {
    *piVar2 = 0;
    piVar2 = piVar2 + 1;
  }
  iVar7 = 0;
  param_1[2] = param_1[2] | 0x80000;
  piVar2 = param_1 + 0xb3;
  for (iVar4 = 7; iVar4 != 0; iVar4 = iVar4 + -1) {
    *piVar2 = 0;
    piVar2 = piVar2 + 1;
  }
  *(undefined2 *)piVar2 = 0;
  param_1[0xc0] = 0;
  if (0 < param_1[0xbb]) {
    do {
      if ((-1 < iVar7) && (iVar4 = *(int *)(param_1[0xbf] + iVar7 * 4), iVar4 != 0)) {
        FUN_004830f0(iVar4);
      }
      FUN_0041cb40(iVar7);
      iVar7 = iVar7 + 1;
    } while (iVar7 < param_1[0xbb]);
  }
  param_1[0xbb] = 0;
  param_1[0xbc] = 0;
  (**(code **)(*param_1 + 0x40))(param_1[2] & 0xfbffffff);
  param_1[0xd9] = 0;
  param_1[0xd8] = 0;
  param_1[0xda] = 0;
  iVar4 = 0x54;
  iVar7 = 6;
  piVar2 = (int *)(*(int *)(param_1[0x3f] + 400) + 0x20);
  piVar6 = piVar2;
  do {
    iVar5 = *piVar6;
    if (iVar5 < 0) {
      iVar5 = -iVar5;
    }
    iVar4 = iVar4 - iVar5;
    piVar6 = piVar6 + 1;
    iVar7 = iVar7 + -1;
  } while (iVar7 != 0);
  uVar9 = (iVar4 * 0x10000) / 6 + 1;
  uVar8 = uVar9;
  iVar4 = 0x22;
  do {
    iVar7 = *piVar2;
    if (iVar7 == 0) {
      (**(code **)(*param_1 + 0xe8))(iVar4,(int)uVar8 >> 0x10);
      uVar8 = uVar8 + (uVar9 - (uVar8 & 0xffff0000));
    }
    else {
      if (iVar7 < 0) {
        iVar7 = -iVar7;
      }
      (**(code **)(*param_1 + 0xe8))(iVar4,iVar7);
    }
    piVar2 = piVar2 + 1;
    iVar7 = iVar4 + -0x21;
    iVar4 = iVar4 + 1;
  } while (iVar7 < 6);
  iVar4 = 0;
  do {
    (**(code **)(*param_1 + 0xe8))(iVar4 + 0x49,0x1e);
    iVar4 = iVar4 + 1;
  } while (iVar4 < 0xb);
  piVar2 = param_1 + 0xd3;
  param_1[0xd5] = -1;
  if (param_1[0xd4] != 0) {
    FUN_004830f0(param_1[0xd4]);
  }
  iVar4 = 0;
  param_1[0xd4] = 0;
  *(short *)piVar2 = 0;
  sVar1 = *(short *)(param_1[0x12] + 0x1c);
  *(undefined2 *)((int)param_1 + 0x34e) = 0;
  if (0 < sVar1) {
    do {
      iVar7 = FUN_0051ff30(iVar4);
      if (iVar7 != 0) {
        if (*(short *)((int)param_1 + 0x34e) <= (short)*piVar2) {
          FUN_0051ffa0(0xffffffff);
        }
        iVar7 = *piVar2;
        *(short *)piVar2 = (short)iVar7 + 1;
        piVar6 = (int *)(param_1[0xd4] + (short)iVar7 * 8);
        *piVar6 = iVar4;
        piVar6[1] = 0;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < *(short *)(param_1[0x12] + 0x1c));
  }
  FUN_0051c660_TPlayer_RefreshStats();
  *(undefined1 *)(param_1 + 0x13c) = 0;
  param_1[0xdb] = 1;
  FUN_00472db0(&DAT_005e2714,0);
  iVar4 = FUN_0047e920_PlayScreen_GameFrame();
  param_1[0xdd] = iVar4;
  iVar4 = FUN_0047e940();
  param_1[0xdc] = iVar4;
  param_1[0x198] = 0;
  piVar2 = param_1 + 0xe8;
  iVar4 = 10;
  do {
    *piVar2 = 0;
    piVar2 = piVar2 + 6;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  uVar9 = 0;
  if (0 < (short)param_1[0xd6]) {
    do {
      iVar4 = *(int *)(param_1[0xd7] + uVar9 * 8);
      if (iVar4 != 0) {
        FUN_004830f0(iVar4);
      }
      iVar4 = param_1[0xd6];
      if (uVar9 < (uint)(int)(short)iVar4) {
        iVar7 = *(short *)((int)param_1 + 0x35a) + -1;
        if ((int)uVar9 < iVar7) {
          puVar3 = (undefined4 *)(param_1[0xd7] + uVar9 * 8);
          iVar7 = iVar7 - uVar9;
          do {
            *puVar3 = puVar3[2];
            puVar3[1] = puVar3[3];
            puVar3 = puVar3 + 2;
            iVar7 = iVar7 + -1;
          } while (iVar7 != 0);
        }
        sVar1 = (short)iVar4 + -1;
        *(short *)(param_1 + 0xd6) = sVar1;
        if (sVar1 < 1) {
          if (param_1[0xd7] != 0) {
            FUN_004830f0(param_1[0xd7]);
          }
          param_1[0xd7] = 0;
          *(undefined2 *)(param_1 + 0xd6) = 0;
          *(undefined2 *)((int)param_1 + 0x35a) = 0;
        }
      }
      uVar9 = uVar9 + 1;
    } while ((int)uVar9 < (int)(short)param_1[0xd6]);
  }
  if (param_1[0xd7] != 0) {
    FUN_004830f0(param_1[0xd7]);
  }
  param_1[0xd7] = 0;
  *(undefined2 *)(param_1 + 0xd6) = 0;
  *(undefined2 *)((int)param_1 + 0x35a) = 0;
  return;
}


