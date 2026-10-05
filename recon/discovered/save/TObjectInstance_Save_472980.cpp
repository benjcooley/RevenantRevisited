// FUN_00472980 @ 00472980 size=973

void __thiscall FUN_00472980(int param_1,int param_2)

{
  byte bVar1;
  undefined1 uVar2;
  undefined2 uVar3;
  undefined1 *puVar4;
  undefined2 *puVar5;
  uint *puVar6;
  byte *pbVar7;
  int iVar8;
  uint uVar9;
  undefined4 *puVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  byte bVar13;
  uint uVar14;
  byte *pbVar15;
  bool bVar16;
  
  if ((*(uint *)(param_1 + 8) & 4) != 0) {
    *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) | 0xc000;
  }
  pbVar7 = (byte *)**(undefined4 **)(param_1 + 0x4c);
  pbVar15 = *(byte **)(param_1 + 0x38);
  do {
    bVar13 = *pbVar7;
    bVar16 = bVar13 < *pbVar15;
    if (bVar13 != *pbVar15) {
LAB_004729c6:
      iVar8 = (1 - (uint)bVar16) - (uint)(bVar16 != 0);
      goto LAB_004729cb;
    }
    if (bVar13 == 0) break;
    bVar13 = pbVar7[1];
    bVar16 = bVar13 < pbVar15[1];
    if (bVar13 != pbVar15[1]) goto LAB_004729c6;
    pbVar7 = pbVar7 + 2;
    pbVar15 = pbVar15 + 2;
  } while (bVar13 != 0);
  iVar8 = 0;
