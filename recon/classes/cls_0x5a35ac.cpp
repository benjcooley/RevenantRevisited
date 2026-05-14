// REVSYNC: candidate=T3DImagery confidence=MEDIUM evidence=src-path(3DImage.cpp x11) src-file=src/3dimage.cpp date=2026-04-17 note="3dimage.cpp has multiple classes; filename guess plausible"
// Decompiled methods and structure for class: cls_0x5a35ac

/*
/OOAnalyzer/cls_0x5a35ac
pack(disabled)
Structure cls_0x5a35ac {
   0   cls_0x5a486c   196   cls_0x5a486c   "Component (member) class."
}
Length: 196 Alignment: 1

*/

// Function at 00406580

cls_0x5a35ac * __thiscall OOAnalyzer::cls_0x5a35ac::cls_0x5a35ac(cls_0x5a35ac *this)

{
  dword *pdVar1;
  dword *this_00;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined4 in_stack_00000004;
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059c584;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  cls_0x5a486c::cls_0x5a486c(&this->cls_0x5a486c,in_stack_00000004);
  pdVar1 = &(this->cls_0x5a486c).mbr_0x34;
  local_4 = 0;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)pdVar1,4);
  local_4._0_1_ = 1;
  puVar3 = (undefined4 *)FUN_00482fb0(0x5c);
  (this->cls_0x5a486c).mbr_0x48 = (dword)puVar3;
  for (iVar4 = 0x17; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  this_00 = &(this->cls_0x5a486c).mbr_0x4c;
  local_4._0_1_ = 2;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)this_00,4);
  local_4._0_1_ = 3;
  puVar3 = (undefined4 *)FUN_00482fb0(0x98);
  (this->cls_0x5a486c).mbr_0x60 = (dword)puVar3;
  for (iVar4 = 0x26; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  local_4._0_1_ = 4;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x5a486c).mbr_0x64,0x10);
  local_4._0_1_ = 5;
  puVar3 = (undefined4 *)FUN_00482fb0(0x4c);
  (this->cls_0x5a486c).mbr_0x78 = (dword)puVar3;
  for (iVar4 = 0x13; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  local_4._0_1_ = 6;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x5a486c).mbr_0x7c,0x10);
  local_4._0_1_ = 7;
  puVar3 = (undefined4 *)FUN_00482fb0(0x10);
  if (puVar3 == (undefined4 *)0x0) {
    puVar3 = (undefined4 *)0x0;
  }
  else {
    puVar3[1] = 0;
    *puVar3 = 0;
    puVar3[3] = 0;
    puVar3[2] = 0;
  }
  (this->cls_0x5a486c).mbr_0x90 = (dword)puVar3;
  *puVar3 = 0;
  puVar3[1] = 0;
  puVar3[2] = 0;
  puVar3[3] = 0;
  (this->cls_0x5a486c).vftptr_0x0 =
       (cls_0x5a486c__vftable_5a486c *)&cls_0x5a35ac__vftable_5a35ac_005a35ac;
  iVar4 = 0;
  local_4 = CONCAT31(local_4._1_3_,8);
  (this->cls_0x5a486c).mbr_0xc = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x14 = 0;
  (this->cls_0x5a486c).mbr_0x24 = 0;
  (this->cls_0x5a486c).mbr_0x28 = 0;
  if (0 < (int)*pdVar1) {
    do {
      if ((-1 < iVar4) && (iVar2 = *(int *)((this->cls_0x5a486c).mbr_0x44 + iVar4 * 4), iVar2 != 0))
      {
        FUN_004830f0(iVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  iVar4 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x38 = 0;
  if (0 < (int)*this_00) {
    do {
      if ((-1 < iVar4) && (iVar2 = *(int *)((this->cls_0x5a486c).mbr_0x5c + iVar4 * 4), iVar2 != 0))
      {
        FUN_004830f0(iVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*this_00);
  }
  pdVar1 = &(this->cls_0x5a486c).mbr_0x64;
  *this_00 = 0;
  iVar4 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x50 = 0;
  if (0 < (int)*pdVar1) {
    do {
      if ((-1 < iVar4) && (iVar2 = *(int *)((this->cls_0x5a486c).mbr_0x74 + iVar4 * 4), iVar2 != 0))
      {
        FUN_004830f0(iVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x68 = 0;
  pdVar1 = &(this->cls_0x5a486c).mbr_0x7c;
  iVar4 = 0;
  if (0 < (int)(this->cls_0x5a486c).mbr_0x7c) {
    do {
      if ((-1 < iVar4) && (*(int *)((this->cls_0x5a486c).mbr_0x8c + iVar4 * 4) != 0)) {
        FUN_00411240(1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x80 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x98 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x9c = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0xa0 = 0;
  ExceptionList = local_c;
  return this;
}



// Function at 004067d0

cls_0x5a35ac * __thiscall OOAnalyzer::cls_0x5a35ac::~cls_0x5a35ac(cls_0x5a35ac *this)

{
  dword *pdVar1;
  dword dVar2;
  int iVar3;
  cls_0x5a35ac *pcVar4;
  int iVar5;
  void *pvStack_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  puStack_8 = &LAB_0059c5e2;
  pvStack_c = ExceptionList;
  ExceptionList = &pvStack_c;
  (this->cls_0x5a486c).vftptr_0x0 =
       (cls_0x5a486c__vftable_5a486c *)&cls_0x5a35ac__vftable_5a35ac_005a35ac;
  local_4 = 4;
  meth_0x4085f0(this);
  pdVar1 = &(this->cls_0x5a486c).mbr_0x7c;
  iVar5 = 0;
  local_4 = CONCAT31(local_4._1_3_,5);
  if (0 < (int)*pdVar1) {
    do {
      cls_0x41c7f0::meth_0x411330((cls_0x41c7f0 *)pdVar1);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)*pdVar1);
  }
  dVar2 = (this->cls_0x5a486c).mbr_0x90;
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x80 = 0;
  if (dVar2 != 0) {
    FUN_004109a0();
    FUN_004830f0(dVar2);
  }
  local_4._0_1_ = 3;
  FUN_004830f0((this->cls_0x5a486c).mbr_0x8c);
  pdVar1 = &(this->cls_0x5a486c).mbr_0x64;
  iVar5 = 0;
  local_4._0_1_ = 6;
  if (0 < (int)*pdVar1) {
    do {
      if ((-1 < iVar5) && (iVar3 = *(int *)((this->cls_0x5a486c).mbr_0x74 + iVar5 * 4), iVar3 != 0))
      {
        FUN_004830f0(iVar3);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x68 = 0;
  FUN_004830f0((this->cls_0x5a486c).mbr_0x78);
  local_4._0_1_ = 2;
  FUN_004830f0((this->cls_0x5a486c).mbr_0x74);
  pdVar1 = &(this->cls_0x5a486c).mbr_0x4c;
  iVar5 = 0;
  local_4._0_1_ = 7;
  if (0 < (int)*pdVar1) {
    do {
      if ((-1 < iVar5) && (iVar3 = *(int *)((this->cls_0x5a486c).mbr_0x5c + iVar5 * 4), iVar3 != 0))
      {
        FUN_004830f0(iVar3);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x50 = 0;
  FUN_004830f0((this->cls_0x5a486c).mbr_0x60);
  local_4._0_1_ = 1;
  FUN_004830f0((this->cls_0x5a486c).mbr_0x5c);
  pdVar1 = &(this->cls_0x5a486c).mbr_0x34;
  iVar5 = 0;
  local_4._0_1_ = 8;
  if (0 < (int)*pdVar1) {
    do {
      if ((-1 < iVar5) && (iVar3 = *(int *)((this->cls_0x5a486c).mbr_0x44 + iVar5 * 4), iVar3 != 0))
      {
        FUN_004830f0(iVar3);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar5 = iVar5 + 1;
    } while (iVar5 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x38 = 0;
  FUN_004830f0((this->cls_0x5a486c).mbr_0x48);
  local_4 = (uint)local_4._1_3_ << 8;
  FUN_004830f0((this->cls_0x5a486c).mbr_0x44);
  local_4 = 0xffffffff;
  pcVar4 = (cls_0x5a35ac *)meth_0x446260(this);
  ExceptionList = pvStack_c;
  return pcVar4;
}



// Function at 00406a50

undefined4 __thiscall OOAnalyzer::cls_0x5a35ac::meth_0x406a50(cls_0x5a35ac *this)

{
  dword *pdVar1;
  cls_0x5a486c__vftable_5a486c *pcVar2;
  undefined uVar3;
  undefined3 extraout_var;
  dword dVar4;
  undefined3 extraout_var_00;
  undefined4 uVar5;
  undefined4 *puVar6;
  undefined3 extraout_var_01;
  int3 extraout_var_02;
  undefined3 extraout_var_03;
  int3 extraout_var_04;
  undefined3 extraout_var_05;
  undefined3 extraout_var_06;
  undefined3 extraout_var_07;
  int3 extraout_var_08;
  undefined3 extraout_var_09;
  undefined3 extraout_var_10;
  undefined3 extraout_var_11;
  undefined3 extraout_var_12;
  undefined3 extraout_var_13;
  uint *puVar7;
  undefined3 extraout_var_14;
  undefined3 extraout_var_15;
  int3 extraout_var_16;
  undefined3 extraout_var_17;
  int3 extraout_var_18;
  undefined3 extraout_var_19;
  undefined3 extraout_var_20;
  undefined4 *puVar8;
  uint *puVar9;
  int3 extraout_var_21;
  undefined3 extraout_var_22;
  undefined3 extraout_var_23;
  undefined *puVar10;
  undefined3 extraout_var_24;
  byte *pbVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  int iVar16;
  uint *puVar17;
  int *piVar18;
  int unaff_EDI;
  uint *puVar19;
  undefined4 *puVar20;
  bool bVar21;
  uint *in_stack_00000004;
  int iStack_138;
  uint *local_134;
  uint *puStack_130;
  int iStack_12c;
  uint *puStack_128;
  uint *apuStack_120 [2];
  int iStack_118;
  uint auStack_114 [8];
  int iStack_f4;
  uint uStack_f0;
  uint uStack_ec;
  int iStack_e8;
  uint uStack_e4;
  int iStack_e0;
  int iStack_dc;
  int *piStack_d8;
  int iStack_d4;
  int *piStack_d0;
  undefined auStack_c4 [44];
  undefined4 auStack_98 [38];
  
  if ((this->cls_0x5a486c).mbr_0xc != 0) {
    return 1;
  }
  iVar15 = 1;
  (this->cls_0x5a486c).mbr_0xc = 1;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x20 = 0;
  uVar12 = *in_stack_00000004;
  uVar14 = in_stack_00000004[1];
  *(uint *)&(this->cls_0x5a486c).mbr_0x1c = uVar12;
  (this->cls_0x5a486c).mbr_0x24 = uVar14;
  if ((uVar12 & 1) != 0) {
    uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    iVar15 = CONCAT31(extraout_var,uVar3);
  }
  dVar4 = FUN_00482fb0(iVar15 * 4);
  (this->cls_0x5a486c).mbr_0x28 = dVar4;
  iStack_138 = 0;
  if (0 < iVar15) {
    puVar17 = in_stack_00000004 + 2;
    do {
      if (((this->cls_0x5a486c).mbr_0x1c & 1) == 0) {
        iStack_12c = 1;
        puStack_130 = puVar17;
      }
      else {
        uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
        puStack_130 = (uint *)CONCAT31(extraout_var_00,uVar3);
        unaff_EDI = iStack_138;
      }
      uVar5 = FUN_00482fb0(iStack_12c * 4);
      iVar16 = 0;
      *(undefined4 *)((this->cls_0x5a486c).mbr_0x28 + iStack_138 * 4) = uVar5;
      if (0 < iStack_12c) {
        do {
          puVar6 = (undefined4 *)FUN_00482fb0((this->cls_0x5a486c).mbr_0x24 << 5);
          uVar12 = *puVar17;
          *(undefined4 **)(*(int *)((this->cls_0x5a486c).mbr_0x28 + iStack_138 * 4) + iVar16 * 4) =
               puVar6;
          if (uVar12 == 0) {
            pbVar11 = (byte *)0x0;
          }
          else {
            pbVar11 = (byte *)(uVar12 + (int)puVar17);
          }
          if (*(int *)(pbVar11 + iVar16 * 4) == 0) {
            puVar8 = (undefined4 *)0x0;
          }
          else {
            puVar8 = (undefined4 *)(pbVar11 + *(int *)(pbVar11 + iVar16 * 4) + iVar16 * 4);
          }
          for (uVar12 = ((this->cls_0x5a486c).mbr_0x24 << 5) >> 2; uVar12 != 0; uVar12 = uVar12 - 1)
          {
            *puVar6 = *puVar8;
            puVar8 = puVar8 + 1;
            puVar6 = puVar6 + 1;
          }
          iVar16 = iVar16 + 1;
          for (iVar13 = 0; iVar13 != 0; iVar13 = iVar13 + -1) {
            *(undefined *)puVar6 = *(undefined *)puVar8;
            puVar8 = (undefined4 *)((int)puVar8 + 1);
            puVar6 = (undefined4 *)((int)puVar6 + 1);
          }
          puVar17 = puStack_130;
        } while (iVar16 < iStack_12c);
      }
      iStack_138 = iStack_138 + 1;
      puVar17 = puVar17 + 1;
    } while (iStack_138 < iVar15);
  }
  uVar12 = in_stack_00000004[0x42];
  (this->cls_0x5a486c).mbr_0x2c = uVar12;
  puVar6 = (undefined4 *)FUN_00482fb0(uVar12 * 6);
  uVar12 = in_stack_00000004[0x43];
  (this->cls_0x5a486c).mbr_0x30 = (dword)puVar6;
  if (uVar12 == 0) {
    puVar8 = (undefined4 *)0x0;
  }
  else {
    puVar8 = (undefined4 *)((int)in_stack_00000004 + uVar12 + 0x10c);
  }
  uVar12 = in_stack_00000004[0x42];
  for (uVar14 = uVar12 * 6 >> 2; uVar14 != 0; uVar14 = uVar14 - 1) {
    *puVar6 = *puVar8;
    puVar8 = puVar8 + 1;
    puVar6 = puVar6 + 1;
  }
  for (uVar12 = uVar12 * 6 & 3; uVar12 != 0; uVar12 = uVar12 - 1) {
    *(undefined *)puVar6 = *(undefined *)puVar8;
    puVar8 = (undefined4 *)((int)puVar8 + 1);
    puVar6 = (undefined4 *)((int)puVar6 + 1);
  }
  if (((int)in_stack_00000004[0x1d6] < 0x1389) && (0 < (int)in_stack_00000004[0x1d6])) {
    iStack_138 = 0;
    local_134 = in_stack_00000004 + 0x6f7;
    puStack_130 = in_stack_00000004 + 0x537;
    puVar17 = in_stack_00000004 + 0x417;
    puVar9 = in_stack_00000004 + 0x1d7;
    do {
      puVar7 = puVar9;
      puVar19 = auStack_114;
      for (iVar15 = 8; iVar15 != 0; iVar15 = iVar15 + -1) {
        *puVar19 = *puVar7;
        puVar7 = puVar7 + 1;
        puVar19 = puVar19 + 1;
      }
      apuStack_120[0] = puVar17;
      FUN_0059bd3e(auStack_114);
      uStack_f0 = (uint)*(ushort *)puVar17;
      piStack_d8 = (int *)(uint)*(ushort *)(puVar17 + -0x40);
      uStack_ec = (uint)*(ushort *)(puVar17 + -0x20);
      uStack_e4 = (uint)*(ushort *)(puStack_130 + -0x100);
      iStack_e8 = 0;
      iStack_dc = FUN_00482fb0(in_stack_00000004[0xe5] * 4 + 4);
      iStack_e0 = FUN_00482fb0(in_stack_00000004[0xe5] * 4 + 4);
      uVar12 = in_stack_00000004[0xe5];
      iVar15 = 0;
      puVar17 = puStack_130;
      if (0 < (int)(uVar12 + 1)) {
        do {
          iStack_e8 = iStack_e8 + (uint)*(ushort *)puVar17;
          *(uint *)(iStack_dc + iVar15 * 4) = (uint)*(ushort *)puVar17;
          *(uint *)(iStack_e0 + iVar15 * 4) = *(ushort *)(puVar17 + -0x100) - uStack_e4;
          iVar15 = iVar15 + 1;
          puVar17 = (uint *)((int)puVar17 + 2);
        } while (iVar15 < (int)(uVar12 + 1));
      }
      uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      iStack_f4 = FUN_00482fb0(CONCAT31(extraout_var_01,uVar3) << 2);
      if ((*(byte *)in_stack_00000004 & 4) == 0) {
        iVar15 = 0;
        uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        if (CONCAT31(extraout_var_04,uVar3) != 0 && -1 < extraout_var_04) {
          do {
            pcVar2 = (this->cls_0x5a486c).vftptr_0x0;
            *(undefined4 *)(iStack_f4 + iVar15 * 4) = 0xffffffff;
            iVar15 = iVar15 + 1;
            uVar3 = (*pcVar2->virt_meth_0x4101f0_60)(this);
          } while (iVar15 < CONCAT31(extraout_var_05,uVar3));
        }
      }
      else {
        if (*local_134 == 0) {
          pbVar11 = (byte *)0x0;
        }
        else {
          pbVar11 = (byte *)((int)local_134 + *local_134);
        }
        iVar15 = 0;
        uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        if (CONCAT31(extraout_var_02,uVar3) != 0 && -1 < extraout_var_02) {
          do {
            pcVar2 = (this->cls_0x5a486c).vftptr_0x0;
            *(uint *)(iStack_f4 + iVar15 * 4) = (uint)pbVar11[iVar15];
            iVar15 = iVar15 + 1;
            uVar3 = (*pcVar2->virt_meth_0x4101f0_60)(this);
          } while (iVar15 < CONCAT31(extraout_var_03,uVar3));
        }
      }
      uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      piStack_d0 = (int *)FUN_00482fb0(CONCAT31(extraout_var_06,uVar3) << 2);
      uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      iStack_d4 = FUN_00482fb0(CONCAT31(extraout_var_07,uVar3) << 2);
      iStack_12c = 0;
      uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      if (CONCAT31(extraout_var_08,uVar3) != 0 && -1 < extraout_var_08) {
        iStack_118 = -0x19dc - (int)in_stack_00000004;
        puVar17 = in_stack_00000004 + 0x677;
        do {
          iVar15 = iStack_118;
          if ((*puVar17 == 0) || ((byte *)(*puVar17 + (int)puVar17) == (byte *)0x0)) {
            *(undefined4 *)((byte *)(iStack_118 + (int)puVar17) + iStack_d4) = 0;
            *(undefined4 *)((byte *)(iStack_118 + (int)puVar17) + (int)piStack_d0) = 0;
          }
          else {
            iVar16 = iStack_12c;
            uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            uVar5 = FUN_00482fb0(CONCAT31(extraout_var_09,uVar3) << 3,iVar16);
            pcVar2 = (this->cls_0x5a486c).vftptr_0x0;
            *(undefined4 *)((int)puVar17 + iStack_d4 + iVar15) = uVar5;
            uVar3 = (*pcVar2->virt_meth_0x4104a0_144)(this);
            *(uint *)((int)puVar17 + iStack_dc + iVar15) = CONCAT31(extraout_var_10,uVar3);
            uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            puStack_130 = (uint *)(CONCAT31(extraout_var_11,uVar3) << 3);
            uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            puVar6 = (undefined4 *)(iStack_12c + CONCAT31(extraout_var_12,uVar3) * unaff_EDI * 8);
            puVar8 = *(undefined4 **)((int)puVar17 + iStack_e0 + iVar15);
            for (uVar12 = (uint)local_134 >> 2; uVar12 != 0; uVar12 = uVar12 - 1) {
              *puVar8 = *puVar6;
              puVar6 = puVar6 + 1;
              puVar8 = puVar8 + 1;
            }
            for (uVar12 = (uint)local_134 & 3; uVar12 != 0; uVar12 = uVar12 - 1) {
              *(undefined *)puVar8 = *(undefined *)puVar6;
              puVar6 = (undefined4 *)((int)puVar6 + 1);
              puVar8 = (undefined4 *)((int)puVar8 + 1);
            }
          }
          iStack_12c = iStack_12c + 1;
          puVar17 = puVar17 + 1;
          uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        } while (iStack_12c < CONCAT31(extraout_var_13,uVar3));
      }
      if ((this->cls_0x5a486c).mbr_0xc == 0) {
        if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
          cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
        }
        meth_0x407510(this);
      }
      puVar7 = (uint *)FUN_00482fb0(0x4c);
      puVar17 = auStack_114;
      for (iVar15 = 0x13; iVar15 != 0; iVar15 = iVar15 + -1) {
        *puVar7 = *puVar17;
        puVar17 = puVar17 + 1;
        puVar7 = puVar7 + 1;
      }
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x64);
      puStack_130 = puStack_130 + 4;
      local_134 = local_134 + 1;
      iStack_138 = iStack_138 + 1;
      puVar17 = (uint *)((int)apuStack_120[0] + 2);
      puVar9 = puVar9 + 8;
    } while (iStack_138 < (int)in_stack_00000004[0x1d6]);
    apuStack_120[0] = puVar17;
    uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    uVar5 = FUN_00482fb0(CONCAT31(extraout_var_14,uVar3) << 2);
    *(undefined4 *)&(this->cls_0x5a486c).field_0x98 = uVar5;
    uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    puVar6 = *(undefined4 **)&(this->cls_0x5a486c).field_0x98;
    for (uVar12 = CONCAT31(extraout_var_15,uVar3) & 0x3fffffff; uVar12 != 0; uVar12 = uVar12 - 1) {
      *puVar6 = 0;
      puVar6 = puVar6 + 1;
    }
    for (iVar15 = 0; iVar15 != 0; iVar15 = iVar15 + -1) {
      *(undefined *)puVar6 = 0;
      puVar6 = (undefined4 *)((int)puVar6 + 1);
    }
    if (((this->cls_0x5a486c).mbr_0x1c & 1) == 0) {
      iStack_138 = 0;
      uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      if (CONCAT31(extraout_var_16,uVar3) != 0 && -1 < extraout_var_16) {
        local_134 = (uint *)0x0;
        puStack_130 = in_stack_00000004 + 0x637;
        do {
          if (*puStack_130 == 0) {
            dVar4 = (this->cls_0x5a486c).mbr_0x4;
            *(undefined4 *)
             (*(int *)&(this->cls_0x5a486c).field_0x98 + (-0x18dc - (int)in_stack_00000004) +
             (int)puStack_130) = 0;
            pbVar11 = (byte *)(*(int *)(dVar4 + 0x54) + 0x31 + (int)local_134);
            *pbVar11 = *pbVar11 | 0x20;
          }
          else {
            iVar15 = iStack_138;
            uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            puVar17 = (uint *)FUN_00482fb0(CONCAT31(extraout_var_17,uVar3) << 3,iVar15);
            *(uint **)(*(int *)&(this->cls_0x5a486c).field_0x98 + (-0x18dc - (int)in_stack_00000004)
                      + (int)puStack_130) = puVar17;
            if (*puStack_130 == 0) {
              puVar9 = (uint *)0x0;
            }
            else {
              puVar9 = (uint *)(*puStack_130 + (int)local_134);
            }
            iVar15 = 0;
            uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            if (CONCAT31(extraout_var_18,uVar3) != 0 && -1 < extraout_var_18) {
              do {
                uVar12 = *puVar9;
                puVar9 = puVar9 + 1;
                *puVar17 = uVar12 >> 8 & 0xffff;
                pcVar2 = (this->cls_0x5a486c).vftptr_0x0;
                puVar17[1] = uVar12 >> 0x18 ^ uVar12 & 0xff000000;
                iVar15 = iVar15 + 1;
                puVar17 = puVar17 + 2;
                uVar3 = (*pcVar2->virt_meth_0x4104a0_144)(this);
              } while (iVar15 < CONCAT31(extraout_var_19,uVar3));
            }
          }
          iStack_138 = iStack_138 + 1;
          local_134 = (uint *)((int)local_134 + 0x4c);
          puStack_130 = puStack_130 + 1;
          uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        } while (iStack_138 < CONCAT31(extraout_var_20,uVar3));
      }
    }
    pdVar1 = &(this->cls_0x5a486c).mbr_0x4c;
    (this->cls_0x5a486c).mbr_0x10 = (uint)(in_stack_00000004[0xe5] != 0);
    iVar15 = 0;
    if (0 < (int)*pdVar1) {
      do {
        if ((-1 < iVar15) &&
           (iVar16 = *(int *)((this->cls_0x5a486c).mbr_0x5c + iVar15 * 4), iVar16 != 0)) {
          FUN_004830f0(iVar16);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
        iVar15 = iVar15 + 1;
      } while (iVar15 < (int)*pdVar1);
    }
    bVar21 = DAT_005c619c != 0;
    *pdVar1 = 0;
    *(undefined4 *)&(this->cls_0x5a486c).field_0x50 = 0;
    if ((bVar21) && (iStack_138 = 0, 0 < (int)in_stack_00000004[0xe5])) {
      local_134 = in_stack_00000004 + 0xe6;
      do {
        puVar17 = auStack_114;
        for (iVar15 = 0x1f; iVar15 != 0; iVar15 = iVar15 + -1) {
          *puVar17 = 0;
          puVar17 = puVar17 + 1;
        }
        puVar17 = local_134;
        puVar9 = auStack_114;
        for (iVar15 = 0x1b; iVar15 != 0; iVar15 = iVar15 + -1) {
          *puVar9 = *puVar17;
          puVar17 = puVar17 + 1;
          puVar9 = puVar9 + 1;
        }
        auStack_114[0] = 0x7c;
        puVar6 = auStack_98;
        for (iVar15 = 0x26; iVar15 != 0; iVar15 = iVar15 + -1) {
          *puVar6 = 0;
          puVar6 = puVar6 + 1;
        }
        iVar15 = meth_0x40bb30(this,auStack_98);
        if (iVar15 != 0) {
          puVar8 = (undefined4 *)FUN_00482fb0(0x98);
          puVar6 = auStack_98;
          for (iVar15 = 0x26; iVar15 != 0; iVar15 = iVar15 + -1) {
            *puVar8 = *puVar6;
            puVar6 = puVar6 + 1;
            puVar8 = puVar8 + 1;
          }
          cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x4c);
        }
        iStack_138 = iStack_138 + 1;
        local_134 = local_134 + 0x1b;
      } while (iStack_138 < (int)in_stack_00000004[0xe5]);
    }
    pdVar1 = &(this->cls_0x5a486c).mbr_0x34;
    iVar15 = 0;
    if (0 < (int)(this->cls_0x5a486c).mbr_0x34) {
      do {
        if ((-1 < iVar15) &&
           (iVar16 = *(int *)((this->cls_0x5a486c).mbr_0x44 + iVar15 * 4), iVar16 != 0)) {
          FUN_004830f0(iVar16);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
        iVar15 = iVar15 + 1;
      } while (iVar15 < (int)*pdVar1);
    }
    uVar12 = in_stack_00000004[0x44];
    *pdVar1 = 0;
    *(undefined4 *)&(this->cls_0x5a486c).field_0x38 = 0;
    iStack_138 = 0;
    if (0 < (int)uVar12) {
      local_134 = in_stack_00000004 + 0x45;
      do {
        puVar17 = local_134;
        puVar9 = auStack_114;
        for (iVar15 = 0x14; iVar15 != 0; iVar15 = iVar15 + -1) {
          *puVar9 = *puVar17;
          puVar17 = puVar17 + 1;
          puVar9 = puVar9 + 1;
        }
        iVar15 = (**(code **)(*DAT_00668eb0 + 0x14))(DAT_00668eb0,auStack_c4,0);
        if (iVar15 != 0) {
          FUN_004a90d0(iVar15,s_d__revenant_3DImage_cpp_005c5ee8,0x9ff);
        }
        piStack_d8 = (int *)0x0;
        (**(code **)(*piStack_d0 + 0xc))(piStack_d0,apuStack_120);
        (**(code **)(*piStack_d8 + 0x14))(piStack_d8,DAT_00668f14,&iStack_d4);
        puVar9 = (uint *)FUN_00482fb0(0x5c);
        puVar17 = auStack_114;
        for (iVar15 = 0x17; iVar15 != 0; iVar15 = iVar15 + -1) {
          *puVar9 = *puVar17;
          puVar17 = puVar17 + 1;
          puVar9 = puVar9 + 1;
        }
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)pdVar1);
        iStack_138 = iStack_138 + 1;
        local_134 = local_134 + 0x14;
      } while (iStack_138 < (int)in_stack_00000004[0x44]);
    }
    *(undefined4 *)&(this->cls_0x5a486c).field_0x9c = 0;
    if (((this->cls_0x5a486c).mbr_0x1c & 2) != 0) {
      iStack_138 = 0;
      uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      if (CONCAT31(extraout_var_21,uVar3) != 0 && -1 < extraout_var_21) {
        puStack_128 = in_stack_00000004 + 0x6b7;
        do {
          if (((*puStack_128 != 0) &&
              (puVar17 = (uint *)(*puStack_128 + (int)puStack_128), puVar17 != (uint *)0x0)) &&
             (0 < (int)*puVar17)) {
            if (*(int *)&(this->cls_0x5a486c).field_0x9c == 0) {
              uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
              uVar5 = FUN_00482fb0(CONCAT31(extraout_var_22,uVar3) << 3);
              *(undefined4 *)&(this->cls_0x5a486c).field_0x9c = uVar5;
            }
            uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
            puVar6 = *(undefined4 **)&(this->cls_0x5a486c).field_0x9c;
            for (uVar12 = (uint)(CONCAT31(extraout_var_23,uVar3) << 3) >> 2; uVar12 != 0;
                uVar12 = uVar12 - 1) {
              *puVar6 = 0;
              puVar6 = puVar6 + 1;
            }
            for (iVar15 = 0; iVar15 != 0; iVar15 = iVar15 + -1) {
              *(undefined *)puVar6 = 0;
              puVar6 = (undefined4 *)((int)puVar6 + 1);
            }
            puVar6 = (undefined4 *)FUN_00482fb0(*puVar17);
            if ((puVar17[1] == 0) || ((byte *)(puVar17[1] + 4 + (int)puVar17) == (byte *)0x0)) {
              if ((puVar17[2] != 0) && ((byte *)(puVar17[2] + 8 + (int)puVar17) != (byte *)0x0)) {
                if (puVar17[2] == 0) {
                  puVar8 = (undefined4 *)0x0;
                }
                else {
                  puVar8 = (undefined4 *)(puVar17[2] + 8 + (int)puVar17);
                }
                uVar12 = *puVar17;
                puVar20 = puVar6;
                for (uVar14 = uVar12 >> 2; uVar14 != 0; uVar14 = uVar14 - 1) {
                  *puVar20 = *puVar8;
                  puVar8 = puVar8 + 1;
                  puVar20 = puVar20 + 1;
                }
                iVar15 = *(int *)&(this->cls_0x5a486c).field_0x9c;
                for (uVar12 = uVar12 & 3; uVar12 != 0; uVar12 = uVar12 - 1) {
                  *(undefined *)puVar20 = *(undefined *)puVar8;
                  puVar8 = (undefined4 *)((int)puVar8 + 1);
                  puVar20 = (undefined4 *)((int)puVar20 + 1);
                }
                iVar16 = 0;
                *(undefined4 **)(iVar15 + 4 + iStack_138 * 8) = puVar6;
                if (0 < (int)puVar6[3]) {
                  piVar18 = puVar6 + 0xe;
                  do {
                    if (*piVar18 == 0) {
                      puVar10 = (undefined *)0x0;
                    }
                    else {
                      puVar10 = (undefined *)(*piVar18 + (int)piVar18);
                    }
                    FUN_004b8820(puVar10);
                    iVar16 = iVar16 + 1;
                    piVar18 = piVar18 + 10;
                  } while (iVar16 < (int)puVar6[3]);
                }
              }
            }
            else {
              if (puVar17[1] == 0) {
                puVar8 = (undefined4 *)0x0;
              }
              else {
                puVar8 = (undefined4 *)(puVar17[1] + 4 + (int)puVar17);
              }
              uVar12 = *puVar17;
              puVar20 = puVar6;
              for (uVar14 = uVar12 >> 2; uVar14 != 0; uVar14 = uVar14 - 1) {
                *puVar20 = *puVar8;
                puVar8 = puVar8 + 1;
                puVar20 = puVar20 + 1;
              }
              iVar15 = *(int *)&(this->cls_0x5a486c).field_0x9c;
              for (uVar12 = uVar12 & 3; uVar12 != 0; uVar12 = uVar12 - 1) {
                *(undefined *)puVar20 = *(undefined *)puVar8;
                puVar8 = (undefined4 *)((int)puVar8 + 1);
                puVar20 = (undefined4 *)((int)puVar20 + 1);
              }
              *(undefined4 **)(iVar15 + iStack_138 * 8) = puVar6;
              FUN_004b8820(puVar6);
            }
          }
          iStack_138 = iStack_138 + 1;
          puStack_128 = puStack_128 + 1;
          uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        } while (iStack_138 < CONCAT31(extraout_var_24,uVar3));
      }
    }
    pdVar1 = &(this->cls_0x5a486c).mbr_0x7c;
    iVar15 = 0;
    if (0 < (int)(this->cls_0x5a486c).mbr_0x7c) {
      do {
        if ((-1 < iVar15) &&
           (iVar16 = *(int *)((this->cls_0x5a486c).mbr_0x8c + iVar15 * 4), iVar16 != 0)) {
          if (*(int *)(iVar16 + 8) != 0) {
            FUN_00482f80(*(int *)(iVar16 + 8));
          }
          if (*(int *)(iVar16 + 0xc) != 0) {
            FUN_00482f80(*(int *)(iVar16 + 0xc));
          }
          FUN_004830f0(iVar16);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
        iVar15 = iVar15 + 1;
      } while (iVar15 < (int)*pdVar1);
    }
    *pdVar1 = 0;
    *(undefined4 *)&(this->cls_0x5a486c).field_0x80 = 0;
    cls_0x5a486c::meth_0x447d80(&this->cls_0x5a486c);
    return 1;
  }
  return 0;
}



// Function at 00407510

undefined4 __thiscall OOAnalyzer::cls_0x5a35ac::meth_0x407510(cls_0x5a35ac *this)

{
  dword *pdVar1;
  byte bVar2;
  uint uVar3;
  cls_0x5a486c__vftable_5a486c *pcVar4;
  undefined uVar5;
  undefined4 uVar6;
  undefined3 extraout_var;
  dword dVar7;
  undefined4 *puVar8;
  undefined3 extraout_var_00;
  int3 extraout_var_01;
  undefined3 extraout_var_02;
  int3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined3 extraout_var_05;
  undefined3 extraout_var_06;
  int3 extraout_var_07;
  undefined3 extraout_var_08;
  undefined3 extraout_var_09;
  undefined3 extraout_var_10;
  undefined3 extraout_var_11;
  undefined4 *puVar9;
  undefined3 extraout_var_12;
  undefined3 extraout_var_13;
  int3 extraout_var_14;
  undefined3 extraout_var_15;
  undefined3 extraout_var_16;
  int3 extraout_var_17;
  undefined3 extraout_var_18;
  int3 extraout_var_19;
  undefined3 extraout_var_20;
  undefined3 extraout_var_21;
  uint *puVar10;
  int3 extraout_var_22;
  undefined3 extraout_var_23;
  undefined3 extraout_var_24;
  undefined *puVar11;
  undefined3 extraout_var_25;
  undefined4 *puVar12;
  int3 extraout_var_26;
  int *piVar13;
  undefined3 extraout_var_27;
  byte *pbVar14;
  int *piVar15;
  uint uVar16;
  int iVar17;
  uint uVar18;
  uint *puVar19;
  int iVar20;
  byte *pbVar21;
  int unaff_EBP;
  uint *puVar22;
  char *pcVar23;
  int iVar24;
  char *pcVar25;
  bool bVar26;
  uint *in_stack_00000004;
  uint *puStack_128;
  uint *puStack_124;
  uint *puStack_120;
  uint *puStack_11c;
  undefined4 auStack_118 [8];
  int iStack_f8;
  uint uStack_f4;
  uint uStack_f0;
  int iStack_ec;
  uint uStack_e8;
  int iStack_e4;
  int iStack_e0;
  int *piStack_dc;
  int iStack_d8;
  int *piStack_d4;
  undefined auStack_c8 [48];
  uint auStack_98 [38];
  
  if ((this->cls_0x5a486c).mbr_0xc != 0) {
    return 1;
  }
  (this->cls_0x5a486c).mbr_0xb0 = 0xffffffff;
  (this->cls_0x5a486c).mbr_0xac = 0xffffffff;
  (this->cls_0x5a486c).mbr_0xc0 = 0xffffffff;
  (this->cls_0x5a486c).mbr_0xbc = 0xffffffff;
  bVar2 = *(byte *)in_stack_00000004;
  (this->cls_0x5a486c).mbr_0xb8 = 0;
  (this->cls_0x5a486c).mbr_0xb4 = 0;
  if ((bVar2 & 0x10) == 0) {
    uVar6 = meth_0x406a50(this);
    return uVar6;
  }
  if (3 < in_stack_00000004[1]) {
    FUN_00481c10(s__Newer_imagery_version_in_file___005c5cdf + 1,(this->cls_0x5a486c).mbr_0x4);
  }
  uVar16 = *in_stack_00000004;
  uVar18 = in_stack_00000004[1];
  uVar3 = in_stack_00000004[3];
  puVar19 = (uint *)0x1;
  (this->cls_0x5a486c).mbr_0xc = 1;
  *(uint *)&(this->cls_0x5a486c).mbr_0x1c = uVar16;
  *(uint *)&(this->cls_0x5a486c).field_0x20 = uVar18;
  (this->cls_0x5a486c).mbr_0x24 = uVar3;
  if ((uVar16 & 1) != 0) {
    uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    puVar19 = (uint *)CONCAT31(extraout_var,uVar5);
  }
  puStack_124 = puVar19;
  dVar7 = FUN_00482fb0((int)puVar19 * 4);
  (this->cls_0x5a486c).mbr_0x28 = dVar7;
  puStack_120 = (uint *)0x0;
  if (0 < (int)puVar19) {
    do {
      iVar17 = (int)puStack_120;
      if (((this->cls_0x5a486c).mbr_0x1c & 1) == 0) {
        puStack_128 = (uint *)0x1;
      }
      else {
        (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
      }
      uVar6 = FUN_00482fb0((int)puStack_128 * 4);
      iVar20 = 0;
      *(undefined4 *)((this->cls_0x5a486c).mbr_0x28 + iVar17 * 4) = uVar6;
      if (0 < (int)puStack_128) {
        puStack_11c = in_stack_00000004 + iVar17 + 4;
        do {
          puVar8 = (undefined4 *)FUN_00482fb0((this->cls_0x5a486c).mbr_0x24 << 5);
          uVar16 = in_stack_00000004[4];
          *(undefined4 **)(*(int *)((this->cls_0x5a486c).mbr_0x28 + iVar17 * 4) + iVar20 * 4) =
               puVar8;
          piVar15 = (int *)(iVar20 * 4 + *(int *)((int)puStack_11c + uVar16) +
                           (int)(int *)((int)puStack_11c + uVar16));
          iVar17 = *piVar15;
          if (iVar17 == 0) {
            puVar9 = (undefined4 *)0x0;
          }
          else {
            puVar9 = (undefined4 *)(iVar17 + (int)piVar15);
          }
          for (uVar16 = ((this->cls_0x5a486c).mbr_0x24 << 5) >> 2; uVar16 != 0; uVar16 = uVar16 - 1)
          {
            *puVar8 = *puVar9;
            puVar9 = puVar9 + 1;
            puVar8 = puVar8 + 1;
          }
          iVar20 = iVar20 + 1;
          for (iVar17 = 0; iVar17 != 0; iVar17 = iVar17 + -1) {
            *(undefined *)puVar8 = *(undefined *)puVar9;
            puVar9 = (undefined4 *)((int)puVar9 + 1);
            puVar8 = (undefined4 *)((int)puVar8 + 1);
          }
          iVar17 = (int)puStack_120;
        } while (iVar20 < (int)puStack_128);
      }
      puStack_120 = (uint *)(iVar17 + 1);
    } while ((int)puStack_120 < (int)puStack_124);
  }
  uVar16 = in_stack_00000004[5];
  (this->cls_0x5a486c).mbr_0x2c = uVar16;
  puVar8 = (undefined4 *)FUN_00482fb0(uVar16 * 6);
  uVar16 = in_stack_00000004[6];
  (this->cls_0x5a486c).mbr_0x30 = (dword)puVar8;
  if (uVar16 == 0) {
    puVar9 = (undefined4 *)0x0;
  }
  else {
    puVar9 = (undefined4 *)(uVar16 + 0x18 + (int)in_stack_00000004);
  }
  uVar16 = in_stack_00000004[5];
  for (uVar18 = uVar16 * 6 >> 2; uVar18 != 0; uVar18 = uVar18 - 1) {
    *puVar8 = *puVar9;
    puVar9 = puVar9 + 1;
    puVar8 = puVar8 + 1;
  }
  for (uVar16 = uVar16 * 6 & 3; uVar16 != 0; uVar16 = uVar16 - 1) {
    *(undefined *)puVar8 = *(undefined *)puVar9;
    puVar9 = (undefined4 *)((int)puVar9 + 1);
    puVar8 = (undefined4 *)((int)puVar8 + 1);
  }
  if ((5000 < (int)in_stack_00000004[0xb]) || ((int)in_stack_00000004[0xb] < 1)) {
    return 0;
  }
  puStack_120 = (uint *)0x0;
  puStack_11c = (uint *)0x0;
  puVar19 = in_stack_00000004 + 0xc;
  do {
    puVar8 = (undefined4 *)((byte *)((int)puStack_11c + *puVar19) + (int)puVar19);
    puVar9 = auStack_118;
    for (iVar17 = 8; iVar17 != 0; iVar17 = iVar17 + -1) {
      *puVar9 = *puVar8;
      puVar8 = puVar8 + 1;
      puVar9 = puVar9 + 1;
    }
    FUN_0059bd3e(auStack_118);
    uVar16 = *puVar19;
    iStack_ec = 0;
    piStack_dc = (int *)(uint)*(ushort *)((byte *)((int)puStack_11c + (int)puVar19) + uVar16 + 0x20)
    ;
    uStack_f4 = (uint)*(ushort *)((byte *)((int)puStack_11c + (int)puVar19) + uVar16 + 0x24);
    uStack_f0 = (uint)*(ushort *)((byte *)((int)puStack_11c + (int)puVar19) + uVar16 + 0x22);
    uStack_e8 = (uint)*(ushort *)
                       ((byte *)((int)puStack_11c + (int)puVar19) +
                       *(int *)((byte *)((int)puStack_11c + (int)puVar19) + uVar16 + 0x28) +
                       uVar16 + 0x28);
    iStack_e0 = FUN_00482fb0(in_stack_00000004[9] * 4 + 4);
    iStack_e4 = FUN_00482fb0(in_stack_00000004[9] * 4 + 4);
    uVar16 = in_stack_00000004[9];
    iVar17 = 0;
    if (0 < (int)(uVar16 + 1)) {
      puStack_124 = (uint *)((byte *)((int)puStack_11c + (int)puVar19) + *puVar19 + 0x28);
      do {
        iVar20 = iVar17 * 4;
        iStack_ec = iStack_ec +
                    (uint)*(ushort *)
                           ((byte *)((int)puStack_11c + *puVar19 + 0x2a) + (int)puVar19 +
                           iVar20 + *(int *)((byte *)((int)puStack_11c + *puVar19 + 0x28) +
                                            (int)puVar19));
        piVar15 = (int *)((byte *)((int)puStack_11c + *puVar19 + 0x28) + (int)puVar19);
        *(uint *)(iVar20 + iStack_e0) = (uint)*(ushort *)((int)piVar15 + *piVar15 + iVar20 + 2);
        iVar17 = iVar17 + 1;
        *(uint *)(iVar20 + iStack_e4) =
             *(ushort *)((int)puStack_124 + *puStack_124 + iVar20) - uStack_e8;
      } while (iVar17 < (int)(uVar16 + 1));
    }
    uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    iStack_f8 = FUN_00482fb0(CONCAT31(extraout_var_00,uVar5) << 2);
    if ((*(byte *)in_stack_00000004 & 4) == 0) {
      iVar17 = 0;
      uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      piVar15 = (int *)puStack_11c;
      if (CONCAT31(extraout_var_03,uVar5) != 0 && -1 < extraout_var_03) {
        do {
          pcVar4 = (this->cls_0x5a486c).vftptr_0x0;
          *(undefined4 *)(iStack_f8 + iVar17 * 4) = 0xffffffff;
          iVar17 = iVar17 + 1;
          uVar5 = (*pcVar4->virt_meth_0x4101f0_60)(this);
          piVar15 = (int *)puStack_11c;
        } while (iVar17 < CONCAT31(extraout_var_04,uVar5));
      }
    }
    else {
      iVar17 = 0;
      uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      piVar15 = (int *)puStack_11c;
      if (CONCAT31(extraout_var_01,uVar5) != 0 && -1 < extraout_var_01) {
        puStack_128 = (uint *)0x0;
        do {
          if ((*(byte *)in_stack_00000004 & 0x80) == 0) {
            iVar20 = *(int *)((byte *)((int)piVar15 + *puVar19 + 0x2c) + (int)puVar19);
            if (iVar20 == 0) {
              pbVar21 = (byte *)0x0;
            }
            else {
              pbVar21 = (byte *)((int)piVar15 + *puVar19 + 0x2c) + (int)puVar19 + iVar20;
            }
            uVar6 = *(undefined4 *)(pbVar21 + iVar17 * 8);
          }
          else {
            uVar6 = *(undefined4 *)
                     ((int)((byte *)((int)piVar15 + (int)puVar19) + *puVar19 + 0x2c) +
                     *(int *)((byte *)((int)piVar15 + (int)puVar19) + *puVar19 + 0x2c) +
                     (int)puStack_128);
          }
          puStack_128 = (uint *)((int)puStack_128 + 0xc);
          pcVar4 = (this->cls_0x5a486c).vftptr_0x0;
          *(undefined4 *)(iStack_f8 + iVar17 * 4) = uVar6;
          iVar17 = iVar17 + 1;
          uVar5 = (*pcVar4->virt_meth_0x4101f0_60)(this);
        } while (iVar17 < CONCAT31(extraout_var_02,uVar5));
      }
    }
    uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    piStack_d4 = (int *)FUN_00482fb0(CONCAT31(extraout_var_05,uVar5) << 2);
    uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    iStack_d8 = FUN_00482fb0(CONCAT31(extraout_var_06,uVar5) << 2);
    puStack_128 = (uint *)0x0;
    uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    if (CONCAT31(extraout_var_07,uVar5) != 0 && -1 < extraout_var_07) {
      puStack_124 = (uint *)0x0;
      do {
        if ((*(byte *)in_stack_00000004 & 0x80) == 0) {
          iVar17 = *(int *)((byte *)((int)piVar15 + (int)puVar19) + *puVar19 + 0x2c);
          if (iVar17 == 0) {
            pbVar21 = (byte *)0x0;
          }
          else {
            pbVar21 = (byte *)((int)piVar15 + (int)puVar19) + iVar17 + *puVar19 + 0x2c;
          }
          piVar15 = (int *)(pbVar21 + (int)puStack_128 * 8 + 4);
          if ((*(int *)(pbVar21 + (int)puStack_128 * 8 + 4) != 0) &&
             ((byte *)(*(int *)(pbVar21 + (int)puStack_128 * 8 + 4) + (int)piVar15) != (byte *)0x0))
          {
            iVar17 = (int)puStack_128;
            uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            uVar6 = FUN_00482fb0(CONCAT31(extraout_var_08,uVar5) << 3,iVar17);
            pcVar4 = (this->cls_0x5a486c).vftptr_0x0;
            *(undefined4 *)(iStack_d8 + (int)puStack_128 * 4) = uVar6;
            uVar5 = (*pcVar4->virt_meth_0x4104a0_144)(this);
            *(uint *)(iStack_e0 + (int)puStack_128 * 4) = CONCAT31(extraout_var_09,uVar5);
            if (*piVar15 == 0) {
              puVar8 = (undefined4 *)0x0;
            }
            else {
              puVar8 = (undefined4 *)(*piVar15 + (int)piVar15);
            }
            uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            uVar16 = CONCAT31(extraout_var_10,uVar5) << 3;
            puVar9 = *(undefined4 **)(iStack_e0 + unaff_EBP * 4);
            goto LAB_00407a8d;
          }
          piStack_d4[(int)puStack_128] = 0;
          *(undefined4 *)(iStack_d8 + (int)puStack_128 * 4) = 0;
        }
        else {
          iVar17 = *(int *)((byte *)((int)puStack_11c + *puVar19 + 0x2c) + (int)puVar19);
          pbVar21 = (byte *)((int)puStack_124 +
                            (int)((byte *)((int)puStack_11c + *puVar19 + 0x2c) + (int)puVar19));
          if ((*(int *)(pbVar21 + iVar17 + 8) == 0) ||
             (pbVar21 + *(int *)(pbVar21 + iVar17 + 8) + 8 + iVar17 == (byte *)0x0)) {
            piStack_d4[(int)puStack_128] = 0;
            *(undefined4 *)(iStack_d8 + (int)puStack_128 * 4) = 0;
          }
          else {
            iVar20 = FUN_00482fb0(*(int *)(pbVar21 + iVar17 + 4) << 2);
            piStack_d4[(int)puStack_128] = iVar20;
            *(undefined4 *)(iStack_d8 + (int)puStack_128 * 4) =
                 *(undefined4 *)(pbVar21 + iVar17 + 4);
            if (*(int *)(pbVar21 + iVar17 + 8) == 0) {
              puVar8 = (undefined4 *)0x0;
            }
            else {
              puVar8 = (undefined4 *)(pbVar21 + *(int *)(pbVar21 + iVar17 + 8) + 8 + iVar17);
            }
            puVar9 = (undefined4 *)piStack_d4[(int)puStack_128];
            uVar16 = *(int *)(pbVar21 + iVar17 + 4) << 2;
LAB_00407a8d:
            for (uVar16 = uVar16 >> 2; uVar16 != 0; uVar16 = uVar16 - 1) {
              *puVar9 = *puVar8;
              puVar8 = puVar8 + 1;
              puVar9 = puVar9 + 1;
            }
            for (iVar17 = 0; iVar17 != 0; iVar17 = iVar17 + -1) {
              *(undefined *)puVar9 = *(undefined *)puVar8;
              puVar8 = (undefined4 *)((int)puVar8 + 1);
              puVar9 = (undefined4 *)((int)puVar9 + 1);
            }
          }
        }
        puStack_128 = (uint *)((int)puStack_128 + 1);
        puStack_124 = puStack_124 + 3;
        uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        piVar15 = (int *)puStack_11c;
      } while ((int)puStack_128 < CONCAT31(extraout_var_11,uVar5));
    }
    if ((this->cls_0x5a486c).mbr_0xc == 0) {
      if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
        cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
      }
      meth_0x407510(this);
    }
    puVar9 = (undefined4 *)FUN_00482fb0(0x4c);
    puVar8 = auStack_118;
    for (iVar17 = 0x13; iVar17 != 0; iVar17 = iVar17 + -1) {
      *puVar9 = *puVar8;
      puVar8 = puVar8 + 1;
      puVar9 = puVar9 + 1;
    }
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x64);
    puStack_11c = puStack_11c + 0xc;
    puStack_120 = (uint *)((int)puStack_120 + 1);
  } while ((int)puStack_120 < (int)in_stack_00000004[0xb]);
  uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
  uVar6 = FUN_00482fb0(CONCAT31(extraout_var_12,uVar5) << 2);
  *(undefined4 *)&(this->cls_0x5a486c).field_0x98 = uVar6;
  uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
  puVar8 = *(undefined4 **)&(this->cls_0x5a486c).field_0x98;
  for (uVar16 = CONCAT31(extraout_var_13,uVar5) & 0x3fffffff; uVar16 != 0; uVar16 = uVar16 - 1) {
    *puVar8 = 0;
    puVar8 = puVar8 + 1;
  }
  for (iVar17 = 0; iVar17 != 0; iVar17 = iVar17 + -1) {
    *(undefined *)puVar8 = 0;
    puVar8 = (undefined4 *)((int)puVar8 + 1);
  }
  if (((this->cls_0x5a486c).mbr_0x1c & 1) == 0) {
    iVar17 = 0;
    uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    if (CONCAT31(extraout_var_14,uVar5) != 0 && -1 < extraout_var_14) {
      puStack_120 = (uint *)0x0;
      puStack_11c = (uint *)0x0;
      puStack_128 = (uint *)0x0;
      do {
        if (in_stack_00000004[1] < 2) {
          piVar15 = (int *)puStack_11c;
          if (in_stack_00000004[2] != 0) {
            piVar15 = (int *)((byte *)(in_stack_00000004[2] + 8 + (int)in_stack_00000004) +
                             (int)puStack_11c);
          }
        }
        else if (in_stack_00000004[1] == 2) {
          piVar15 = (int *)puStack_128;
          if (in_stack_00000004[2] != 0) {
            piVar15 = (int *)((int)puStack_128 +
                             (int)(in_stack_00000004[2] + 8 + (int)in_stack_00000004));
          }
        }
        else {
          piVar15 = (int *)((byte *)((int)in_stack_00000004 + in_stack_00000004[2] + 8) +
                           (int)puStack_11c);
        }
        if (piVar15[3] == 0) {
          dVar7 = (this->cls_0x5a486c).mbr_0x4;
          *(undefined4 *)(*(int *)&(this->cls_0x5a486c).field_0x98 + iVar17 * 4) = 0;
          pbVar21 = (byte *)(*(int *)(dVar7 + 0x54) + 0x31 + (int)puStack_120);
          *pbVar21 = *pbVar21 | 0x20;
        }
        else {
          iVar20 = iVar17;
          uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
          puVar19 = (uint *)FUN_00482fb0(CONCAT31(extraout_var_15,uVar5) << 3,iVar20);
          *(uint **)(*(int *)&(this->cls_0x5a486c).field_0x98 + iVar17 * 4) = puVar19;
          if ((*in_stack_00000004 & 0x40) == 0) {
            iVar20 = piVar15[3];
            if ((*in_stack_00000004 & 8) == 0) {
              if (iVar20 == 0) {
                puStack_128 = (uint *)0x0;
              }
              else {
                puStack_128 = (uint *)((int)piVar15 + iVar20 + 0xc);
              }
              iVar20 = 0;
              uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
              if (CONCAT31(extraout_var_19,uVar5) != 0 && -1 < extraout_var_19) {
                do {
                  uVar16 = *puStack_120;
                  puStack_120 = puStack_120 + 1;
                  *puVar19 = uVar16 >> 8 & 0xffff;
                  pcVar4 = (this->cls_0x5a486c).vftptr_0x0;
                  puVar19[1] = uVar16 >> 0x18 ^ uVar16 & 0xff000000;
                  iVar20 = iVar20 + 1;
                  uVar5 = (*pcVar4->virt_meth_0x4104a0_144)(this);
                  puVar19 = puVar19 + 2;
                } while (iVar20 < CONCAT31(extraout_var_20,uVar5));
              }
            }
            else {
              if (iVar20 == 0) {
                puStack_128 = (uint *)0x0;
              }
              else {
                puStack_128 = (uint *)((int)piVar15 + iVar20 + 0xc);
              }
              iVar20 = 0;
              uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
              if (CONCAT31(extraout_var_17,uVar5) != 0 && -1 < extraout_var_17) {
                do {
                  auStack_98[0] = *puVar19;
                  uVar16 = *puStack_120;
                  puStack_120 = puStack_120 + 1;
                  *puVar19 = uVar16 & 0xffff;
                  pcVar4 = (this->cls_0x5a486c).vftptr_0x0;
                  puVar19[1] = (uVar16 & 0xffff0000) << 8 | uVar16 >> 0x18;
                  iVar20 = iVar20 + 1;
                  uVar5 = (*pcVar4->virt_meth_0x4104a0_144)(this);
                  puVar19 = puVar19 + 2;
                } while (iVar20 < CONCAT31(extraout_var_18,uVar5));
              }
            }
          }
          else {
            if (piVar15[3] == 0) {
              puVar8 = (undefined4 *)0x0;
            }
            else {
              puVar8 = (undefined4 *)(piVar15[3] + 0xc + (int)piVar15);
            }
            uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            puVar9 = *(undefined4 **)(*(int *)&(this->cls_0x5a486c).field_0x98 + iVar17 * 4);
            for (uVar16 = (uint)(CONCAT31(extraout_var_16,uVar5) << 3) >> 2; uVar16 != 0;
                uVar16 = uVar16 - 1) {
              *puVar9 = *puVar8;
              puVar8 = puVar8 + 1;
              puVar9 = puVar9 + 1;
            }
            for (iVar20 = 0; iVar20 != 0; iVar20 = iVar20 + -1) {
              *(undefined *)puVar9 = *(undefined *)puVar8;
              puVar8 = (undefined4 *)((int)puVar8 + 1);
              puVar9 = (undefined4 *)((int)puVar9 + 1);
            }
          }
        }
        puStack_120 = puStack_120 + 0x13;
        puStack_128 = puStack_128 + 0x14;
        puStack_11c = puStack_11c + 4;
        iVar17 = iVar17 + 1;
        uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      } while (iVar17 < CONCAT31(extraout_var_21,uVar5));
    }
  }
  pdVar1 = &(this->cls_0x5a486c).mbr_0x4c;
  (this->cls_0x5a486c).mbr_0x10 = (uint)(in_stack_00000004[9] != 0);
  iVar17 = 0;
  if (0 < (int)*pdVar1) {
    do {
      if ((-1 < iVar17) &&
         (iVar20 = *(int *)((this->cls_0x5a486c).mbr_0x5c + iVar17 * 4), iVar20 != 0)) {
        FUN_004830f0(iVar20);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar17 = iVar17 + 1;
    } while (iVar17 < (int)*pdVar1);
  }
  bVar26 = DAT_005c619c != 0;
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x50 = 0;
  if ((bVar26) && (puStack_120 = (uint *)0x0, 0 < (int)in_stack_00000004[9])) {
    puVar19 = in_stack_00000004 + 10;
    puStack_128 = (uint *)0x0;
    do {
      puVar8 = auStack_118;
      for (iVar17 = 0x1f; iVar17 != 0; iVar17 = iVar17 + -1) {
        *puVar8 = 0;
        puVar8 = puVar8 + 1;
      }
      piVar15 = (int *)((int)puVar19 + (int)puStack_128 + *puVar19 + 0x70);
      puVar8 = (undefined4 *)((int)puVar19 + (int)puStack_128 + *puVar19);
      puVar9 = auStack_118;
      for (iVar17 = 0x1b; iVar17 != 0; iVar17 = iVar17 + -1) {
        *puVar9 = *puVar8;
        puVar8 = puVar8 + 1;
        puVar9 = puVar9 + 1;
      }
      auStack_118[0] = 0x7c;
      iVar17 = *piVar15;
      if (iVar17 == 0) {
        puStack_124 = (uint *)0x0;
      }
      else {
        puStack_124 = (uint *)((int)piVar15 + iVar17);
      }
      puVar22 = auStack_98;
      for (iVar17 = 0x26; iVar17 != 0; iVar17 = iVar17 + -1) {
        *puVar22 = 0;
        puVar22 = puVar22 + 1;
      }
      iVar17 = meth_0x40bb30(this,auStack_98);
      if (iVar17 != 0) {
        puVar10 = (uint *)FUN_00482fb0(0x98);
        puVar22 = auStack_98;
        for (iVar17 = 0x26; iVar17 != 0; iVar17 = iVar17 + -1) {
          *puVar10 = *puVar22;
          puVar22 = puVar22 + 1;
          puVar10 = puVar10 + 1;
        }
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x4c);
      }
      puStack_120 = (uint *)((int)puStack_120 + 1);
      puStack_128 = (uint *)((int)puStack_128 + 0x78);
    } while ((int)puStack_120 < (int)in_stack_00000004[9]);
  }
  pdVar1 = &(this->cls_0x5a486c).mbr_0x34;
  iVar17 = 0;
  if (0 < (int)(this->cls_0x5a486c).mbr_0x34) {
    do {
      if ((-1 < iVar17) &&
         (iVar20 = *(int *)((this->cls_0x5a486c).mbr_0x44 + iVar17 * 4), iVar20 != 0)) {
        FUN_004830f0(iVar20);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar17 = iVar17 + 1;
    } while (iVar17 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x38 = 0;
  puStack_120 = (uint *)0x0;
  if (0 < (int)in_stack_00000004[7]) {
    puStack_124 = (uint *)0x0;
    do {
      puVar8 = (undefined4 *)
               ((byte *)(in_stack_00000004[8] + (int)(in_stack_00000004 + 8)) + (int)puStack_124);
      puVar9 = auStack_118;
      for (iVar17 = 0x14; iVar17 != 0; iVar17 = iVar17 + -1) {
        *puVar9 = *puVar8;
        puVar8 = puVar8 + 1;
        puVar9 = puVar9 + 1;
      }
      iVar17 = (**(code **)(*DAT_00668eb0 + 0x14))(DAT_00668eb0,auStack_c8,0);
      if (iVar17 != 0) {
        FUN_004a90d0(iVar17,s_d__revenant_3DImage_cpp_005c5ee8,0x9ff);
      }
      piStack_dc = (int *)0x0;
      (**(code **)(*piStack_d4 + 0xc))(piStack_d4,&puStack_124);
      (**(code **)(*piStack_dc + 0x14))(piStack_dc,DAT_00668f14,&iStack_d8);
      puVar9 = (undefined4 *)FUN_00482fb0(0x5c);
      puVar8 = auStack_118;
      for (iVar17 = 0x17; iVar17 != 0; iVar17 = iVar17 + -1) {
        *puVar9 = *puVar8;
        puVar8 = puVar8 + 1;
        puVar9 = puVar9 + 1;
      }
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)pdVar1);
      puStack_120 = (uint *)((int)puStack_120 + 1);
      puStack_124 = puStack_124 + 0x14;
    } while ((int)puStack_120 < (int)in_stack_00000004[7]);
  }
  *(undefined4 *)&(this->cls_0x5a486c).field_0x9c = 0;
  if (((this->cls_0x5a486c).mbr_0x1c & 2) != 0) {
    puStack_120 = (uint *)0x0;
    uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    if (CONCAT31(extraout_var_22,uVar5) != 0 && -1 < extraout_var_22) {
      puStack_128 = (uint *)0x0;
      puStack_124 = (uint *)0x0;
      do {
        if (in_stack_00000004[1] < 2) {
          puVar19 = puStack_128;
          if (in_stack_00000004[2] != 0) {
            puVar19 = (uint *)((byte *)((int)in_stack_00000004 + in_stack_00000004[2] + 8) +
                              (int)puStack_128);
          }
        }
        else if (in_stack_00000004[1] == 2) {
          puVar19 = puStack_124;
          if (in_stack_00000004[2] != 0) {
            puVar19 = (uint *)((int)puStack_124 +
                              (int)((int)in_stack_00000004 + in_stack_00000004[2] + 8));
          }
        }
        else {
          puVar19 = (uint *)((byte *)((int)(in_stack_00000004 + 2) + (int)puStack_128) +
                            in_stack_00000004[2]);
        }
        if (0 < (int)*puVar19) {
          if (*(int *)&(this->cls_0x5a486c).field_0x9c == 0) {
            uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
            uVar6 = FUN_00482fb0(CONCAT31(extraout_var_23,uVar5) << 3);
            *(undefined4 *)&(this->cls_0x5a486c).field_0x9c = uVar6;
          }
          uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
          puVar8 = *(undefined4 **)&(this->cls_0x5a486c).field_0x9c;
          for (uVar16 = (uint)(CONCAT31(extraout_var_24,uVar5) << 3) >> 2; uVar16 != 0;
              uVar16 = uVar16 - 1) {
            *puVar8 = 0;
            puVar8 = puVar8 + 1;
          }
          for (iVar17 = 0; iVar17 != 0; iVar17 = iVar17 + -1) {
            *(undefined *)puVar8 = 0;
            puVar8 = (undefined4 *)((int)puVar8 + 1);
          }
          puVar8 = (undefined4 *)FUN_00482fb0(*puVar19);
          if ((puVar19[1] == 0) || ((byte *)((int)puVar19 + puVar19[1] + 4) == (byte *)0x0)) {
            if ((puVar19[2] != 0) && ((byte *)((int)puVar19 + puVar19[2] + 8) != (byte *)0x0)) {
              if (puVar19[2] == 0) {
                puVar9 = (undefined4 *)0x0;
              }
              else {
                puVar9 = (undefined4 *)((int)puVar19 + puVar19[2] + 8);
              }
              uVar16 = *puVar19;
              puVar12 = puVar8;
              for (uVar18 = uVar16 >> 2; uVar18 != 0; uVar18 = uVar18 - 1) {
                *puVar12 = *puVar9;
                puVar9 = puVar9 + 1;
                puVar12 = puVar12 + 1;
              }
              iVar17 = *(int *)&(this->cls_0x5a486c).field_0x9c;
              for (uVar16 = uVar16 & 3; uVar16 != 0; uVar16 = uVar16 - 1) {
                *(undefined *)puVar12 = *(undefined *)puVar9;
                puVar9 = (undefined4 *)((int)puVar9 + 1);
                puVar12 = (undefined4 *)((int)puVar12 + 1);
              }
              iVar20 = 0;
              *(undefined4 **)(iVar17 + 4 + (int)puStack_120 * 8) = puVar8;
              if (0 < (int)puVar8[3]) {
                piVar15 = puVar8 + 0xe;
                do {
                  if (*piVar15 == 0) {
                    puVar11 = (undefined *)0x0;
                  }
                  else {
                    puVar11 = (undefined *)(*piVar15 + (int)piVar15);
                  }
                  FUN_004b8820(puVar11);
                  iVar20 = iVar20 + 1;
                  piVar15 = piVar15 + 10;
                } while (iVar20 < (int)puVar8[3]);
              }
            }
          }
          else {
            if (puVar19[1] == 0) {
              puVar9 = (undefined4 *)0x0;
            }
            else {
              puVar9 = (undefined4 *)((int)puVar19 + puVar19[1] + 4);
            }
            uVar16 = *puVar19;
            puVar12 = puVar8;
            for (uVar18 = uVar16 >> 2; uVar18 != 0; uVar18 = uVar18 - 1) {
              *puVar12 = *puVar9;
              puVar9 = puVar9 + 1;
              puVar12 = puVar12 + 1;
            }
            iVar17 = *(int *)&(this->cls_0x5a486c).field_0x9c;
            for (uVar16 = uVar16 & 3; uVar16 != 0; uVar16 = uVar16 - 1) {
              *(undefined *)puVar12 = *(undefined *)puVar9;
              puVar9 = (undefined4 *)((int)puVar9 + 1);
              puVar12 = (undefined4 *)((int)puVar12 + 1);
            }
            *(undefined4 **)(iVar17 + (int)puStack_120 * 8) = puVar8;
            FUN_004b8820(puVar8);
          }
        }
        puStack_124 = puStack_124 + 0x14;
        iVar17 = (int)puStack_120 + 1;
        puStack_128 = puStack_128 + 4;
        puStack_120 = (uint *)iVar17;
        uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      } while (iVar17 < CONCAT31(extraout_var_25,uVar5));
    }
  }
  pdVar1 = &(this->cls_0x5a486c).mbr_0x7c;
  iVar17 = 0;
  if (0 < (int)(this->cls_0x5a486c).mbr_0x7c) {
    do {
      if ((-1 < iVar17) &&
         (iVar20 = *(int *)((this->cls_0x5a486c).mbr_0x8c + iVar17 * 4), iVar20 != 0)) {
        if (*(int *)(iVar20 + 8) != 0) {
          FUN_00482f80(*(int *)(iVar20 + 8));
        }
        if (*(int *)(iVar20 + 0xc) != 0) {
          FUN_00482f80(*(int *)(iVar20 + 0xc));
        }
        FUN_004830f0(iVar20);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar17 = iVar17 + 1;
    } while (iVar17 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x80 = 0;
  uVar16 = in_stack_00000004[1];
  if ((uVar16 < 2) && ((*(byte *)in_stack_00000004 & 0x20) != 0)) {
    if (in_stack_00000004[0xe] == 0) {
      puVar8 = (undefined4 *)0x0;
    }
    else {
      puVar8 = (undefined4 *)((int)in_stack_00000004 + in_stack_00000004[0xe] + 0x38);
    }
    puStack_124 = (uint *)0x0;
    if (0 < (int)in_stack_00000004[0xd]) {
      puVar9 = puVar8 + 3;
      do {
        puVar12 = (undefined4 *)FUN_00482fb0(0x10);
        if (puVar12 != (undefined4 *)0x0) {
          uVar6 = puVar9[-2];
          *puVar12 = *puVar8;
          puVar12[1] = uVar6;
          uVar6 = FUN_0059b6bc(&DAT_005c5d20);
          puVar12[2] = uVar6;
          uVar6 = FUN_0059b6bc(puVar9);
          uVar6 = FUN_0059bd3e(uVar6);
          puVar12[3] = uVar6;
        }
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x7c);
        puStack_124 = (uint *)((int)puStack_124 + 1);
        puVar8 = puVar8 + 0xb;
        puVar9 = puVar9 + 0xb;
      } while ((int)puStack_124 < (int)in_stack_00000004[0xd]);
    }
  }
  else if (uVar16 == 2) {
    if (in_stack_00000004[0xe] == 0) {
      puStack_11c = (uint *)0x0;
    }
    else {
      puStack_11c = (uint *)((int)in_stack_00000004 + in_stack_00000004[0xe] + 0x38);
    }
    puStack_124 = (uint *)0x0;
    puStack_120 = (uint *)0x0;
    uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    if (CONCAT31(extraout_var_26,uVar5) != 0 && -1 < extraout_var_26) {
      puStack_128 = (uint *)0x0;
      do {
        if (in_stack_00000004[2] == 0) {
          pbVar21 = (byte *)0x0;
        }
        else {
          pbVar21 = (byte *)((int)in_stack_00000004 + in_stack_00000004[2] + 8);
        }
        if (pbVar21[(int)puStack_128 + 0x10] != 0) {
          piVar15 = (int *)FUN_00482fb0(0x10);
          if (piVar15 != (int *)0x0) {
            *piVar15 = (int)puStack_120;
            piVar15[1] = 0;
            iVar17 = FUN_0059b6bc(&PTR_null_005c5d28);
            piVar15[2] = iVar17;
            uVar6 = FUN_0059b6bc(pbVar21 + (int)puStack_128 + 0x10);
            iVar17 = FUN_0059bd3e(uVar6);
            piVar15[3] = iVar17;
          }
          cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x7c);
        }
        if (pbVar21[(int)puStack_128 + 0x30] != 0) {
          piVar15 = (int *)FUN_00482fb0(0x10);
          if (piVar15 != (int *)0x0) {
            *piVar15 = (int)puStack_120;
            piVar15[1] = 0;
            iVar17 = FUN_0059b6bc(&PTR_null_005c5d2c);
            piVar15[2] = iVar17;
            uVar6 = FUN_0059b6bc(pbVar21 + (int)puStack_128 + 0x30);
            iVar17 = FUN_0059bd3e(uVar6);
            piVar15[3] = iVar17;
          }
          cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x7c);
        }
        if (((*(byte *)in_stack_00000004 & 0x20) != 0) &&
           ((int)puStack_124 < (int)in_stack_00000004[0xd])) {
          piVar15 = (int *)(puStack_11c + 3);
          do {
            if ((uint *)*puStack_11c != puStack_120) break;
            piVar13 = (int *)FUN_00482fb0(0x10);
            if (piVar13 != (int *)0x0) {
              piVar13[1] = piVar15[-2];
              *piVar13 = *puStack_11c;
              iVar17 = FUN_0059b6bc(&DAT_005c5d30);
              piVar13[2] = iVar17;
              uVar6 = FUN_0059b6bc(piVar15);
              iVar17 = FUN_0059bd3e(uVar6);
              piVar13[3] = iVar17;
            }
            cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x7c);
            puStack_11c = puStack_11c + 0xb;
            piVar15 = piVar15 + 0xb;
            puStack_124 = (uint *)((int)puStack_124 + 1);
          } while ((int)puStack_124 < (int)in_stack_00000004[0xd]);
        }
        puStack_128 = (uint *)((int)puStack_128 + 0x50);
        iVar17 = (int)puStack_120 + 1;
        puStack_120 = (uint *)iVar17;
        uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      } while (iVar17 < CONCAT31(extraout_var_27,uVar5));
    }
  }
  else if (2 < uVar16) {
    if (in_stack_00000004[0xe] == 0) {
      puVar8 = (undefined4 *)0x0;
    }
    else {
      puVar8 = (undefined4 *)((int)in_stack_00000004 + in_stack_00000004[0xe] + 0x38);
    }
    puStack_120 = (uint *)0x0;
    if (0 < (int)in_stack_00000004[0xd]) {
      do {
        puVar9 = (undefined4 *)FUN_00482fb0(0x10);
        if (puVar9 != (undefined4 *)0x0) {
          if (puVar8[3] == 0) {
            pbVar21 = (byte *)0x0;
          }
          else {
            pbVar21 = (byte *)(puVar8[3] + 0xc + (int)puVar8);
          }
          if (puVar8[2] == 0) {
            pbVar14 = (byte *)0x0;
          }
          else {
            pbVar14 = (byte *)(puVar8[2] + 8 + (int)puVar8);
          }
          uVar6 = puVar8[1];
          *puVar9 = *puVar8;
          puVar9[1] = uVar6;
          uVar6 = FUN_0059b6bc(pbVar14);
          puVar9[2] = uVar6;
          uVar6 = FUN_0059b6bc(pbVar21);
          uVar6 = FUN_0059bd3e(uVar6);
          puVar9[3] = uVar6;
        }
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x7c);
        puStack_120 = (uint *)((int)puStack_120 + 1);
        puVar8 = puVar8 + 4;
      } while ((int)puStack_120 < (int)in_stack_00000004[0xd]);
    }
  }
  iVar17 = 0;
  puStack_120 = (uint *)0x0;
  if (0 < (int)(this->cls_0x5a486c).mbr_0x7c) {
    do {
      dVar7 = *(dword *)((this->cls_0x5a486c).mbr_0x8c + iVar17);
      if (dVar7 == 0) {
        dVar7 = (this->cls_0x5a486c).mbr_0x90;
      }
      iVar20 = 5;
      bVar26 = true;
      pcVar23 = *(char **)(dVar7 + 8);
      pcVar25 = &DAT_005c5d38;
      do {
        if (iVar20 == 0) break;
        iVar20 = iVar20 + -1;
        bVar26 = *pcVar23 == *pcVar25;
        pcVar23 = pcVar23 + 1;
        pcVar25 = pcVar25 + 1;
      } while (bVar26);
      if (bVar26) {
        dVar7 = *(dword *)((this->cls_0x5a486c).mbr_0x8c + iVar17);
        if (dVar7 == 0) {
          dVar7 = (this->cls_0x5a486c).mbr_0x90;
        }
        iVar20 = FUN_004833a0(*(undefined4 *)(dVar7 + 0xc));
        iVar24 = 0;
        if (0 < iVar20) {
          do {
            dVar7 = *(dword *)((this->cls_0x5a486c).mbr_0x8c + iVar17);
            if (dVar7 == 0) {
              dVar7 = (this->cls_0x5a486c).mbr_0x90;
            }
            FUN_00483330(*(undefined4 *)(dVar7 + 0xc),iVar24,0,0);
            cls_0x41c7d0::meth_0x49b880((cls_0x41c7d0 *)&DAT_00667548);
            iVar24 = iVar24 + 1;
          } while (iVar24 < iVar20);
        }
      }
      puStack_120 = (uint *)((int)puStack_120 + 1);
      iVar17 = iVar17 + 4;
    } while ((int)puStack_120 < (int)(this->cls_0x5a486c).mbr_0x7c);
  }
  cls_0x5a486c::meth_0x447d80(&this->cls_0x5a486c);
  DAT_005e8300 = DAT_005e8300 + 1;
  return 1;
}



// Function at 004085f0

void __thiscall OOAnalyzer::cls_0x5a35ac::meth_0x4085f0(cls_0x5a35ac *this)

{
  dword *pdVar1;
  int iVar2;
  undefined uVar3;
  int iVar4;
  int3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  int3 extraout_var_03;
  undefined4 *puVar5;
  undefined3 extraout_var_04;
  int iVar6;
  int iVar7;
  int iStack_4;
  
  if ((this->cls_0x5a486c).mbr_0xc != 0) {
    pdVar1 = &(this->cls_0x5a486c).mbr_0x64;
    iVar6 = 0;
    if (0 < (int)(this->cls_0x5a486c).mbr_0x64) {
      do {
        meth_0x408ff0(this);
        iVar6 = iVar6 + 1;
      } while (iVar6 < (int)*pdVar1);
    }
    iVar6 = 0;
    if (0 < (int)*pdVar1) {
      do {
        if ((-1 < iVar6) &&
           (iVar4 = *(int *)((this->cls_0x5a486c).mbr_0x74 + iVar6 * 4), iVar4 != 0)) {
          FUN_004830f0(iVar4);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
        iVar6 = iVar6 + 1;
      } while (iVar6 < (int)*pdVar1);
    }
    *pdVar1 = 0;
    *(undefined4 *)&(this->cls_0x5a486c).field_0x68 = 0;
    pdVar1 = &(this->cls_0x5a486c).mbr_0x4c;
    iVar6 = 0;
    if (0 < (int)(this->cls_0x5a486c).mbr_0x4c) {
      do {
        meth_0x40c190(this);
        iVar6 = iVar6 + 1;
      } while (iVar6 < (int)*pdVar1);
    }
    iVar6 = 0;
    if (0 < (int)*pdVar1) {
      do {
        if ((-1 < iVar6) &&
           (iVar4 = *(int *)((this->cls_0x5a486c).mbr_0x5c + iVar6 * 4), iVar4 != 0)) {
          FUN_004830f0(iVar4);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
        iVar6 = iVar6 + 1;
      } while (iVar6 < (int)*pdVar1);
    }
    *pdVar1 = 0;
    *(undefined4 *)&(this->cls_0x5a486c).field_0x50 = 0;
    pdVar1 = &(this->cls_0x5a486c).mbr_0x34;
    iVar6 = 0;
    if (0 < (int)(this->cls_0x5a486c).mbr_0x34) {
      do {
        iVar4 = cls_0x45f7c0::meth_0x410160((cls_0x45f7c0 *)pdVar1);
        if ((iVar4 != 0) &&
           (iVar4 = cls_0x45f7c0::meth_0x410f60((cls_0x45f7c0 *)pdVar1), *(int *)(iVar4 + 0x50) != 0
           )) {
          iVar4 = cls_0x45f7c0::meth_0x410f60((cls_0x45f7c0 *)pdVar1);
          if (*(int *)(iVar4 + 0x50) != 0) {
            iVar4 = cls_0x45f7c0::meth_0x410f60((cls_0x45f7c0 *)pdVar1);
            (**(code **)(**(int **)(iVar4 + 0x50) + 8))(*(int **)(iVar4 + 0x50));
            iVar4 = cls_0x45f7c0::meth_0x410f60((cls_0x45f7c0 *)pdVar1);
            *(undefined4 *)(iVar4 + 0x50) = 0;
          }
          iVar4 = cls_0x45f7c0::meth_0x410f60((cls_0x45f7c0 *)pdVar1);
          *(undefined4 *)(iVar4 + 0x50) = 0;
          iVar4 = cls_0x45f7c0::meth_0x410f60((cls_0x45f7c0 *)pdVar1);
          *(undefined4 *)(iVar4 + 0x54) = 0;
          cls_0x45f7c0::meth_0x411280((cls_0x45f7c0 *)pdVar1);
        }
        iVar6 = iVar6 + 1;
      } while (iVar6 < (int)*pdVar1);
    }
    cls_0x45f7c0::meth_0x4112b0((cls_0x45f7c0 *)pdVar1);
    iVar6 = 0;
    uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    if (CONCAT31(extraout_var,uVar3) != 0 && -1 < extraout_var) {
      do {
        iVar4 = *(int *)(*(int *)&(this->cls_0x5a486c).field_0x98 + iVar6 * 4);
        if (iVar4 != 0) {
          FUN_004830f0(iVar4);
        }
        iVar6 = iVar6 + 1;
        uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      } while (iVar6 < CONCAT31(extraout_var_00,uVar3));
    }
    FUN_004830f0(*(undefined4 *)&(this->cls_0x5a486c).field_0x98);
    *(undefined4 *)&(this->cls_0x5a486c).field_0x98 = 0;
    if (((this->cls_0x5a486c).mbr_0x1c & 1) == 0) {
      iStack_4 = 1;
    }
    else {
      uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      iStack_4 = CONCAT31(extraout_var_01,uVar3);
    }
    iVar6 = 0;
    if (0 < iStack_4) {
      do {
        if (((this->cls_0x5a486c).mbr_0x1c & 1) == 0) {
          iVar4 = 1;
        }
        else {
          uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
          iVar4 = CONCAT31(extraout_var_02,uVar3);
        }
        iVar7 = 0;
        if (0 < iVar4) {
          do {
            FUN_004830f0(*(undefined4 *)
                          (*(int *)((this->cls_0x5a486c).mbr_0x28 + iVar6 * 4) + iVar7 * 4));
            iVar7 = iVar7 + 1;
          } while (iVar7 < iVar4);
        }
        FUN_004830f0(*(undefined4 *)((this->cls_0x5a486c).mbr_0x28 + iVar6 * 4));
        iVar6 = iVar6 + 1;
      } while (iVar6 < iStack_4);
    }
    iVar6 = 0;
    FUN_004830f0((this->cls_0x5a486c).mbr_0x28);
    (this->cls_0x5a486c).mbr_0x28 = 0;
    (this->cls_0x5a486c).mbr_0x24 = 0;
    FUN_004830f0((this->cls_0x5a486c).mbr_0x30);
    (this->cls_0x5a486c).mbr_0x30 = 0;
    if (*(int *)&(this->cls_0x5a486c).field_0x9c != 0) {
      iVar4 = 0;
      uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      if (CONCAT31(extraout_var_03,uVar3) != 0 && -1 < extraout_var_03) {
        do {
          iVar7 = *(int *)&(this->cls_0x5a486c).field_0x9c;
          iVar2 = *(int *)(iVar7 + iVar4 * 8);
          puVar5 = (undefined4 *)(iVar7 + iVar4 * 8);
          if (iVar2 != 0) {
            FUN_004830f0(iVar2);
            puVar5 = (undefined4 *)(*(int *)&(this->cls_0x5a486c).field_0x9c + iVar4 * 8);
            *puVar5 = 0;
          }
          iVar7 = puVar5[1];
          if (iVar7 != 0) {
            FUN_00418600();
            FUN_004830f0(iVar7);
            *(undefined4 *)(*(int *)&(this->cls_0x5a486c).field_0x9c + 4 + iVar4 * 8) = 0;
          }
          iVar4 = iVar4 + 1;
          uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        } while (iVar4 < CONCAT31(extraout_var_04,uVar3));
      }
      FUN_004830f0(*(undefined4 *)&(this->cls_0x5a486c).field_0x9c);
    }
    pdVar1 = &(this->cls_0x5a486c).mbr_0x7c;
    *(undefined4 *)&(this->cls_0x5a486c).field_0x9c = 0;
    if (0 < (int)(this->cls_0x5a486c).mbr_0x7c) {
      do {
        if ((-1 < iVar6) &&
           (iVar4 = *(int *)((this->cls_0x5a486c).mbr_0x8c + iVar6 * 4), iVar4 != 0)) {
          if (*(int *)(iVar4 + 8) != 0) {
            FUN_00482f80(*(int *)(iVar4 + 8));
          }
          if (*(int *)(iVar4 + 0xc) != 0) {
            FUN_00482f80(*(int *)(iVar4 + 0xc));
          }
          FUN_004830f0(iVar4);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
        iVar6 = iVar6 + 1;
      } while (iVar6 < (int)*pdVar1);
    }
    DAT_005e8300 = DAT_005e8300 + -1;
    *pdVar1 = 0;
    *(undefined4 *)&(this->cls_0x5a486c).field_0x80 = 0;
    (this->cls_0x5a486c).mbr_0xc = 0;
  }
  return;
}



// Function at 004088e0

undefined4 __thiscall OOAnalyzer::cls_0x5a35ac::virt_meth_0x4088e0(cls_0x5a35ac *this)

{
  dword dVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = 0;
  if (0 < (int)(this->cls_0x5a486c).mbr_0x4c) {
    do {
      dVar1 = (this->cls_0x5a486c).mbr_0x5c;
      if (((dVar1 != 0) && (uVar3 < (this->cls_0x5a486c).mbr_0x4c)) &&
         (*(int *)(dVar1 + uVar3 * 4) != 0)) {
        dVar1 = *(dword *)((this->cls_0x5a486c).mbr_0x5c + uVar3 * 4);
        if (dVar1 == 0) {
          dVar1 = (this->cls_0x5a486c).mbr_0x60;
        }
        if (*(int *)(dVar1 + 0x90) != 0) {
          dVar1 = *(dword *)((this->cls_0x5a486c).mbr_0x5c + uVar3 * 4);
          if (dVar1 == 0) {
            dVar1 = (this->cls_0x5a486c).mbr_0x60;
          }
          iVar2 = (**(code **)(**(int **)(dVar1 + 0x90) + 0x60))(*(int **)(dVar1 + 0x90));
          if (iVar2 == -0x7789fe3e) {
            meth_0x4085f0(this);
            if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
              cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
            }
            iVar2 = meth_0x407510(this);
            if (iVar2 != 0) {
              return 1;
            }
            return 0;
          }
        }
      }
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < (int)(this->cls_0x5a486c).mbr_0x4c);
  }
  return 1;
}



// Function at 00408980

void __thiscall OOAnalyzer::cls_0x5a35ac::virt_meth_0x408980(cls_0x5a35ac *this)

{
  uint uVar1;
  
  if (((this->cls_0x5a486c).mbr_0xc == 0) && (DAT_006680c0 != 0)) {
    uVar1 = FUN_004a8170();
    if (0x200000 < uVar1) {
      if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
        cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
      }
      meth_0x407510(this);
    }
  }
  return;
}



// Function at 00408ff0

void __thiscall OOAnalyzer::cls_0x5a35ac::meth_0x408ff0(cls_0x5a35ac *this)

{
  uint uVar1;
  int iVar2;
  undefined uVar3;
  int3 extraout_var;
  undefined3 extraout_var_00;
  dword dVar4;
  int iVar5;
  uint in_stack_00000004;
  
  if ((this->cls_0x5a486c).mbr_0xc == 0) {
    if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
      cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
    }
    meth_0x407510(this);
  }
  if (-1 < (int)in_stack_00000004) {
    uVar1 = (this->cls_0x5a486c).mbr_0x64;
    if (((((int)in_stack_00000004 <= (int)uVar1) &&
         (dVar4 = (this->cls_0x5a486c).mbr_0x74, dVar4 != 0)) && (in_stack_00000004 < uVar1)) &&
       (*(int *)(dVar4 + in_stack_00000004 * 4) != 0)) {
      dVar4 = *(dword *)(dVar4 + in_stack_00000004 * 4);
      if (dVar4 == 0) {
        dVar4 = (this->cls_0x5a486c).mbr_0x78;
      }
      FUN_004830f0(*(undefined4 *)(dVar4 + 0x38));
      FUN_004830f0(*(undefined4 *)(dVar4 + 0x34));
      FUN_004830f0(*(undefined4 *)(dVar4 + 0x20));
      iVar5 = 0;
      uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      if (CONCAT31(extraout_var,uVar3) != 0 && -1 < extraout_var) {
        do {
          iVar2 = *(int *)(*(int *)(dVar4 + 0x44) + iVar5 * 4);
          if (iVar2 != 0) {
            FUN_004830f0(iVar2);
          }
          iVar5 = iVar5 + 1;
          uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        } while (iVar5 < CONCAT31(extraout_var_00,uVar3));
      }
      FUN_004830f0(*(undefined4 *)(dVar4 + 0x44));
      FUN_004830f0(*(undefined4 *)(dVar4 + 0x40));
      iVar5 = *(int *)((this->cls_0x5a486c).mbr_0x74 + in_stack_00000004 * 4);
      if (iVar5 != 0) {
        FUN_004830f0(iVar5);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x64);
    }
  }
  return;
}



// Function at 0040bb30

undefined4 __thiscall OOAnalyzer::cls_0x5a35ac::meth_0x40bb30(cls_0x5a35ac *this,uint *param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int *unaff_EBX;
  uint *puVar4;
  undefined4 *unaff_EDI;
  uint *puVar5;
  undefined4 *puVar6;
  undefined4 unaff_retaddr;
  uint *in_stack_00000008;
  uint in_stack_00000010;
  uint in_stack_00000018;
  int *piVar7;
  undefined4 uVar8;
  int *piVar9;
  int *piStack_11c;
  int iStack_118;
  uint uStack_114;
  uint uStack_110;
  int iStack_10c;
  int *piStack_108;
  int iStack_104;
  undefined4 uStack_100;
  undefined4 uStack_fc;
  uint auStack_f8 [4];
  uint uStack_e8;
  uint uStack_c8;
  uint uStack_ac;
  int iStack_a4;
  undefined4 uStack_90;
  int iStack_84;
  undefined4 uStack_80;
  undefined4 auStack_7c [4];
  undefined4 uStack_6c;
  uint uStack_48;
  uint uStack_1c;
  uint uStack_18;
  undefined4 uStack_c;
  int iStack_8;
  
  if (in_stack_00000008[3] != in_stack_00000008[2]) {
    FUN_00481d10(s__s_has_non_square_texture_005c5f00,(this->cls_0x5a486c).mbr_0x4);
  }
  uVar1 = in_stack_00000008[3];
  if (((((uVar1 != 0x80) && (uVar1 != 0x40)) && (uVar1 != 0x100)) &&
      ((uVar1 != 0x20 && (uVar1 != 0x200)))) && ((uVar1 != 0x10 && (uVar1 != 8)))) {
    FUN_00481d10(s__s_texture_is_not_a_power_of_two_005c5f1c,(this->cls_0x5a486c).mbr_0x4);
  }
  puVar4 = in_stack_00000008;
  puVar5 = auStack_f8;
  for (iVar2 = 0x1f; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar5 = *puVar4;
    puVar4 = puVar4 + 1;
    puVar5 = puVar5 + 1;
  }
  cls_0x4164c0::meth_0x4164c0((cls_0x4164c0 *)((int)&DAT_0065a578 + 4),in_stack_00000008);
  iVar2 = 0;
  if ((uStack_ac & 0x30) == 0) {
    if ((uStack_ac & 0x1808) != 0) {
      iVar2 = 1 << ((byte)iStack_a4 & 0x1f);
    }
  }
  else {
    iVar2 = 0x100;
  }
  in_stack_00000008[4] = in_stack_00000008[3];
  auStack_f8[1] = 0x100f;
  uStack_90 = 0x1000;
  if (DAT_005e88a4 < auStack_f8[3]) {
    do {
      auStack_f8[3] = auStack_f8[3] >> 1;
    } while (DAT_005e88a4 < auStack_f8[3]);
  }
  if (DAT_005e88a4 < auStack_f8[2]) {
    do {
      auStack_f8[2] = auStack_f8[2] >> 1;
    } while (DAT_005e88a4 < auStack_f8[2]);
  }
  uStack_e8 = auStack_f8[3];
  puVar4 = auStack_f8;
  puVar5 = param_1;
  for (iVar3 = 0x1f; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar5 = *puVar4;
    puVar4 = puVar4 + 1;
    puVar5 = puVar5 + 1;
  }
  if (iVar2 < 1) {
    iStack_104 = 0;
  }
  else {
    iStack_104 = FUN_00482fb0(iVar2 << 2);
  }
  iVar2 = uStack_e8 * auStack_f8[2];
  iVar3 = FUN_004a8170();
  if ((int)(iVar3 - ((uint)(iVar2 * iStack_a4) >> 3) * in_stack_00000010) < 0x19000) {
    in_stack_00000018 = 1;
  }
  if ((int)in_stack_00000010 < 2) {
    in_stack_00000018 = 0;
  }
  param_1[0x23] = in_stack_00000018;
  param_1[0x1f] = in_stack_00000010;
  param_1[0x20] = 0;
  if ((int)in_stack_00000010 < 2) {
    param_1[0x21] = 0;
    param_1[0x22] = 0;
  }
  else {
    uVar1 = FUN_00482fb0(in_stack_00000010 * 4);
    param_1[0x21] = uVar1;
    uVar1 = FUN_00482fb0(in_stack_00000010 * 4);
    param_1[0x22] = uVar1;
  }
  param_1[0x24] = 0;
  param_1[0x25] = 0;
  piStack_11c = (int *)0x0;
  piStack_108 = (int *)0x0;
  uStack_fc = 0;
  uStack_114 = 0;
  uStack_110 = 0;
  uStack_100 = 0;
  iStack_10c = 0;
  if (0 < (int)in_stack_00000010) {
    do {
      if (piStack_11c == (int *)0x0) {
        uStack_90 = 0x1800;
        (**(code **)(*DAT_006695ac + 0x18))(DAT_006695ac,auStack_f8,&piStack_11c,0);
        if (piStack_11c == (int *)0x0) {
          return 0;
        }
      }
      puVar6 = auStack_7c;
      for (iVar2 = 0x1f; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar6 = 0;
        puVar6 = puVar6 + 1;
      }
      piVar9 = (int *)0x0;
      puVar6 = auStack_7c;
      uVar8 = 0x801;
      auStack_7c[0] = 0x7c;
      iVar2 = (**(code **)(*piStack_11c + 100))(piStack_11c,0,puVar6,0x801);
      if (iVar2 != 0) {
        FUN_004a90d0(iVar2,s_d__revenant_3DImage_cpp_005c5f48,0xabe);
      }
      iVar3 = iStack_118;
      iVar2 = *(int *)(iStack_8 + (int)unaff_EBX * 4);
      if (iVar2 == 0) {
        iVar2 = 0;
      }
      else {
        iVar2 = iStack_8 + (int)unaff_EBX * 4 + iVar2;
      }
      cls_0x4164c0::meth_0x416790
                ((cls_0x4164c0 *)((int)&DAT_0065a578 + 4),uStack_c,iVar2,unaff_retaddr,&iStack_10c,
                 uStack_6c);
      iVar2 = (**(code **)(*piVar9 + 0x80))(piVar9,0);
      if (iVar2 != 0) {
        FUN_004a90d0(iVar2,s_d__revenant_3DImage_cpp_005c5f60,0xac1);
      }
      if (iVar3 != 0) {
        if ((uStack_c8 & 0x30) == 0) {
          if ((uStack_c8 & 8) == 0) {
            if ((uStack_c8 & 0x1000) == 0) {
              if ((uStack_c8 & 0x800) != 0) {
                uVar8 = 0x100;
              }
            }
            else {
              uVar8 = 0x200;
            }
          }
          else {
            uVar8 = 1;
          }
        }
        else {
          uVar8 = 0x44;
        }
        piVar9 = (int *)0x0;
        iVar2 = (**(code **)(*DAT_006695ac + 0x14))(DAT_006695ac,uVar8,iVar3,&iStack_118);
        if (iVar2 != 0) {
          FUN_004a90d0(iVar2,s_d__revenant_3DImage_cpp_005c5f78,0xace);
        }
        iVar2 = (**(code **)(*piVar9 + 0x7c))(piVar9,unaff_EDI);
        if (iVar2 != 0) {
          FUN_004a90d0(iVar2,s_d__revenant_3DImage_cpp_005c5f90,0xacf);
        }
      }
      iVar2 = (**(code **)*puVar6)(puVar6,&DAT_005a61f8,&stack0xfffffedc);
      if (iVar2 != 0) {
        FUN_004a90d0(iVar2,s_d__revenant_3DImage_cpp_005c5fa8,0xad2);
      }
      if (uStack_114 == 0) {
        puVar6 = auStack_7c;
        for (iVar2 = 0x1f; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar6 = 0;
          puVar6 = puVar6 + 1;
        }
        auStack_7c[0] = 0x7c;
        piVar9 = piStack_11c;
        iVar2 = (**(code **)(*piStack_11c + 0x58))(piStack_11c,auStack_7c);
        if (iVar2 != 0) {
          FUN_004a90d0(iVar2,s_d__revenant_3DImage_cpp_005c5fc0,0xada);
        }
        uStack_80 = 0x1007;
        uStack_1c = 0x4001000;
        iVar2 = FUN_004a8370();
        if (iVar2 == 0) {
          uStack_1c = uStack_1c | 0x800;
        }
        else {
          uStack_1c = uStack_1c | 0x10004000;
          uStack_18 = uStack_18 | 8;
        }
        piVar7 = &iStack_84;
        (**(code **)(*DAT_006695ac + 0x18))(DAT_006695ac,piVar7,&piStack_11c,0);
        if (unaff_EDI == (undefined4 *)0x0) {
          return 0;
        }
        if ((uStack_48 & 0x30) == 0) {
          if ((uStack_48 & 8) != 0) {
            uVar8 = 1;
            goto LAB_0040bfbd;
          }
          if ((uStack_48 & 0x1000) != 0) {
            uVar8 = 0x200;
            goto LAB_0040bfbd;
          }
          if ((uStack_48 & 0x800) != 0) {
            uVar8 = 0x100;
            goto LAB_0040bfbd;
          }
        }
        else {
          uVar8 = 0x44;
LAB_0040bfbd:
          iVar2 = (**(code **)(*DAT_006695ac + 0x14))(DAT_006695ac,uVar8,iVar3,&iStack_118,0);
          if (iVar2 != 0) {
            FUN_004a90d0(iVar2,s_d__revenant_3DImage_cpp_005c5fd8,0xaf4);
          }
          iVar2 = (**(code **)(*piVar7 + 0x7c))(piVar7,unaff_EDI);
          if (iVar2 != 0) {
            FUN_004a90d0(iVar2,s_d__revenant_3DImage_cpp_005c5ff0,0xaf5);
          }
        }
        iVar2 = (**(code **)*unaff_EDI)(unaff_EDI,&DAT_005a61f8,&stack0xfffffed8);
        if (iVar2 != 0) {
          FUN_004a90d0(iVar2,s_d__revenant_3DImage_cpp_005c6008,0xaf8);
        }
        iVar2 = (**(code **)(*piVar9 + 0x14))(piVar9,unaff_EDI);
        if (iVar2 != 0) {
          return 0;
        }
      }
      if ((int)in_stack_00000010 < 2) {
        param_1[0x24] = uStack_114;
        param_1[0x25] = uStack_110;
      }
      else if ((in_stack_00000018 == 0) || (param_1[0x21] == 0)) {
        puVar4 = (uint *)param_1[0x21];
        if (puVar4 != (uint *)0x0) {
          puVar5 = (uint *)param_1[0x22];
          puVar4[iStack_10c] = uStack_114;
          puVar5[iStack_10c] = uStack_110;
          uStack_114 = 0;
          uStack_110 = 0;
          uVar1 = *puVar5;
          param_1[0x24] = *puVar4;
          param_1[0x25] = uVar1;
        }
      }
      else {
        uVar1 = param_1[0x22];
        *(int **)(param_1[0x21] + iStack_10c * 4) = piStack_11c;
        *(int **)(uVar1 + iStack_10c * 4) = piStack_108;
        piStack_11c = (int *)0x0;
        piStack_108 = (int *)0x0;
        param_1[0x24] = uStack_114;
        param_1[0x25] = uStack_110;
      }
      iStack_10c = iStack_10c + 1;
    } while (iStack_10c < (int)in_stack_00000010);
    if (piStack_108 != (int *)0x0) {
      (**(code **)(*piStack_108 + 8))(piStack_108);
      (**(code **)(*unaff_EBX + 8))(unaff_EBX);
    }
  }
  if (iStack_104 != 0) {
    FUN_004830f0(iStack_104);
  }
  return 1;
}



// Function at 0040c190

void __thiscall OOAnalyzer::cls_0x5a35ac::meth_0x40c190(cls_0x5a35ac *this)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  dword dVar5;
  int in_stack_00000004;
  
  iVar4 = 0;
  dVar5 = *(dword *)((this->cls_0x5a486c).mbr_0x5c + in_stack_00000004 * 4);
  if (dVar5 == 0) {
    dVar5 = (this->cls_0x5a486c).mbr_0x60;
  }
  FUN_004172b0(0,0,0);
  FUN_004172b0(1,0,0);
  if ((*(int *)(dVar5 + 0x7c) < 2) || (*(int *)(dVar5 + 0x8c) != 0)) {
    iVar2 = (**(code **)(**(int **)(dVar5 + 0x94) + 4))(*(int **)(dVar5 + 0x94));
    while (iVar2 = iVar2 + -1, iVar2 != 0) {
      (**(code **)(**(int **)(dVar5 + 0x94) + 8))(*(int **)(dVar5 + 0x94));
    }
    for (iVar2 = (**(code **)(**(int **)(dVar5 + 0x90) + 4))(*(int **)(dVar5 + 0x90)); iVar2 != 0;
        iVar2 = iVar2 + -1) {
      (**(code **)(**(int **)(dVar5 + 0x90) + 8))(*(int **)(dVar5 + 0x90));
    }
  }
  *(undefined4 *)(dVar5 + 0x94) = 0;
  *(undefined4 *)(dVar5 + 0x90) = 0;
  if (*(int *)(dVar5 + 0x84) != 0) {
    if (0 < *(int *)(dVar5 + 0x7c)) {
      iVar2 = 0;
      do {
        piVar1 = *(int **)(iVar2 + *(int *)(dVar5 + 0x88));
        iVar3 = (**(code **)(*piVar1 + 4))(piVar1);
        while (iVar3 = iVar3 + -1, iVar3 != 0) {
          piVar1 = *(int **)(iVar2 + *(int *)(dVar5 + 0x88));
          (**(code **)(*piVar1 + 8))(piVar1);
        }
        piVar1 = *(int **)(*(int *)(dVar5 + 0x84) + iVar2);
        for (iVar3 = (**(code **)(*piVar1 + 4))(piVar1); iVar3 != 0; iVar3 = iVar3 + -1) {
          piVar1 = *(int **)(*(int *)(dVar5 + 0x84) + iVar2);
          (**(code **)(*piVar1 + 8))(piVar1);
        }
        iVar4 = iVar4 + 1;
        iVar2 = iVar2 + 4;
      } while (iVar4 < *(int *)(dVar5 + 0x7c));
    }
    FUN_004830f0(*(undefined4 *)(dVar5 + 0x84));
    FUN_004830f0(*(undefined4 *)(dVar5 + 0x88));
    *(undefined4 *)(dVar5 + 0x84) = 0;
    *(undefined4 *)(dVar5 + 0x88) = 0;
  }
  *(undefined4 *)(dVar5 + 0x7c) = 0;
  *(undefined4 *)(dVar5 + 0x80) = 0;
  if ((-1 < in_stack_00000004) &&
     (iVar4 = *(int *)((this->cls_0x5a486c).mbr_0x5c + in_stack_00000004 * 4), iVar4 != 0)) {
    FUN_004830f0(iVar4);
  }
  cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x4c);
  return;
}



// Function at 0040cc40

undefined4 __thiscall
OOAnalyzer::cls_0x5a35ac::meth_0x40cc40
          (cls_0x5a35ac *this,uint param_1,uint param_2,int *param_3,uint *param_4,uint *param_5,
          uint *param_6,uint *param_7)

{
  int iVar1;
  uint uVar2;
  uint *in_stack_00000020;
  
  iVar1 = *(int *)((this->cls_0x5a486c).mbr_0x4 + 0x54);
  if (((param_1 < *(uint *)(iVar1 + 4)) &&
      (param_2 < (uint)(int)*(short *)(iVar1 + 0x32 + param_1 * 0x4c))) &&
     (iVar1 = *(int *)(*(int *)&(this->cls_0x5a486c).field_0x98 + param_1 * 4), iVar1 != 0)) {
    uVar2 = *(uint *)(iVar1 + param_2 * 8);
    *param_3 = (uVar2 & 0xffff) << 8;
    *param_4 = (int)uVar2 >> 8 & 0xffffff00;
    uVar2 = *(uint *)(iVar1 + 4 + param_2 * 8);
    *param_5 = uVar2 & 0xff;
    *param_6 = uVar2 >> 8 & 0xff;
    *param_7 = uVar2 >> 0x10 & 0xff;
    *in_stack_00000020 = uVar2 >> 0x18;
    return 1;
  }
  *in_stack_00000020 = 0;
  *param_7 = 0;
  *param_6 = 0;
  *param_5 = 0;
  *param_4 = 0;
  *param_3 = 0;
  return 0;
}



// Function at 0040ce60

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __thiscall OOAnalyzer::cls_0x5a35ac::virt_meth_0x40ce60(cls_0x5a35ac *this)

{
  dword dVar1;
  int *piVar2;
  undefined uVar3;
  int iVar4;
  undefined3 extraout_var;
  int in_stack_00000004;
  
  dVar1 = (this->cls_0x5a486c).mbr_0x4;
  iVar4 = *(int *)(dVar1 + 0x74);
  if (iVar4 == 0) {
    if ((this->cls_0x5a486c).mbr_0xc == 0) {
      if (*(int *)(dVar1 + 0x60) == 0) {
        cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
      }
      iVar4 = meth_0x407510(this);
      if (iVar4 != 0) goto LAB_0040ce95;
    }
    else {
LAB_0040ce95:
      if (((*(int *)&(this->cls_0x5a486c).field_0x9c != 0) && (-1 < in_stack_00000004)) &&
         (uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this),
         in_stack_00000004 <= CONCAT31(extraout_var,uVar3))) {
        piVar2 = *(int **)&(this->cls_0x5a486c).field_0x9c;
        iVar4 = piVar2[in_stack_00000004 * 2];
        if ((iVar4 == 0) && (iVar4 = *piVar2, iVar4 == 0)) {
          return 0;
        }
        goto LAB_0040cec5;
      }
    }
    iVar4 = 0;
  }
  else {
LAB_0040cec5:
    if (((*(byte *)(iVar4 + 0x10) & 6) != 0) && (*(int *)(iVar4 + 0x18) == 0)) {
      *(undefined4 *)(iVar4 + 0x18) = _DAT_006668d0;
      return iVar4;
    }
  }
  return iVar4;
}



// Function at 0040cef0

undefined4 __thiscall OOAnalyzer::cls_0x5a35ac::virt_meth_0x40cef0(cls_0x5a35ac *this)

{
  undefined uVar1;
  int iVar2;
  undefined3 extraout_var;
  int in_stack_00000004;
  
  if ((this->cls_0x5a486c).mbr_0xc == 0) {
    if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
      cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
    }
    iVar2 = meth_0x407510(this);
    if (iVar2 == 0) {
      return 0;
    }
  }
  if (((*(int *)&(this->cls_0x5a486c).field_0x9c != 0) && (-1 < in_stack_00000004)) &&
     (uVar1 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this),
     in_stack_00000004 <= CONCAT31(extraout_var,uVar1))) {
    return *(undefined4 *)(*(int *)&(this->cls_0x5a486c).field_0x9c + 4 + in_stack_00000004 * 8);
  }
  return 0;
}



// Function at 0040cf50

void __thiscall OOAnalyzer::cls_0x5a35ac::virt_meth_0x40cf50(cls_0x5a35ac *this)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  int *piVar6;
  int in_stack_00000004;
  
  if ((this->cls_0x5a486c).mbr_0xc == 0) {
    if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
      cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
    }
    iVar3 = meth_0x407510(this);
    if (iVar3 == 0) {
      FUN_00481c10(s_Unable_to_initialize_3D_imagery_f_005c6020,
                   *(undefined4 *)(in_stack_00000004 + 0x38));
    }
  }
  iVar3 = 0;
  uVar1 = **(undefined4 **)(in_stack_00000004 + 0x4c);
  if (0 < DAT_005e872c) {
    piVar6 = &DAT_005e851c;
    do {
      iVar4 = FUN_0059a530_stricmp(uVar1,*(undefined4 *)(*piVar6 + 4));
      if (iVar4 == 0) {
        puVar5 = (undefined4 *)(&DAT_005e851c)[iVar3];
        if (puVar5 != &DAT_005e8508) goto LAB_0040d076;
        break;
      }
      iVar3 = iVar3 + 1;
      piVar6 = piVar6 + 1;
    } while (iVar3 < DAT_005e872c);
  }
  uVar2 = *(undefined4 *)(*(int *)(in_stack_00000004 + 0x48) + 4);
  iVar3 = FUN_0059a530_stricmp(uVar2,s_weapon_005c6048);
  if ((((iVar3 == 0) || (iVar3 = FUN_0059a530_stricmp(uVar2,s_armor_005c6050), iVar3 == 0)) ||
      (iVar3 = FUN_0059a530_stricmp(uVar2,s_lightsource_005c6058), iVar3 == 0)) ||
     ((iVar3 = FUN_0059a530_stricmp(uVar2,&DAT_005c6064), iVar3 == 0 &&
      (iVar3 = FUN_0059a530_stricmp(uVar1,s_arrow_005c606c), iVar3 == 0)))) {
    iVar3 = 0;
    if (0 < DAT_005e872c) {
      piVar6 = &DAT_005e851c;
      do {
        iVar4 = FUN_0059a530_stricmp(s_equip_005c6074,*(undefined4 *)(*piVar6 + 4));
        if (iVar4 == 0) {
LAB_0040d0bc:
          (***(code ***)(&DAT_005e851c)[iVar3])(in_stack_00000004);
          return;
        }
        iVar3 = iVar3 + 1;
        piVar6 = piVar6 + 1;
      } while (iVar3 < DAT_005e872c);
    }
  }
  else {
    iVar3 = 0;
    if (0 < DAT_005e872c) {
      piVar6 = &DAT_005e851c;
      do {
        iVar4 = FUN_0059a530_stricmp(uVar2,*(undefined4 *)(*piVar6 + 4));
        if (iVar4 == 0) goto LAB_0040d0bc;
        iVar3 = iVar3 + 1;
        piVar6 = piVar6 + 1;
      } while (iVar3 < DAT_005e872c);
    }
  }
  puVar5 = &DAT_005e8508;
LAB_0040d076:
  (**(code **)*puVar5)(in_stack_00000004);
  return;
}



// Function at 00410130

cls_0x5a35ac * __thiscall OOAnalyzer::cls_0x5a35ac::virt_meth_0x410130(cls_0x5a35ac *this)

{
  byte in_stack_00000004;
  
  ~cls_0x5a35ac(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004109d0

void __thiscall OOAnalyzer::cls_0x5a35ac::meth_0x4109d0(cls_0x5a35ac *this)

{
  if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
    cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
  }
  return;
}



// Function at 004191b0

void __thiscall OOAnalyzer::cls_0x5a35ac::meth_0x4191b0(cls_0x5a35ac *this)

{
  dword dVar1;
  undefined4 *puVar2;
  undefined uVar3;
  int3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar4;
  int iVar5;
  int unaff_ESI;
  int iVar6;
  int unaff_EDI;
  
  if ((this->cls_0x5a486c).mbr_0x10 != 0) {
    uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    iVar5 = CONCAT31(extraout_var,uVar3);
    if (iVar5 != 0 && -1 < extraout_var) {
      iVar6 = 0;
      if (iVar5 != 0 && -1 < extraout_var) {
        iVar4 = 0;
        do {
          if (*(int *)(iVar4 + (this->cls_0x5a486c).mbr_0x10) != 0) {
            uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            if (CONCAT31(extraout_var_00,uVar3) < 2) {
              iVar5 = 1;
            }
            else {
              uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
              iVar5 = CONCAT31(extraout_var_01,uVar3);
            }
            iVar6 = 0;
            if (0 < iVar5) {
              do {
                dVar1 = (this->cls_0x5a486c).mbr_0x10;
                if ((*(int *)(*(int *)(*(int *)(iVar4 + dVar1) + 8) + iVar6 * 4) != 0) &&
                   (puVar2 = *(undefined4 **)(*(int *)(*(int *)(iVar4 + dVar1) + 8) + iVar6 * 4),
                   puVar2 != (undefined4 *)0x0)) {
                  (**(code **)*puVar2)(1);
                }
                iVar6 = iVar6 + 1;
              } while (iVar6 < iVar5);
            }
            FUN_004830f0(*(undefined4 *)(*(int *)(iVar4 + (this->cls_0x5a486c).mbr_0x10) + 8));
            FUN_004830f0(*(undefined4 *)(iVar4 + (this->cls_0x5a486c).mbr_0x10));
            iVar5 = unaff_EDI;
            iVar6 = unaff_ESI;
          }
          iVar6 = iVar6 + 1;
          iVar4 = iVar4 + 4;
        } while (iVar6 < iVar5);
      }
      FUN_004830f0((this->cls_0x5a486c).mbr_0x10);
      (this->cls_0x5a486c).mbr_0x10 = 0;
    }
  }
  return;
}



// Function at 00446260

void __thiscall OOAnalyzer::cls_0x5a35ac::meth_0x446260(cls_0x5a35ac *this)

{
  (this->cls_0x5a486c).vftptr_0x0 = &cls_0x5a486c__vftable_5a486c_005a486c;
  cls_0x5a486c::meth_0x447d80(&this->cls_0x5a486c);
  return;
}



