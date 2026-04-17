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
  int iVar3;
  byte *pbVar4;
  int iVar5;
  int *piVar6;
  uint uVar7;
  uint uVar8;
  byte *pbVar9;
  int **ppiVar10;
  char *pcVar11;
  char *pcVar12;
  undefined4 *puVar13;
  undefined4 *puVar14;
  int iVar15;
  uint *puVar16;
  int *piVar17;
  bool bVar18;
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
    if ((LPCVOID)this->mbr_0x64 != (LPCVOID)0x0) {
      FUN_004830f0((LPCVOID)this->mbr_0x64);
      this->mbr_0x64 = 0;
    }
    iVar3 = DAT_00667fcc;
    iVar15 = DAT_0065a784;
    if ((((-1 < DAT_0065a784) && (iVar5 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4), iVar5 != 0)) &&
        (pbVar4 = (byte *)(DAT_00667fcc + 0x318), pbVar4 != (byte *)0x0)) && (iVar5 != -0x58)) {
      pbVar9 = (byte *)(iVar5 + 0x58);
      do {
        bVar2 = *pbVar4;
        bVar18 = bVar2 < *pbVar9;
        if (bVar2 != *pbVar9) {
LAB_0052c46a:
          iVar5 = (1 - (uint)bVar18) - (uint)(bVar18 != 0);
          goto LAB_0052c471;
        }
        if (bVar2 == 0) break;
        bVar2 = pbVar4[1];
        bVar18 = bVar2 < pbVar9[1];
        if (bVar2 != pbVar9[1]) goto LAB_0052c46a;
        pbVar4 = pbVar4 + 2;
        pbVar9 = pbVar9 + 2;
      } while (bVar2 != 0);
      iVar5 = 0;
LAB_0052c471:
      if (iVar5 == 0) {
        this->mbr_0x60 = DAT_00667fcc + 0x314;
      }
    }
    if (this->mbr_0x60 == 0) {
      this->mbr_0x60 = iVar3 + 0x314;
      if ((iVar15 < 0) || (iVar15 = *(int *)(DAT_0065a77c + iVar15 * 4), iVar15 == 0)) {
        uVar7 = 0xffffffff;
        pcVar11 = s_Revenant_005e3794;
        do {
          pcVar12 = pcVar11;
          if (uVar7 == 0) break;
          uVar7 = uVar7 - 1;
          pcVar12 = pcVar11 + 1;
          cVar1 = *pcVar11;
          pcVar11 = pcVar12;
        } while (cVar1 != '\0');
        uVar7 = ~uVar7;
        puVar13 = (undefined4 *)(pcVar12 + -uVar7);
      }
      else {
        uVar7 = 0xffffffff;
        pcVar11 = (char *)(iVar15 + 0x58);
        do {
          pcVar12 = pcVar11;
          if (uVar7 == 0) break;
          uVar7 = uVar7 - 1;
          pcVar12 = pcVar11 + 1;
          cVar1 = *pcVar11;
          pcVar11 = pcVar12;
        } while (cVar1 != '\0');
        uVar7 = ~uVar7;
        puVar13 = (undefined4 *)(pcVar12 + -uVar7);
      }
      puVar14 = (undefined4 *)(iVar3 + 0x318);
      for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
        *puVar14 = *puVar13;
        puVar13 = puVar13 + 1;
        puVar14 = puVar14 + 1;
      }
      for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
        *(undefined *)puVar14 = *(undefined *)puVar13;
        puVar13 = (undefined4 *)((int)puVar13 + 1);
        puVar14 = (undefined4 *)((int)puVar14 + 1);
      }
      this_00 = (cls_0x45f7c0 *)(iVar3 + 0x338);
      iVar15 = 0;
      if (0 < *(int *)(iVar3 + 0x338)) {
        do {
          cls_0x45f7c0::meth_0x52c720(this_00);
          iVar15 = iVar15 + 1;
        } while (iVar15 < (int)(this_00->cls_0x41c7f0).mbr_0x0);
      }
      (this_00->cls_0x41c7f0).mbr_0x0 = 0;
      *(undefined4 *)(iVar3 + 0x33c) = 0;
    }
    iVar15 = _DAT_00666970;
    puVar16 = (uint *)(this->mbr_0x60 + 0x24);
    ppiVar10 = *(int ***)(this->mbr_0x60 + 0x34);
    for (uVar7 = 0; (puVar16 != (uint *)0x0 && (uVar7 < *puVar16)); uVar7 = uVar7 + 1) {
      if (**ppiVar10 == iVar15) {
        puVar13 = FUN_00482fb0(0x2084);
        local_4 = 0;
        if (puVar13 == (undefined4 *)0x0) {
          puVar13 = (undefined4 *)0x0;
        }
        else {
          puVar13 = FUN_00529220(puVar13,*ppiVar10);
        }
        local_4 = 0xffffffff;
        this->mbr_0x64 = (dword)puVar13;
      }
      ppiVar10 = ppiVar10 + 1;
    }
    if (this->mbr_0x64 == 0) {
      piVar6 = FUN_00482fb0(0x2084);
      if (piVar6 == (int *)0x0) {
        piVar6 = (int *)0x0;
      }
      else {
        *piVar6 = iVar15;
        piVar17 = piVar6;
        for (iVar15 = 0x820; piVar17 = piVar17 + 1, iVar15 != 0; iVar15 = iVar15 + -1) {
          *piVar17 = 0;
        }
      }
      this->mbr_0x64 = (dword)piVar6;
    }
  }
  ExceptionList = local_c;
  return;
}



