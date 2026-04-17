#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x52b9f0



// Function at 0052b9f0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void cls_0x52b9f0::meth_0x52b9f0()

{
  undefined *this_00;
  char cVar1;
  byte bVar2;
  TPlayScreen *pcVar3;
  byte *pbVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  uint *puVar9;
  int *piVar10;
  uint uVar11;
  uint uVar12;
  int **ppiVar13;
  int iVar14;
  byte *pbVar15;
  char *pcVar16;
  char *pcVar17;
  int *piVar18;
  bool bVar19;
  cls_0x5a5ff0 local_20;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  pcVar3 = DAT_00667fcc;
  iVar14 = _DAT_00666970;
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a18b2;
  local_c = ExceptionList;
  if (DAT_00667fcc == (TPlayScreen *)0x0) {
    return;
  }
  ExceptionList = &local_c;
  this->mbr_0x114 = this->mbr_0x118;
  this->mbr_0x10c = this->mbr_0x110;
  this->mbr_0x118 = (int)this->mbr_0x120 >> 6;
  this->mbr_0x110 = (int)this->mbr_0x11c >> 6;
  this->mbr_0x134 = this->mbr_0x138;
  this->mbr_0x13c = this->mbr_0x140;
  TPlayScreen::meth_0x46eb40(pcVar3,&this->mbr_0x138);
  pcVar3 = DAT_00667fcc;
  iVar6 = DAT_0065a784;
  if (this->mbr_0x60 == 0) {
    if ((((-1 < DAT_0065a784) && (iVar5 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4), iVar5 != 0)) &&
        (pbVar4 = &DAT_00667fcc->field_0x318, pbVar4 != (undefined *)0x0)) && (iVar5 != -0x58)) {
      pbVar15 = (byte *)(iVar5 + 0x58);
      do {
        bVar2 = *pbVar4;
        bVar19 = bVar2 < *pbVar15;
        if (bVar2 != *pbVar15) {
LAB_0052bae1:
          iVar5 = (1 - (uint)bVar19) - (uint)(bVar19 != 0);
          goto LAB_0052bae6;
        }
        if (bVar2 == 0) break;
        bVar2 = pbVar4[1];
        bVar19 = bVar2 < pbVar15[1];
        if (bVar2 != pbVar15[1]) goto LAB_0052bae1;
        pbVar4 = pbVar4 + 2;
        pbVar15 = pbVar15 + 2;
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
      uVar11 = 0xffffffff;
      pcVar16 = (char *)(iVar6 + 0x58);
      do {
        pcVar17 = pcVar16;
        if (uVar11 == 0) break;
        uVar11 = uVar11 - 1;
        pcVar17 = pcVar16 + 1;
        cVar1 = *pcVar16;
        pcVar16 = pcVar17;
      } while (cVar1 != '\0');
      uVar11 = ~uVar11;
      puVar7 = (undefined4 *)(pcVar17 + -uVar11);
      puVar8 = (undefined4 *)&pcVar3->field_0x318;
      for (uVar12 = uVar11 >> 2; uVar12 != 0; uVar12 = uVar12 - 1) {
        *puVar8 = *puVar7;
        puVar7 = puVar7 + 1;
        puVar8 = puVar8 + 1;
      }
      for (uVar11 = uVar11 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
        *(undefined *)puVar8 = *(undefined *)puVar7;
        puVar7 = (undefined4 *)((int)puVar7 + 1);
        puVar8 = (undefined4 *)((int)puVar8 + 1);
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
  piVar10 = (int *)this->mbr_0x64;
  if (piVar10 != (int *)0x0) {
    if (iVar14 == *piVar10) goto LAB_0052bd31;
    if (piVar10 != (int *)0x0) {
      puVar7 = FUN_00482fb0(0xc);
      local_4 = 0;
      if (puVar7 != (undefined4 *)0x0) {
        puVar8 = (undefined4 *)this->mbr_0x64;
        *puVar7 = *puVar8;
        cls_0x5a5ff0::cls_0x5a5ff0(&local_20,0x2080);
        piVar10 = puVar7 + 1;
        local_4._0_1_ = 1;
        FUN_005295e0((short *)(puVar8 + 1),0x1040,piVar10,&local_20);
        puVar8 = FUN_00482ef0(*piVar10 << 1);
        iVar6 = *piVar10;
        puVar7[2] = puVar8;
        puVar8 = (undefined4 *)cls_0x5a5ff0::meth_0x49cdd0(&local_20);
        local_4 = (uint)local_4._1_3_ << 8;
        puVar7 = (undefined4 *)puVar7[2];
        for (uVar11 = (uint)(iVar6 << 1) >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
          *puVar7 = *puVar8;
          puVar8 = puVar8 + 1;
          puVar7 = puVar7 + 1;
        }
        for (uVar11 = iVar6 << 1 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
          *(undefined *)puVar7 = *(undefined *)puVar8;
          puVar8 = (undefined4 *)((int)puVar8 + 1);
          puVar7 = (undefined4 *)((int)puVar7 + 1);
        }
        cls_0x5a5ff0::~cls_0x5a5ff0(&local_20);
      }
      local_4 = 0xffffffff;
      puVar9 = (uint *)(this->mbr_0x60 + 0x24);
      uVar11 = 0;
      ppiVar13 = *(int ***)(this->mbr_0x60 + 0x34);
      while( true ) {
        if ((puVar9 == (uint *)0x0) || (*puVar9 <= uVar11)) goto LAB_0052bc77;
        if (**ppiVar13 == *(int *)this->mbr_0x64) break;
        ppiVar13 = ppiVar13 + 1;
        uVar11 = uVar11 + 1;
      }
      piVar10 = *ppiVar13;
      if (piVar10 != (int *)0x0) {
        *piVar10 = -1;
        if ((LPCVOID)piVar10[2] != (LPCVOID)0x0) {
          FUN_00482f80((LPCVOID)piVar10[2]);
        }
        piVar10[2] = 0;
        piVar10[1] = 0;
        FUN_004830f0(piVar10);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)(this->mbr_0x60 + 0x24));
LAB_0052bc77:
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)(this->mbr_0x60 + 0x24));
      FUN_004830f0((LPCVOID)this->mbr_0x64);
      this->mbr_0x64 = 0;
    }
  }
  puVar9 = (uint *)(this->mbr_0x60 + 0x24);
  uVar11 = 0;
  ppiVar13 = *(int ***)(this->mbr_0x60 + 0x34);
  while ((puVar9 != (uint *)0x0 && (uVar11 < *puVar9))) {
    if (**ppiVar13 == iVar14) {
      puVar7 = FUN_00482fb0(0x2084);
      local_4 = 2;
      if (puVar7 == (undefined4 *)0x0) {
        local_4 = 0xffffffff;
        this->mbr_0x64 = 0;
        goto LAB_0052bcf8;
      }
      puVar7 = FUN_00529220(puVar7,*ppiVar13);
      ppiVar13 = ppiVar13 + 1;
      local_4 = 0xffffffff;
      this->mbr_0x64 = (dword)puVar7;
      uVar11 = uVar11 + 1;
    }
    else {
LAB_0052bcf8:
      ppiVar13 = ppiVar13 + 1;
      uVar11 = uVar11 + 1;
    }
  }
  if (this->mbr_0x64 == 0) {
    piVar10 = FUN_00482fb0(0x2084);
    if (piVar10 == (int *)0x0) {
      piVar10 = (int *)0x0;
    }
    else {
      *piVar10 = iVar14;
      piVar18 = piVar10;
      for (iVar14 = 0x820; piVar18 = piVar18 + 1, iVar14 != 0; iVar14 = iVar14 + -1) {
        *piVar18 = 0;
      }
    }
    this->mbr_0x64 = (dword)piVar10;
  }
LAB_0052bd31:
  if ((this->mbr_0x138 != this->mbr_0x134) || (this->mbr_0x140 != this->mbr_0x13c)) {
    iVar14 = -0x200;
    do {
      iVar6 = -0x200;
      do {
        FUN_005293f0(this->mbr_0x64,this->mbr_0x138 + iVar14,this->mbr_0x140 + iVar6,1);
        iVar6 = iVar6 + 0x80;
      } while (iVar6 < 0x201);
      iVar14 = iVar14 + 0x80;
    } while (iVar14 < 0x201);
  }
  ExceptionList = local_c;
  return;
}



