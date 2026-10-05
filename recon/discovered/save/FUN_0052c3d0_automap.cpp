// FUN_0052c3d0 @ 0052c3d0 size=481

void __thiscall FUN_0052c3d0(int param_1,int param_2)

{
  char cVar1;
  byte bVar2;
  byte *pbVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  int *piVar7;
  uint uVar8;
  uint uVar9;
  byte *pbVar10;
  undefined4 *puVar11;
  char *pcVar12;
  char *pcVar13;
  int iVar14;
  uint *puVar15;
  int *piVar16;
  bool bVar17;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a18c7;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  *(undefined4 *)(param_1 + 0x60) = 0;
  if (param_2 != 0) {
    if (*(int *)(param_1 + 100) != 0) {
      FUN_004830f0(*(int *)(param_1 + 100));
      *(undefined4 *)(param_1 + 100) = 0;
    }
    iVar5 = DAT_00667fcc;
    iVar14 = DAT_0065a784;
    if ((((-1 < DAT_0065a784) && (iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4), iVar4 != 0)) &&
        (pbVar3 = (byte *)(DAT_00667fcc + 0x318), pbVar3 != (byte *)0x0)) && (iVar4 != -0x58)) {
      pbVar10 = (byte *)(iVar4 + 0x58);
      do {
        bVar2 = *pbVar3;
        bVar17 = bVar2 < *pbVar10;
        if (bVar2 != *pbVar10) {
LAB_0052c46a:
          iVar4 = (1 - (uint)bVar17) - (uint)(bVar17 != 0);
          goto LAB_0052c471;
        }
        if (bVar2 == 0) break;
        bVar2 = pbVar3[1];
        bVar17 = bVar2 < pbVar10[1];
        if (bVar2 != pbVar10[1]) goto LAB_0052c46a;
        pbVar3 = pbVar3 + 2;
        pbVar10 = pbVar10 + 2;
      } while (bVar2 != 0);
      iVar4 = 0;
LAB_0052c471:
      if (iVar4 == 0) {
        *(int *)(param_1 + 0x60) = DAT_00667fcc + 0x314;
      }
    }
    if (*(int *)(param_1 + 0x60) == 0) {
      *(int *)(param_1 + 0x60) = iVar5 + 0x314;
      if ((iVar14 < 0) || (iVar14 = *(int *)(DAT_0065a77c + iVar14 * 4), iVar14 == 0)) {
        uVar8 = 0xffffffff;
        pcVar13 = s_Revenant_005e3794;
        do {
          pcVar12 = pcVar13;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pcVar12 = pcVar13 + 1;
          cVar1 = *pcVar13;
          pcVar13 = pcVar12;
        } while (cVar1 != '\0');
        uVar8 = ~uVar8;
        pcVar12 = pcVar12 + -uVar8;
      }
      else {
        uVar8 = 0xffffffff;
        pcVar13 = (char *)(iVar14 + 0x58);
        do {
          pcVar12 = pcVar13;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pcVar12 = pcVar13 + 1;
          cVar1 = *pcVar13;
          pcVar13 = pcVar12;
        } while (cVar1 != '\0');
        uVar8 = ~uVar8;
        pcVar12 = pcVar12 + -uVar8;
      }
      pcVar13 = (char *)(iVar5 + 0x318);
      for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
        *(undefined4 *)pcVar13 = *(undefined4 *)pcVar12;
        pcVar12 = pcVar12 + 4;
        pcVar13 = pcVar13 + 4;
      }
      for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
        *pcVar13 = *pcVar12;
        pcVar12 = pcVar12 + 1;
        pcVar13 = pcVar13 + 1;
      }
      iVar14 = 0;
      if (0 < *(int *)(iVar5 + 0x338)) {
        do {
          FUN_0052c720(iVar14);
          iVar14 = iVar14 + 1;
        } while (iVar14 < *(int *)(iVar5 + 0x338));
      }
      *(int *)(iVar5 + 0x338) = 0;
      *(undefined4 *)(iVar5 + 0x33c) = 0;
    }
    iVar14 = DAT_00666970;
    puVar15 = (uint *)(*(int *)(param_1 + 0x60) + 0x24);
    puVar11 = *(undefined4 **)(*(int *)(param_1 + 0x60) + 0x34);
    for (uVar8 = 0; (puVar15 != (uint *)0x0 && (uVar8 < *puVar15)); uVar8 = uVar8 + 1) {
      if (*(int *)*puVar11 == iVar14) {
        iVar5 = FUN_00482fb0(0x2084);
        local_4 = 0;
        if (iVar5 == 0) {
          uVar6 = 0;
        }
        else {
          uVar6 = FUN_00529220(*puVar11);
        }
        local_4 = 0xffffffff;
        *(undefined4 *)(param_1 + 100) = uVar6;
      }
      puVar11 = puVar11 + 1;
    }
    if (*(int *)(param_1 + 100) == 0) {
      piVar7 = (int *)FUN_00482fb0(0x2084);
      if (piVar7 == (int *)0x0) {
        piVar7 = (int *)0x0;
      }
      else {
        *piVar7 = iVar14;
        piVar16 = piVar7;
        for (iVar14 = 0x820; piVar16 = piVar16 + 1, iVar14 != 0; iVar14 = iVar14 + -1) {
          *piVar16 = 0;
        }
      }
      *(int **)(param_1 + 100) = piVar7;
    }
  }
  ExceptionList = local_c;
  return;
}