LAB_004729cb:
  if (iVar8 == 0) {
    FUN_004779d0(1);
    puVar4 = *(undefined1 **)(param_2 + 8);
    *puVar4 = 0;
    *(undefined1 **)(param_2 + 8) = puVar4 + 1;
  }
  else {
    iVar8 = -1;
    pbVar7 = *(byte **)(param_1 + 0x38);
    do {
      if (iVar8 == 0) break;
      iVar8 = iVar8 + -1;
      bVar13 = *pbVar7;
      pbVar7 = pbVar7 + 1;
    } while (bVar13 != 0);
    bVar13 = ~(byte)iVar8 - 1;
    FUN_004779d0(1);
    pbVar7 = *(byte **)(param_2 + 8);
    iVar8 = 0;
    *pbVar7 = bVar13;
    *(byte **)(param_2 + 8) = pbVar7 + 1;
    if (bVar13 != 0) {
      do {
        bVar1 = *(byte *)(*(int *)(param_1 + 0x38) + iVar8);
        FUN_004779d0(1);
        pbVar7 = *(byte **)(param_2 + 8);
        *pbVar7 = bVar1 | 0x80;
        *(byte **)(param_2 + 8) = pbVar7 + 1;
        iVar8 = iVar8 + 1;
      } while (iVar8 < (int)(uint)bVar13);
    }
  }
  uVar11 = *(undefined4 *)(param_1 + 8);
  FUN_004779d0(4);
  puVar10 = *(undefined4 **)(param_2 + 8);
  *puVar10 = uVar11;
  uVar11 = *(undefined4 *)(param_1 + 0x10);
  *(undefined4 **)(param_2 + 8) = puVar10 + 1;
  FUN_004779d0(4);
  puVar10 = *(undefined4 **)(param_2 + 8);
  *puVar10 = uVar11;
  uVar11 = *(undefined4 *)(param_1 + 0x14);
  *(undefined4 **)(param_2 + 8) = puVar10 + 1;
  FUN_004779d0(4);
  puVar10 = *(undefined4 **)(param_2 + 8);
  *puVar10 = uVar11;
  uVar11 = *(undefined4 *)(param_1 + 0x18);
  *(undefined4 **)(param_2 + 8) = puVar10 + 1;
  FUN_004779d0(4);
  puVar10 = *(undefined4 **)(param_2 + 8);
  *puVar10 = uVar11;
  *(undefined4 **)(param_2 + 8) = puVar10 + 1;
  if ((*(byte *)(param_1 + 8) & 1) == 0) {
    uVar11 = *(undefined4 *)(param_1 + 0x1c);
    FUN_004779d0(4);
    puVar10 = *(undefined4 **)(param_2 + 8);
    *puVar10 = uVar11;
    uVar11 = *(undefined4 *)(param_1 + 0x20);
    *(undefined4 **)(param_2 + 8) = puVar10 + 1;
    FUN_004779d0(4);
    puVar10 = *(undefined4 **)(param_2 + 8);
    *puVar10 = uVar11;
    uVar11 = *(undefined4 *)(param_1 + 0x24);
    *(undefined4 **)(param_2 + 8) = puVar10 + 1;
    FUN_004779d0(4);
    puVar10 = *(undefined4 **)(param_2 + 8);
    *puVar10 = uVar11;
    *(undefined4 **)(param_2 + 8) = puVar10 + 1;
  }
  uVar3 = *(undefined2 *)(param_1 + 0xc);
  FUN_004779d0(2);
  puVar5 = *(undefined2 **)(param_2 + 8);
  *puVar5 = uVar3;
  *(undefined2 **)(param_2 + 8) = puVar5 + 1;
  if ((*(uint *)(param_1 + 8) & 0x80000) != 0) {
    uVar3 = *(undefined2 *)(param_1 + 0xe);
    FUN_004779d0(2);
    puVar5 = *(undefined2 **)(param_2 + 8);
    *puVar5 = uVar3;
    *(undefined2 **)(param_2 + 8) = puVar5 + 1;
  }
  uVar3 = *(undefined2 *)(param_1 + 0x7c);
  FUN_004779d0(2);
  puVar5 = *(undefined2 **)(param_2 + 8);
  *puVar5 = uVar3;
  uVar3 = *(undefined2 *)(param_1 + 0x7e);
  *(undefined2 **)(param_2 + 8) = puVar5 + 1;
  FUN_004779d0(2);
  puVar5 = *(undefined2 **)(param_2 + 8);
  *puVar5 = uVar3;
  uVar11 = *(undefined4 *)(param_1 + 0x50);
  *(undefined2 **)(param_2 + 8) = puVar5 + 1;
  FUN_004779d0(4);
  puVar10 = *(undefined4 **)(param_2 + 8);
  uVar2 = *(undefined1 *)(param_1 + 0x34);
  *puVar10 = uVar11;
  *(undefined4 **)(param_2 + 8) = puVar10 + 1;
  FUN_004779d0(1);
  puVar4 = *(undefined1 **)(param_2 + 8);
  *puVar4 = uVar2;
  uVar2 = *(undefined1 *)(param_1 + 0x35);
  *(undefined1 **)(param_2 + 8) = puVar4 + 1;
  FUN_004779d0(1);
  puVar4 = *(undefined1 **)(param_2 + 8);
  *puVar4 = uVar2;
  uVar2 = *(undefined1 *)(param_1 + 0x36);
  *(undefined1 **)(param_2 + 8) = puVar4 + 1;
  FUN_004779d0(1);
  puVar4 = *(undefined1 **)(param_2 + 8);
  uVar11 = *(undefined4 *)(param_1 + 0x40);
  *puVar4 = uVar2;
  *(undefined1 **)(param_2 + 8) = puVar4 + 1;
  FUN_004779d0(4);
  puVar10 = *(undefined4 **)(param_2 + 8);
  *puVar10 = uVar11;
  *(undefined4 **)(param_2 + 8) = puVar10 + 1;
  if ((*(uint *)(param_1 + 8) & 0x4000) != 0) {
    uVar3 = *(undefined2 *)(param_1 + 0x5c);
    FUN_004779d0(2);
    puVar5 = *(undefined2 **)(param_2 + 8);
    *puVar5 = uVar3;
    uVar3 = *(undefined2 *)(param_1 + 0x5e);
    *(undefined2 **)(param_2 + 8) = puVar5 + 1;
    FUN_004779d0(2);
    puVar5 = *(undefined2 **)(param_2 + 8);
    *puVar5 = uVar3;
    *(undefined2 **)(param_2 + 8) = puVar5 + 1;
  }
  uVar2 = *(undefined1 *)(param_1 + 0x37);
  FUN_004779d0(1);
  puVar4 = *(undefined1 **)(param_2 + 8);
  iVar8 = *(int *)(param_1 + 0x48);
  *puVar4 = uVar2;
  uVar2 = *(undefined1 *)(iVar8 + 0x1c);
  *(undefined1 **)(param_2 + 8) = puVar4 + 1;
  FUN_004779d0(1);
  puVar4 = *(undefined1 **)(param_2 + 8);
  iVar8 = 0;
  *puVar4 = uVar2;
  *(undefined1 **)(param_2 + 8) = puVar4 + 1;
  if (0 < *(short *)(*(int *)(param_1 + 0x48) + 0x1c)) {
    do {
      uVar9 = FUN_00477d10(iVar8);
      uVar11 = *(undefined4 *)(*(int *)(param_1 + 0xac) + iVar8 * 4);
      FUN_004779d0(4);
      puVar10 = *(undefined4 **)(param_2 + 8);
      *puVar10 = uVar11;
      *(undefined4 **)(param_2 + 8) = puVar10 + 1;
      FUN_004779d0(4);
      puVar6 = *(uint **)(param_2 + 8);
      *puVar6 = uVar9 | 0x80808080;
      *(uint **)(param_2 + 8) = puVar6 + 1;
      iVar8 = iVar8 + 1;
    } while (iVar8 < *(short *)(*(int *)(param_1 + 0x48) + 0x1c));
  }
  if ((*(byte *)(param_1 + 8) & 4) != 0) {
    uVar2 = *(undefined1 *)(param_1 + 0x88);
    FUN_004779d0(1);
    puVar4 = *(undefined1 **)(param_2 + 8);
    uVar11 = *(undefined4 *)(param_1 + 0x90);
    *puVar4 = uVar2;
    *(undefined1 **)(param_2 + 8) = puVar4 + 1;
    FUN_004779d0(4);
    puVar10 = *(undefined4 **)(param_2 + 8);
    *puVar10 = uVar11;
    uVar11 = *(undefined4 *)(param_1 + 0x94);
    *(undefined4 **)(param_2 + 8) = puVar10 + 1;
    FUN_004779d0(4);
    puVar10 = *(undefined4 **)(param_2 + 8);
    *puVar10 = uVar11;
    uVar11 = *(undefined4 *)(param_1 + 0x98);
    *(undefined4 **)(param_2 + 8) = puVar10 + 1;
    FUN_004779d0(4);
    puVar10 = *(undefined4 **)(param_2 + 8);
    uVar9 = (uint)*(byte *)(param_1 + 0x89);
    uVar14 = (uint)*(byte *)(param_1 + 0x8c);
    *puVar10 = uVar11;
    puVar10 = puVar10 + 1;
    *(undefined4 **)(param_2 + 8) = puVar10;
    uVar11 = CONCAT22((short)((uint)puVar10 >> 0x10),*(undefined2 *)(param_1 + 0x8a));
    uVar12 = CONCAT31((int3)((uint)uVar11 >> 8),*(undefined1 *)(param_1 + 0x8d));
    FUN_00477cd0(*(undefined1 *)(param_1 + 0x8e));
    FUN_00477cd0(uVar12);
    FUN_00477cd0(uVar14);
    FUN_00477cd0(uVar9);
    FUN_00477c90(uVar11);
  }
  return;
}


