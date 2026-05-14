// Decompiled methods and structure for class: cls_0x52b9f0

/*
/OOAnalyzer/cls_0x52b9f0
pack(disabled)
Structure cls_0x52b9f0 {
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   268   dword   4   mbr_0x10c   "Unsigned Double-Word (ddw, 4-bytes)"
   272   dword   4   mbr_0x110   "Unsigned Double-Word (ddw, 4-bytes)"
   276   dword   4   mbr_0x114   "Unsigned Double-Word (ddw, 4-bytes)"
   280   dword   4   mbr_0x118   "Unsigned Double-Word (ddw, 4-bytes)"
   284   dword   4   mbr_0x11c   "Unsigned Double-Word (ddw, 4-bytes)"
   288   dword   4   mbr_0x120   "Unsigned Double-Word (ddw, 4-bytes)"
   308   dword   4   mbr_0x134   "Unsigned Double-Word (ddw, 4-bytes)"
   312   dword   4   mbr_0x138   "Unsigned Double-Word (ddw, 4-bytes)"
   316   dword   4   mbr_0x13c   "Unsigned Double-Word (ddw, 4-bytes)"
   320   dword   4   mbr_0x140   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 324 Alignment: 1

*/

// Function at 0052b9f0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall OOAnalyzer::cls_0x52b9f0::meth_0x52b9f0(cls_0x52b9f0 *this)

