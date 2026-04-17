// Decompiled methods and structure for class: cls_0x52c3d0

/*
/OOAnalyzer/cls_0x52c3d0
pack(disabled)
Structure cls_0x52c3d0 {
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 104 Alignment: 1

*/

// Function at 0052c3d0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x52c3d0::meth_0x52c3d0(cls_0x52c3d0 *this)

{
  cls_0x45f7c0 *this_00;
  char cVar1;
  byte bVar2;
  byte *pbVar3;
  int iVar4;
  int iVar5;
  dword dVar6;
  int *piVar7;
  uint uVar8;
  uint uVar9;
  byte *pbVar10;
  int **ppiVar11;
  char *pcVar12;
  char *pcVar13;
  undefined4 *puVar14;
  undefined4 *puVar15;
  int iVar16;
  uint *puVar17;
  int *piVar18;
  bool bVar19;
  int in_stack_00000004;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a18c7;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  this->mbr_0x60 = 0;
  if (in_stack_00000004 != 0) {
    if (this->mbr_0x64 != 0) {
      FUN_004830f0(this->mbr_0x64);
      this->mbr_0x64 = 0;
    }
    iVar5 = DAT_00667fcc;
    iVar16 = DAT_0065a784;
    if ((((-1 < DAT_0065a784) && (iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4), iVar4 != 0)) &&
        (pbVar3 = (byte *)(DAT_00667fcc + 0x318), pbVar3 != (byte *)0x0)) && (iVar4 != -0x58)) {
      pbVar10 = (byte *)(iVar4 + 0x58);
      do {
        bVar2 = *pbVar3;
        bVar19 = bVar2 < *pbVar10;
        if (bVar2 != *pbVar10) {
LAB_0052c46a:
          iVar4 = (1 - (uint)bVar19) - (uint)(bVar19 != 0);
          goto LAB_0052c471;
        }
        if (bVar2 == 0) break;
        bVar2 = pbVar3[1];
        bVar19 = bVar2 < pbVar10[1];
        if (bVar2 != pbVar10[1]) goto LAB_0052c46a;
        pbVar3 = pbVar3 + 2;
        pbVar10 = pbVar10 + 2;
      } while (bVar2 != 0);
      iVar4 = 0;
LAB_0052c471:
      if (iVar4 == 0) {
        this->mbr_0x60 = DAT_00667fcc + 0x314;
      }
    }
    if (this->mbr_0x60 == 0) {
      this->mbr_0x60 = iVar5 + 0x314;
      if ((iVar16 < 0) || (iVar16 = *(int *)(DAT_0065a77c + iVar16 * 4), iVar16 == 0)) {
        uVar8 = 0xffffffff;
        pcVar12 = s_Revenant_005e3794;
        do {
          pcVar13 = pcVar12;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pcVar13 = pcVar12 + 1;
          cVar1 = *pcVar12;
          pcVar12 = pcVar13;
        } while (cVar1 != '\0');
        uVar8 = ~uVar8;
        puVar14 = (undefined4 *)(pcVar13 + -uVar8);
      }
      else {
        uVar8 = 0xffffffff;
        pcVar12 = (char *)(iVar16 + 0x58);
        do {
          pcVar13 = pcVar12;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pcVar13 = pcVar12 + 1;
          cVar1 = *pcVar12;
          pcVar12 = pcVar13;
        } while (cVar1 != '\0');
        uVar8 = ~uVar8;
        puVar14 = (undefined4 *)(pcVar13 + -uVar8);
      }
      puVar15 = (undefined4 *)(iVar5 + 0x318);
      for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
        *puVar15 = *puVar14;
        puVar14 = puVar14 + 1;
        puVar15 = puVar15 + 1;
      }
      for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
        *(undefined *)puVar15 = *(undefined *)puVar14;
        puVar14 = (undefined4 *)((int)puVar14 + 1);
        puVar15 = (undefined4 *)((int)puVar15 + 1);
      }
      this_00 = (cls_0x45f7c0 *)(iVar5 + 0x338);
      iVar16 = 0;
      if (0 < *(int *)(iVar5 + 0x338)) {
        do {
          cls_0x45f7c0::meth_0x52c720(this_00);
          iVar16 = iVar16 + 1;
        } while (iVar16 < (int)(this_00->cls_0x41c7f0).mbr_0x0);
      }
      (this_00->cls_0x41c7f0).mbr_0x0 = 0;
      *(undefined4 *)(iVar5 + 0x33c) = 0;
    }
    iVar16 = _DAT_00666970;
    puVar17 = (uint *)(this->mbr_0x60 + 0x24);
    ppiVar11 = *(int ***)(this->mbr_0x60 + 0x34);
    for (uVar8 = 0; (puVar17 != (uint *)0x0 && (uVar8 < *puVar17)); uVar8 = uVar8 + 1) {
      if (**ppiVar11 == iVar16) {
        iVar5 = FUN_00482fb0(0x2084);
        local_4 = 0;
        if (iVar5 == 0) {
          dVar6 = 0;
        }
        else {
          dVar6 = FUN_00529220(*ppiVar11);
        }
        local_4 = 0xffffffff;
        this->mbr_0x64 = dVar6;
      }
      ppiVar11 = ppiVar11 + 1;
    }
    if (this->mbr_0x64 == 0) {
      piVar7 = (int *)FUN_00482fb0(0x2084);
      if (piVar7 == (int *)0x0) {
        piVar7 = (int *)0x0;
      }
      else {
        *piVar7 = iVar16;
        piVar18 = piVar7;
        for (iVar16 = 0x820; piVar18 = piVar18 + 1, iVar16 != 0; iVar16 = iVar16 + -1) {
          *piVar18 = 0;
        }
      }
      this->mbr_0x64 = (dword)piVar7;
    }
  }
  ExceptionList = local_c;
  return;
}