{
  undefined *this_00;
  char cVar1;
  byte bVar2;
  cls_0x5a7b98_TCharacter *pcVar3;
  byte *pbVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 uVar8;
  undefined4 *puVar9;
  uint *puVar10;
  dword dVar11;
  int *piVar12;
  uint uVar13;
  uint uVar14;
  int **ppiVar15;
  int iVar16;
  byte *pbVar17;
  char *pcVar18;
  char *pcVar19;
  int *piVar20;
  bool bVar21;
  cls_0x5a5ff0 local_20;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  pcVar3 = DAT_00667fcc;
  iVar16 = _DAT_00666970;
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a18b2;
  local_c = ExceptionList;
  if (DAT_00667fcc == (cls_0x5a7b98_TCharacter *)0x0) {
    return;
  }
  ExceptionList = &local_c;
  this->mbr_0x114 = this->mbr_0x118;
  this->mbr_0x10c = this->mbr_0x110;
  this->mbr_0x118 = (int)this->mbr_0x120 >> 6;
  this->mbr_0x110 = (int)this->mbr_0x11c >> 6;
  this->mbr_0x134 = this->mbr_0x138;
  this->mbr_0x13c = this->mbr_0x140;
  cls_0x5a7b98_TCharacter::meth_0x46eb40(pcVar3,&this->mbr_0x138);
  pcVar3 = DAT_00667fcc;
  iVar6 = DAT_0065a784;
  if (this->mbr_0x60 == 0) {
    if ((((-1 < DAT_0065a784) && (iVar5 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4), iVar5 != 0)) &&
        (pbVar4 = &DAT_00667fcc->field_0x318, pbVar4 != (undefined *)0x0)) && (iVar5 != -0x58)) {
      pbVar17 = (byte *)(iVar5 + 0x58);
      do {
        bVar2 = *pbVar4;
        bVar21 = bVar2 < *pbVar17;
        if (bVar2 != *pbVar17) {
LAB_0052bae1:
          iVar5 = (1 - (uint)bVar21) - (uint)(bVar21 != 0);
          goto LAB_0052bae6;
        }
        if (bVar2 == 0) break;
        bVar2 = pbVar4[1];
        bVar21 = bVar2 < pbVar17[1];
        if (bVar2 != pbVar17[1]) goto LAB_0052bae1;
        pbVar4 = pbVar4 + 2;
        pbVar17 = pbVar17 + 2;
      } while (bVar2 != 0);
      iVar5 = 0;
LAB_0052bae6:
      if (iVar5 == 0) {
        this->mbr_0x60 = (dword)&DAT_00667fcc->field_0x314;
      }
    }
    if (this->mbr_0x60 == 0) {
      this->mbr_0x60 = (dword)&pcVar3->field_0x314;
      if (iVar6 < 0) {
        iVar6 = 0;
      }
      else {
        iVar6 = *(int *)(DAT_0065a77c + iVar6 * 4);
      }
      uVar13 = 0xffffffff;
      pcVar18 = (char *)(iVar6 + 0x58);
      do {
        pcVar19 = pcVar18;
        if (uVar13 == 0) break;
        uVar13 = uVar13 - 1;
        pcVar19 = pcVar18 + 1;
        cVar1 = *pcVar18;
        pcVar18 = pcVar19;
      } while (cVar1 != '\0');
      uVar13 = ~uVar13;
      puVar7 = (undefined4 *)(pcVar19 + -uVar13);
      puVar9 = (undefined4 *)&pcVar3->field_0x318;
      for (uVar14 = uVar13 >> 2; uVar14 != 0; uVar14 = uVar14 - 1) {
        *puVar9 = *puVar7;
        puVar7 = puVar7 + 1;
        puVar9 = puVar9 + 1;
      }
      for (uVar13 = uVar13 & 3; uVar13 != 0; uVar13 = uVar13 - 1) {
        *(undefined *)puVar9 = *(undefined *)puVar7;
        puVar7 = (undefined4 *)((int)puVar7 + 1);
        puVar9 = (undefined4 *)((int)puVar9 + 1);
      }
      this_00 = &pcVar3->field_0x338;
      iVar6 = 0;
      if (0 < *(int *)&pcVar3->field_0x338) {
        do {
          cls_0x45f7c0::meth_0x52c720((cls_0x45f7c0 *)this_00);
          iVar6 = iVar6 + 1;
        } while (iVar6 < *(int *)this_00);
      }
      *(undefined4 *)this_00 = 0;
      *(undefined4 *)&pcVar3->field_0x33c = 0;
    }
  }
  piVar12 = (int *)this->mbr_0x64;
  if (piVar12 != (int *)0x0) {
    if (iVar16 == *piVar12) goto LAB_0052bd31;
    if (piVar12 != (int *)0x0) {
      puVar7 = (undefined4 *)FUN_00482fb0(0xc);
      local_4 = 0;
      if (puVar7 != (undefined4 *)0x0) {
        puVar9 = (undefined4 *)this->mbr_0x64;
        *puVar7 = *puVar9;
        cls_0x5a5ff0::cls_0x5a5ff0(&local_20,0x2080);
        piVar12 = puVar7 + 1;
        local_4._0_1_ = 1;
        FUN_005295e0(puVar9 + 1,0x1040,piVar12,&local_20);
        uVar8 = FUN_00482ef0(*piVar12 << 1);
        iVar6 = *piVar12;
        puVar7[2] = uVar8;
        puVar9 = (undefined4 *)cls_0x5a5ff0::meth_0x49cdd0(&local_20);
        local_4 = (uint)local_4._1_3_ << 8;
        puVar7 = (undefined4 *)puVar7[2];
        for (uVar13 = (uint)(iVar6 << 1) >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
          *puVar7 = *puVar9;
          puVar9 = puVar9 + 1;
          puVar7 = puVar7 + 1;
        }
        for (uVar13 = iVar6 << 1 & 3; uVar13 != 0; uVar13 = uVar13 - 1) {
          *(undefined *)puVar7 = *(undefined *)puVar9;
          puVar9 = (undefined4 *)((int)puVar9 + 1);
          puVar7 = (undefined4 *)((int)puVar7 + 1);
        }
        cls_0x5a5ff0::~cls_0x5a5ff0(&local_20);
      }
      local_4 = 0xffffffff;
      puVar10 = (uint *)(this->mbr_0x60 + 0x24);
      uVar13 = 0;
      ppiVar15 = *(int ***)(this->mbr_0x60 + 0x34);
      while( true ) {
        if ((puVar10 == (uint *)0x0) || (*puVar10 <= uVar13)) goto LAB_0052bc77;
        if (**ppiVar15 == *(int *)this->mbr_0x64) break;
        ppiVar15 = ppiVar15 + 1;
        uVar13 = uVar13 + 1;
      }
      piVar12 = *ppiVar15;
      if (piVar12 != (int *)0x0) {
        *piVar12 = -1;
        if (piVar12[2] != 0) {
          FUN_00482f80(piVar12[2]);
        }
        piVar12[2] = 0;
        piVar12[1] = 0;
        FUN_004830f0(piVar12);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)(this->mbr_0x60 + 0x24));
LAB_0052bc77:
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)(this->mbr_0x60 + 0x24));
      FUN_004830f0(this->mbr_0x64);
      this->mbr_0x64 = 0;
    }
  }
  puVar10 = (uint *)(this->mbr_0x60 + 0x24);
  uVar13 = 0;
  ppiVar15 = *(int ***)(this->mbr_0x60 + 0x34);
  while ((puVar10 != (uint *)0x0 && (uVar13 < *puVar10))) {
    if (**ppiVar15 == iVar16) {
      iVar6 = FUN_00482fb0(0x2084);
      local_4 = 2;
      if (iVar6 == 0) {
        local_4 = 0xffffffff;
        this->mbr_0x64 = 0;
        goto LAB_0052bcf8;
      }
      dVar11 = FUN_00529220(*ppiVar15);
      ppiVar15 = ppiVar15 + 1;
      local_4 = 0xffffffff;
      this->mbr_0x64 = dVar11;
      uVar13 = uVar13 + 1;
    }
    else {
LAB_0052bcf8:
      ppiVar15 = ppiVar15 + 1;
      uVar13 = uVar13 + 1;
    }
  }
  if (this->mbr_0x64 == 0) {
    piVar12 = (int *)FUN_00482fb0(0x2084);
    if (piVar12 == (int *)0x0) {
      piVar12 = (int *)0x0;
    }
    else {
      *piVar12 = iVar16;
      piVar20 = piVar12;
      for (iVar16 = 0x820; piVar20 = piVar20 + 1, iVar16 != 0; iVar16 = iVar16 + -1) {
        *piVar20 = 0;
      }
    }
    this->mbr_0x64 = (dword)piVar12;
  }
LAB_0052bd31:
  if ((this->mbr_0x138 != this->mbr_0x134) || (this->mbr_0x140 != this->mbr_0x13c)) {
    iVar16 = -0x200;
    do {
      iVar6 = -0x200;
      do {
        FUN_005293f0(this->mbr_0x138 + iVar16,this->mbr_0x140 + iVar6,1);
        iVar6 = iVar6 + 0x80;
      } while (iVar6 < 0x201);
      iVar16 = iVar16 + 0x80;
    } while (iVar16 < 0x201);
  }
  ExceptionList = local_c;
  return;
}



