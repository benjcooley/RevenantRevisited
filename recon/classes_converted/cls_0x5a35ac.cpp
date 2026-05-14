#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x5a35ac



// Function at 00406580

cls_0x5a35ac::cls_0x5a35ac()

{
  dword *pdVar1;
  dword *this_00;
  LPCVOID pvVar2;
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
  puVar3 = FUN_00482fb0(0x5c);
  (this->cls_0x5a486c).mbr_0x48 = (dword)puVar3;
  for (iVar4 = 0x17; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  this_00 = &(this->cls_0x5a486c).mbr_0x4c;
  local_4._0_1_ = 2;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)this_00,4);
  local_4._0_1_ = 3;
  puVar3 = FUN_00482fb0(0x98);
  (this->cls_0x5a486c).mbr_0x60 = (dword)puVar3;
  for (iVar4 = 0x26; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  local_4._0_1_ = 4;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x5a486c).mbr_0x64,0x10);
  local_4._0_1_ = 5;
  puVar3 = FUN_00482fb0(0x4c);
  (this->cls_0x5a486c).mbr_0x78 = (dword)puVar3;
  for (iVar4 = 0x13; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  local_4._0_1_ = 6;
  cls_0x41c7f0::cls_0x41c7f0((cls_0x41c7f0 *)&(this->cls_0x5a486c).mbr_0x7c,0x10);
  local_4._0_1_ = 7;
  puVar3 = FUN_00482fb0(0x10);
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
      if ((-1 < iVar4) &&
         (pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x44 + iVar4 * 4), pvVar2 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar2);
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
      if ((-1 < iVar4) &&
         (pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x5c + iVar4 * 4), pvVar2 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar2);
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
      if ((-1 < iVar4) &&
         (pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x74 + iVar4 * 4), pvVar2 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar2);
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
      if ((-1 < iVar4) &&
         (pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x8c + iVar4 * 4), pvVar2 != (LPCVOID)0x0))
      {
        FUN_00411240(pvVar2,1);
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

cls_0x5a35ac * __thiscall cls_0x5a35ac::~cls_0x5a35ac(cls_0x5a35ac *this)

{
  dword *pdVar1;
  LPCVOID pvVar2;
  cls_0x5a35ac *pcVar3;
  int iVar4;
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
  iVar4 = 0;
  local_4 = CONCAT31(local_4._1_3_,5);
  if (0 < (int)*pdVar1) {
    do {
      cls_0x41c7f0::meth_0x411330((cls_0x41c7f0 *)pdVar1);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*pdVar1);
  }
  pvVar2 = (LPCVOID)(this->cls_0x5a486c).mbr_0x90;
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x80 = 0;
  if (pvVar2 != (LPCVOID)0x0) {
    FUN_004109a0((int)pvVar2);
    FUN_004830f0(pvVar2);
  }
  local_4._0_1_ = 3;
  FUN_004830f0((LPCVOID)(this->cls_0x5a486c).mbr_0x8c);
  pdVar1 = &(this->cls_0x5a486c).mbr_0x64;
  iVar4 = 0;
  local_4._0_1_ = 6;
  if (0 < (int)*pdVar1) {
    do {
      if ((-1 < iVar4) &&
         (pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x74 + iVar4 * 4), pvVar2 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x68 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x5a486c).mbr_0x78);
  local_4._0_1_ = 2;
  FUN_004830f0((LPCVOID)(this->cls_0x5a486c).mbr_0x74);
  pdVar1 = &(this->cls_0x5a486c).mbr_0x4c;
  iVar4 = 0;
  local_4._0_1_ = 7;
  if (0 < (int)*pdVar1) {
    do {
      if ((-1 < iVar4) &&
         (pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x5c + iVar4 * 4), pvVar2 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x50 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x5a486c).mbr_0x60);
  local_4._0_1_ = 1;
  FUN_004830f0((LPCVOID)(this->cls_0x5a486c).mbr_0x5c);
  pdVar1 = &(this->cls_0x5a486c).mbr_0x34;
  iVar4 = 0;
  local_4._0_1_ = 8;
  if (0 < (int)*pdVar1) {
    do {
      if ((-1 < iVar4) &&
         (pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x44 + iVar4 * 4), pvVar2 != (LPCVOID)0x0))
      {
        FUN_004830f0(pvVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x38 = 0;
  FUN_004830f0((LPCVOID)(this->cls_0x5a486c).mbr_0x48);
  local_4 = (uint)local_4._1_3_ << 8;
  FUN_004830f0((LPCVOID)(this->cls_0x5a486c).mbr_0x44);
  local_4 = 0xffffffff;
  pcVar3 = (cls_0x5a35ac *)meth_0x446260(this);
  ExceptionList = pvStack_c;
  return pcVar3;
}



// Function at 00406a50

undefined4 cls_0x5a35ac::meth_0x406a50()

{
  dword *pdVar1;
  cls_0x5a486c__vftable_5a486c *pcVar2;
  dword dVar3;
  LPCVOID pvVar4;
  undefined uVar5;
  undefined3 extraout_var;
  undefined4 *puVar6;
  undefined3 extraout_var_00;
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
  undefined3 extraout_var_24;
  byte *pbVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  int iVar14;
  int iVar15;
  uint *puVar16;
  int unaff_EDI;
  uint *puVar17;
  bool bVar18;
  uint *in_stack_00000004;
  int iStack_138;
  uint *local_134;
  uint *puStack_130;
  int iStack_12c;
  uint *puStack_128;
  uint *apuStack_120 [2];
  int iStack_118;
  uint auStack_114 [8];
  undefined4 *puStack_f4;
  uint uStack_f0;
  uint uStack_ec;
  int iStack_e8;
  uint uStack_e4;
  undefined4 *puStack_e0;
  undefined4 *puStack_dc;
  int *piStack_d8;
  undefined4 *puStack_d4;
  int *piStack_d0;
  undefined auStack_c4 [44];
  undefined4 auStack_98 [38];
  
  if ((this->cls_0x5a486c).mbr_0xc != 0) {
    return 1;
  }
  iVar14 = 1;
  (this->cls_0x5a486c).mbr_0xc = 1;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x20 = 0;
  uVar11 = *in_stack_00000004;
  uVar13 = in_stack_00000004[1];
  *(uint *)&(this->cls_0x5a486c).mbr_0x1c = uVar11;
  (this->cls_0x5a486c).mbr_0x24 = uVar13;
  if ((uVar11 & 1) != 0) {
    uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    iVar14 = CONCAT31(extraout_var,uVar5);
  }
  puVar6 = FUN_00482fb0(iVar14 * 4);
  (this->cls_0x5a486c).mbr_0x28 = (dword)puVar6;
  iStack_138 = 0;
  if (0 < iVar14) {
    puVar16 = in_stack_00000004 + 2;
    do {
      if (((this->cls_0x5a486c).mbr_0x1c & 1) == 0) {
        iStack_12c = 1;
        puStack_130 = puVar16;
      }
      else {
        uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
        puStack_130 = (uint *)CONCAT31(extraout_var_00,uVar5);
        unaff_EDI = iStack_138;
      }
      puVar6 = FUN_00482fb0(iStack_12c * 4);
      iVar15 = 0;
      *(undefined4 **)((this->cls_0x5a486c).mbr_0x28 + iStack_138 * 4) = puVar6;
      if (0 < iStack_12c) {
        do {
          puVar6 = FUN_00482fb0((this->cls_0x5a486c).mbr_0x24 << 5);
          uVar11 = *puVar16;
          *(undefined4 **)(*(int *)((this->cls_0x5a486c).mbr_0x28 + iStack_138 * 4) + iVar15 * 4) =
               puVar6;
          if (uVar11 == 0) {
            pbVar10 = (byte *)0x0;
          }
          else {
            pbVar10 = (byte *)(uVar11 + (int)puVar16);
          }
          if (*(int *)(pbVar10 + iVar15 * 4) == 0) {
            puVar8 = (undefined4 *)0x0;
          }
          else {
            puVar8 = (undefined4 *)(pbVar10 + *(int *)(pbVar10 + iVar15 * 4) + iVar15 * 4);
          }
          for (uVar11 = ((this->cls_0x5a486c).mbr_0x24 << 5) >> 2; uVar11 != 0; uVar11 = uVar11 - 1)
          {
            *puVar6 = *puVar8;
            puVar8 = puVar8 + 1;
            puVar6 = puVar6 + 1;
          }
          iVar15 = iVar15 + 1;
          for (iVar12 = 0; iVar12 != 0; iVar12 = iVar12 + -1) {
            *(undefined *)puVar6 = *(undefined *)puVar8;
            puVar8 = (undefined4 *)((int)puVar8 + 1);
            puVar6 = (undefined4 *)((int)puVar6 + 1);
          }
          puVar16 = puStack_130;
        } while (iVar15 < iStack_12c);
      }
      iStack_138 = iStack_138 + 1;
      puVar16 = puVar16 + 1;
    } while (iStack_138 < iVar14);
  }
  uVar11 = in_stack_00000004[0x42];
  (this->cls_0x5a486c).mbr_0x2c = uVar11;
  puVar6 = FUN_00482fb0(uVar11 * 6);
  uVar11 = in_stack_00000004[0x43];
  (this->cls_0x5a486c).mbr_0x30 = (dword)puVar6;
  if (uVar11 == 0) {
    puVar8 = (undefined4 *)0x0;
  }
  else {
    puVar8 = (undefined4 *)((int)in_stack_00000004 + uVar11 + 0x10c);
  }
  uVar11 = in_stack_00000004[0x42];
  for (uVar13 = uVar11 * 6 >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
    *puVar6 = *puVar8;
    puVar8 = puVar8 + 1;
    puVar6 = puVar6 + 1;
  }
  for (uVar11 = uVar11 * 6 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
    *(undefined *)puVar6 = *(undefined *)puVar8;
    puVar8 = (undefined4 *)((int)puVar8 + 1);
    puVar6 = (undefined4 *)((int)puVar6 + 1);
  }
  if (((int)in_stack_00000004[0x1d6] < 0x1389) && (0 < (int)in_stack_00000004[0x1d6])) {
    iStack_138 = 0;
    local_134 = in_stack_00000004 + 0x6f7;
    puStack_130 = in_stack_00000004 + 0x537;
    puVar16 = in_stack_00000004 + 0x417;
    puVar9 = in_stack_00000004 + 0x1d7;
    do {
      puVar7 = puVar9;
      puVar17 = auStack_114;
      for (iVar14 = 8; iVar14 != 0; iVar14 = iVar14 + -1) {
        *puVar17 = *puVar7;
        puVar7 = puVar7 + 1;
        puVar17 = puVar17 + 1;
      }
      apuStack_120[0] = puVar16;
      FUN_0059bd3e(auStack_114);
      uStack_f0 = (uint)*(ushort *)puVar16;
      piStack_d8 = (int *)(uint)*(ushort *)(puVar16 + -0x40);
      uStack_ec = (uint)*(ushort *)(puVar16 + -0x20);
      uStack_e4 = (uint)*(ushort *)(puStack_130 + -0x100);
      iStack_e8 = 0;
      puStack_dc = FUN_00482fb0(in_stack_00000004[0xe5] * 4 + 4);
      puStack_e0 = FUN_00482fb0(in_stack_00000004[0xe5] * 4 + 4);
      uVar11 = in_stack_00000004[0xe5];
      iVar14 = 0;
      puVar16 = puStack_130;
      if (0 < (int)(uVar11 + 1)) {
        do {
          iStack_e8 = iStack_e8 + (uint)*(ushort *)puVar16;
          puStack_dc[iVar14] = (uint)*(ushort *)puVar16;
          puStack_e0[iVar14] = *(ushort *)(puVar16 + -0x100) - uStack_e4;
          iVar14 = iVar14 + 1;
          puVar16 = (uint *)((int)puVar16 + 2);
        } while (iVar14 < (int)(uVar11 + 1));
      }
      uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      puStack_f4 = FUN_00482fb0(CONCAT31(extraout_var_01,uVar5) << 2);
      if ((*(byte *)in_stack_00000004 & 4) == 0) {
        iVar14 = 0;
        uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        if (CONCAT31(extraout_var_04,uVar5) != 0 && -1 < extraout_var_04) {
          do {
            pcVar2 = (this->cls_0x5a486c).vftptr_0x0;
            puStack_f4[iVar14] = 0xffffffff;
            iVar14 = iVar14 + 1;
            uVar5 = (*pcVar2->virt_meth_0x4101f0_60)(this);
          } while (iVar14 < CONCAT31(extraout_var_05,uVar5));
        }
      }
      else {
        if (*local_134 == 0) {
          pbVar10 = (byte *)0x0;
        }
        else {
          pbVar10 = (byte *)((int)local_134 + *local_134);
        }
        iVar14 = 0;
        uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        if (CONCAT31(extraout_var_02,uVar5) != 0 && -1 < extraout_var_02) {
          do {
            pcVar2 = (this->cls_0x5a486c).vftptr_0x0;
            puStack_f4[iVar14] = (uint)pbVar10[iVar14];
            iVar14 = iVar14 + 1;
            uVar5 = (*pcVar2->virt_meth_0x4101f0_60)(this);
          } while (iVar14 < CONCAT31(extraout_var_03,uVar5));
        }
      }
      uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      piStack_d0 = FUN_00482fb0(CONCAT31(extraout_var_06,uVar5) << 2);
      uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      puStack_d4 = FUN_00482fb0(CONCAT31(extraout_var_07,uVar5) << 2);
      iStack_12c = 0;
      uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      if (CONCAT31(extraout_var_08,uVar5) != 0 && -1 < extraout_var_08) {
        iStack_118 = -0x19dc - (int)in_stack_00000004;
        puVar16 = in_stack_00000004 + 0x677;
        do {
          iVar14 = iStack_118;
          if ((*puVar16 == 0) || ((byte *)(*puVar16 + (int)puVar16) == (byte *)0x0)) {
            *(undefined4 *)((byte *)(iStack_118 + (int)puVar16) + (int)puStack_d4) = 0;
            *(undefined4 *)((byte *)(iStack_118 + (int)puVar16) + (int)piStack_d0) = 0;
          }
          else {
            uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            puVar6 = FUN_00482fb0(CONCAT31(extraout_var_09,uVar5) << 3);
            pcVar2 = (this->cls_0x5a486c).vftptr_0x0;
            pbVar10 = (byte *)(iVar14 + (int)puVar16);
            *(undefined4 **)(pbVar10 + (int)puStack_d4) = puVar6;
            uVar5 = (*pcVar2->virt_meth_0x4104a0_144)(this);
            *(uint *)(pbVar10 + (int)puStack_dc) = CONCAT31(extraout_var_10,uVar5);
            uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            puStack_130 = (uint *)(CONCAT31(extraout_var_11,uVar5) << 3);
            uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            puVar6 = (undefined4 *)(iStack_12c + CONCAT31(extraout_var_12,uVar5) * unaff_EDI * 8);
            puVar8 = *(undefined4 **)(pbVar10 + (int)puStack_e0);
            for (uVar11 = (uint)local_134 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
              *puVar8 = *puVar6;
              puVar6 = puVar6 + 1;
              puVar8 = puVar8 + 1;
            }
            for (uVar11 = (uint)local_134 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
              *(undefined *)puVar8 = *(undefined *)puVar6;
              puVar6 = (undefined4 *)((int)puVar6 + 1);
              puVar8 = (undefined4 *)((int)puVar8 + 1);
            }
          }
          iStack_12c = iStack_12c + 1;
          puVar16 = puVar16 + 1;
          uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        } while (iStack_12c < CONCAT31(extraout_var_13,uVar5));
      }
      if ((this->cls_0x5a486c).mbr_0xc == 0) {
        if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
          cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
        }
        meth_0x407510(this);
      }
      puVar7 = FUN_00482fb0(0x4c);
      puVar16 = auStack_114;
      for (iVar14 = 0x13; iVar14 != 0; iVar14 = iVar14 + -1) {
        *puVar7 = *puVar16;
        puVar16 = puVar16 + 1;
        puVar7 = puVar7 + 1;
      }
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x64);
      puStack_130 = puStack_130 + 4;
      local_134 = local_134 + 1;
      iStack_138 = iStack_138 + 1;
      puVar16 = (uint *)((int)apuStack_120[0] + 2);
      puVar9 = puVar9 + 8;
    } while (iStack_138 < (int)in_stack_00000004[0x1d6]);
    apuStack_120[0] = puVar16;
    uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    puVar6 = FUN_00482fb0(CONCAT31(extraout_var_14,uVar5) << 2);
    *(undefined4 **)&(this->cls_0x5a486c).field_0x98 = puVar6;
    uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    puVar6 = *(undefined4 **)&(this->cls_0x5a486c).field_0x98;
    for (uVar11 = CONCAT31(extraout_var_15,uVar5) & 0x3fffffff; uVar11 != 0; uVar11 = uVar11 - 1) {
      *puVar6 = 0;
      puVar6 = puVar6 + 1;
    }
    for (iVar14 = 0; iVar14 != 0; iVar14 = iVar14 + -1) {
      *(undefined *)puVar6 = 0;
      puVar6 = (undefined4 *)((int)puVar6 + 1);
    }
    if (((this->cls_0x5a486c).mbr_0x1c & 1) == 0) {
      iStack_138 = 0;
      uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      if (CONCAT31(extraout_var_16,uVar5) != 0 && -1 < extraout_var_16) {
        local_134 = (uint *)0x0;
        puStack_130 = in_stack_00000004 + 0x637;
        do {
          if (*puStack_130 == 0) {
            dVar3 = (this->cls_0x5a486c).mbr_0x4;
            *(undefined4 *)
             (*(int *)&(this->cls_0x5a486c).field_0x98 + (-0x18dc - (int)in_stack_00000004) +
             (int)puStack_130) = 0;
            pbVar10 = (byte *)(*(int *)(dVar3 + 0x54) + 0x31 + (int)local_134);
            *pbVar10 = *pbVar10 | 0x20;
          }
          else {
            uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            puVar16 = FUN_00482fb0(CONCAT31(extraout_var_17,uVar5) << 3);
            *(uint **)(*(int *)&(this->cls_0x5a486c).field_0x98 + (-0x18dc - (int)in_stack_00000004)
                      + (int)puStack_130) = puVar16;
            if (*puStack_130 == 0) {
              puVar9 = (uint *)0x0;
            }
            else {
              puVar9 = (uint *)(*puStack_130 + (int)local_134);
            }
            iVar14 = 0;
            uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            if (CONCAT31(extraout_var_18,uVar5) != 0 && -1 < extraout_var_18) {
              do {
                uVar11 = *puVar9;
                puVar9 = puVar9 + 1;
                *puVar16 = uVar11 >> 8 & 0xffff;
                pcVar2 = (this->cls_0x5a486c).vftptr_0x0;
                puVar16[1] = uVar11 >> 0x18 ^ uVar11 & 0xff000000;
                iVar14 = iVar14 + 1;
                puVar16 = puVar16 + 2;
                uVar5 = (*pcVar2->virt_meth_0x4104a0_144)(this);
              } while (iVar14 < CONCAT31(extraout_var_19,uVar5));
            }
          }
          iStack_138 = iStack_138 + 1;
          local_134 = (uint *)((int)local_134 + 0x4c);
          puStack_130 = puStack_130 + 1;
          uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        } while (iStack_138 < CONCAT31(extraout_var_20,uVar5));
      }
    }
    pdVar1 = &(this->cls_0x5a486c).mbr_0x4c;
    (this->cls_0x5a486c).mbr_0x10 = (uint)(in_stack_00000004[0xe5] != 0);
    iVar14 = 0;
    if (0 < (int)*pdVar1) {
      do {
        if ((-1 < iVar14) &&
           (pvVar4 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x5c + iVar14 * 4),
           pvVar4 != (LPCVOID)0x0)) {
          FUN_004830f0(pvVar4);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
        iVar14 = iVar14 + 1;
      } while (iVar14 < (int)*pdVar1);
    }
    bVar18 = DAT_005c619c != 0;
    *pdVar1 = 0;
    *(undefined4 *)&(this->cls_0x5a486c).field_0x50 = 0;
    if ((bVar18) && (iStack_138 = 0, 0 < (int)in_stack_00000004[0xe5])) {
      local_134 = in_stack_00000004 + 0xe6;
      do {
        puVar16 = auStack_114;
        for (iVar14 = 0x1f; iVar14 != 0; iVar14 = iVar14 + -1) {
          *puVar16 = 0;
          puVar16 = puVar16 + 1;
        }
        puVar16 = local_134;
        puVar9 = auStack_114;
        for (iVar14 = 0x1b; iVar14 != 0; iVar14 = iVar14 + -1) {
          *puVar9 = *puVar16;
          puVar16 = puVar16 + 1;
          puVar9 = puVar9 + 1;
        }
        auStack_114[0] = 0x7c;
        puVar6 = auStack_98;
        for (iVar14 = 0x26; iVar14 != 0; iVar14 = iVar14 + -1) {
          *puVar6 = 0;
          puVar6 = puVar6 + 1;
        }
        iVar14 = meth_0x40bb30(this,auStack_98);
        if (iVar14 != 0) {
          puVar8 = FUN_00482fb0(0x98);
          puVar6 = auStack_98;
          for (iVar14 = 0x26; iVar14 != 0; iVar14 = iVar14 + -1) {
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
    iVar14 = 0;
    if (0 < (int)(this->cls_0x5a486c).mbr_0x34) {
      do {
        if ((-1 < iVar14) &&
           (pvVar4 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x44 + iVar14 * 4),
           pvVar4 != (LPCVOID)0x0)) {
          FUN_004830f0(pvVar4);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
        iVar14 = iVar14 + 1;
      } while (iVar14 < (int)*pdVar1);
    }
    uVar11 = in_stack_00000004[0x44];
    *pdVar1 = 0;
    *(undefined4 *)&(this->cls_0x5a486c).field_0x38 = 0;
    iStack_138 = 0;
    if (0 < (int)uVar11) {
      local_134 = in_stack_00000004 + 0x45;
      do {
        puVar16 = local_134;
        puVar9 = auStack_114;
        for (iVar14 = 0x14; iVar14 != 0; iVar14 = iVar14 + -1) {
          *puVar9 = *puVar16;
          puVar16 = puVar16 + 1;
          puVar9 = puVar9 + 1;
        }
        iVar14 = (**(code **)(*DAT_00668eb0 + 0x14))(DAT_00668eb0,auStack_c4,0);
        if (iVar14 != 0) {
          FUN_004a90d0(iVar14);
        }
        piStack_d8 = (int *)0x0;
        (**(code **)(*piStack_d0 + 0xc))(piStack_d0,apuStack_120);
        (**(code **)(*piStack_d8 + 0x14))(piStack_d8,DAT_00668f14,&puStack_d4);
        puVar9 = FUN_00482fb0(0x5c);
        puVar16 = auStack_114;
        for (iVar14 = 0x17; iVar14 != 0; iVar14 = iVar14 + -1) {
          *puVar9 = *puVar16;
          puVar16 = puVar16 + 1;
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
      uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      if (CONCAT31(extraout_var_21,uVar5) != 0 && -1 < extraout_var_21) {
        puStack_128 = in_stack_00000004 + 0x6b7;
        do {
          if (((*puStack_128 != 0) &&
              (puVar16 = (uint *)(*puStack_128 + (int)puStack_128), puVar16 != (uint *)0x0)) &&
             (0 < (int)*puVar16)) {
            if (*(int *)&(this->cls_0x5a486c).field_0x9c == 0) {
              uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
              puVar6 = FUN_00482fb0(CONCAT31(extraout_var_22,uVar5) << 3);
              *(undefined4 **)&(this->cls_0x5a486c).field_0x9c = puVar6;
            }
            uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
            puVar6 = *(undefined4 **)&(this->cls_0x5a486c).field_0x9c;
            for (uVar11 = (uint)(CONCAT31(extraout_var_23,uVar5) << 3) >> 2; uVar11 != 0;
                uVar11 = uVar11 - 1) {
              *puVar6 = 0;
              puVar6 = puVar6 + 1;
            }
            for (iVar14 = 0; iVar14 != 0; iVar14 = iVar14 + -1) {
              *(undefined *)puVar6 = 0;
              puVar6 = (undefined4 *)((int)puVar6 + 1);
            }
            puVar9 = FUN_00482fb0(*puVar16);
            if ((puVar16[1] == 0) || ((byte *)(puVar16[1] + 4 + (int)puVar16) == (byte *)0x0)) {
              if ((puVar16[2] != 0) && ((byte *)(puVar16[2] + 8 + (int)puVar16) != (byte *)0x0)) {
                if (puVar16[2] == 0) {
                  puVar7 = (uint *)0x0;
                }
                else {
                  puVar7 = (uint *)(puVar16[2] + 8 + (int)puVar16);
                }
                uVar11 = *puVar16;
                puVar16 = puVar9;
                for (uVar13 = uVar11 >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
                  *puVar16 = *puVar7;
                  puVar7 = puVar7 + 1;
                  puVar16 = puVar16 + 1;
                }
                iVar14 = *(int *)&(this->cls_0x5a486c).field_0x9c;
                for (uVar11 = uVar11 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
                  *(undefined *)puVar16 = *(undefined *)puVar7;
                  puVar7 = (uint *)((int)puVar7 + 1);
                  puVar16 = (uint *)((int)puVar16 + 1);
                }
                iVar15 = 0;
                *(uint **)(iVar14 + 4 + iStack_138 * 8) = puVar9;
                if (0 < (int)puVar9[3]) {
                  puVar16 = puVar9 + 0xe;
                  do {
                    if (*puVar16 == 0) {
                      puVar7 = (uint *)0x0;
                    }
                    else {
                      puVar7 = (uint *)(*puVar16 + (int)puVar16);
                    }
                    FUN_004b8820(puVar7);
                    iVar15 = iVar15 + 1;
                    puVar16 = puVar16 + 10;
                  } while (iVar15 < (int)puVar9[3]);
                }
              }
            }
            else {
              if (puVar16[1] == 0) {
                puVar7 = (uint *)0x0;
              }
              else {
                puVar7 = (uint *)(puVar16[1] + 4 + (int)puVar16);
              }
              uVar11 = *puVar16;
              puVar16 = puVar9;
              for (uVar13 = uVar11 >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
                *puVar16 = *puVar7;
                puVar7 = puVar7 + 1;
                puVar16 = puVar16 + 1;
              }
              iVar14 = *(int *)&(this->cls_0x5a486c).field_0x9c;
              for (uVar11 = uVar11 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
                *(undefined *)puVar16 = *(undefined *)puVar7;
                puVar7 = (uint *)((int)puVar7 + 1);
                puVar16 = (uint *)((int)puVar16 + 1);
              }
              *(uint **)(iVar14 + iStack_138 * 8) = puVar9;
              FUN_004b8820(puVar9);
            }
          }
          iStack_138 = iStack_138 + 1;
          puStack_128 = puStack_128 + 1;
          uVar5 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        } while (iStack_138 < CONCAT31(extraout_var_24,uVar5));
      }
    }
    pdVar1 = &(this->cls_0x5a486c).mbr_0x7c;
    iVar14 = 0;
    if (0 < (int)(this->cls_0x5a486c).mbr_0x7c) {
      do {
        if ((-1 < iVar14) &&
           (pvVar4 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x8c + iVar14 * 4),
           pvVar4 != (LPCVOID)0x0)) {
          if (*(LPCVOID *)((int)pvVar4 + 8) != (LPCVOID)0x0) {
            FUN_00482f80(*(LPCVOID *)((int)pvVar4 + 8));
          }
          if (*(LPCVOID *)((int)pvVar4 + 0xc) != (LPCVOID)0x0) {
            FUN_00482f80(*(LPCVOID *)((int)pvVar4 + 0xc));
          }
          FUN_004830f0(pvVar4);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
        iVar14 = iVar14 + 1;
      } while (iVar14 < (int)*pdVar1);
    }
    *pdVar1 = 0;
    *(undefined4 *)&(this->cls_0x5a486c).field_0x80 = 0;
    cls_0x5a486c::meth_0x447d80(&this->cls_0x5a486c);
    return 1;
  }
  return 0;
}



// Function at 00407510

undefined4 cls_0x5a35ac::meth_0x407510()

{
  dword *pdVar1;
  byte bVar2;
  uint uVar3;
  cls_0x5a486c__vftable_5a486c *pcVar4;
  LPCVOID pvVar5;
  undefined uVar6;
  undefined4 uVar7;
  undefined3 extraout_var;
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
  uint *puVar9;
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
  undefined3 extraout_var_25;
  undefined4 *puVar11;
  int3 extraout_var_26;
  undefined3 extraout_var_27;
  dword dVar12;
  int *piVar13;
  uint uVar14;
  int iVar15;
  uint uVar16;
  int iVar17;
  int unaff_EBX;
  uint *puVar18;
  int iVar19;
  byte *pbVar20;
  char *pcVar21;
  char *pcVar22;
  bool bVar23;
  uint *in_stack_00000004;
  uint *in_stack_00000008;
  uint *puStack_124;
  uint *puStack_120;
  uint *puStack_11c;
  uint *puStack_118;
  uint auStack_114 [8];
  undefined4 *puStack_f4;
  uint uStack_f0;
  uint uStack_ec;
  int iStack_e8;
  uint uStack_e4;
  undefined4 *puStack_e0;
  undefined4 *puStack_dc;
  int *piStack_d8;
  undefined4 *puStack_d4;
  int *piStack_d0;
  undefined auStack_c4 [48];
  uint auStack_94 [37];
  
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
    uVar7 = meth_0x406a50(this);
    return uVar7;
  }
  if (3 < in_stack_00000004[1]) {
    FUN_00481c10((byte *)(s__Newer_imagery_version_in_file___005c5cdf + 1));
  }
  uVar14 = *in_stack_00000004;
  uVar16 = in_stack_00000004[1];
  uVar3 = in_stack_00000004[3];
  puVar18 = (uint *)0x1;
  (this->cls_0x5a486c).mbr_0xc = 1;
  *(uint *)&(this->cls_0x5a486c).mbr_0x1c = uVar14;
  *(uint *)&(this->cls_0x5a486c).field_0x20 = uVar16;
  (this->cls_0x5a486c).mbr_0x24 = uVar3;
  if ((uVar14 & 1) != 0) {
    uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    puVar18 = (uint *)CONCAT31(extraout_var,uVar6);
  }
  puStack_120 = puVar18;
  puVar8 = FUN_00482fb0((int)puVar18 * 4);
  (this->cls_0x5a486c).mbr_0x28 = (dword)puVar8;
  puStack_11c = (uint *)0x0;
  if (0 < (int)puVar18) {
    do {
      iVar15 = (int)puStack_11c;
      if (((this->cls_0x5a486c).mbr_0x1c & 1) == 0) {
        puStack_124 = (uint *)0x1;
      }
      else {
        (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
      }
      puVar8 = FUN_00482fb0((int)puStack_124 * 4);
      iVar19 = 0;
      *(undefined4 **)((this->cls_0x5a486c).mbr_0x28 + iVar15 * 4) = puVar8;
      if (0 < (int)puStack_124) {
        puStack_118 = in_stack_00000004 + iVar15 + 4;
        do {
          puVar8 = FUN_00482fb0((this->cls_0x5a486c).mbr_0x24 << 5);
          uVar14 = in_stack_00000004[4];
          *(undefined4 **)(*(int *)((this->cls_0x5a486c).mbr_0x28 + iVar15 * 4) + iVar19 * 4) =
               puVar8;
          piVar13 = (int *)(iVar19 * 4 + *(int *)((int)puStack_118 + uVar14) +
                           (int)(int *)((int)puStack_118 + uVar14));
          iVar15 = *piVar13;
          if (iVar15 == 0) {
            puVar11 = (undefined4 *)0x0;
          }
          else {
            puVar11 = (undefined4 *)(iVar15 + (int)piVar13);
          }
          for (uVar14 = ((this->cls_0x5a486c).mbr_0x24 << 5) >> 2; uVar14 != 0; uVar14 = uVar14 - 1)
          {
            *puVar8 = *puVar11;
            puVar11 = puVar11 + 1;
            puVar8 = puVar8 + 1;
          }
          iVar19 = iVar19 + 1;
          for (iVar15 = 0; iVar15 != 0; iVar15 = iVar15 + -1) {
            *(undefined *)puVar8 = *(undefined *)puVar11;
            puVar11 = (undefined4 *)((int)puVar11 + 1);
            puVar8 = (undefined4 *)((int)puVar8 + 1);
          }
          iVar15 = (int)puStack_11c;
          in_stack_00000004 = in_stack_00000008;
        } while (iVar19 < (int)puStack_124);
      }
      puStack_11c = (uint *)(iVar15 + 1);
    } while ((int)puStack_11c < (int)puStack_120);
  }
  uVar14 = in_stack_00000004[5];
  (this->cls_0x5a486c).mbr_0x2c = uVar14;
  puVar8 = FUN_00482fb0(uVar14 * 6);
  uVar14 = in_stack_00000004[6];
  (this->cls_0x5a486c).mbr_0x30 = (dword)puVar8;
  if (uVar14 == 0) {
    puVar11 = (undefined4 *)0x0;
  }
  else {
    puVar11 = (undefined4 *)(uVar14 + 0x18 + (int)in_stack_00000004);
  }
  uVar14 = in_stack_00000008[5];
  for (uVar16 = uVar14 * 6 >> 2; uVar16 != 0; uVar16 = uVar16 - 1) {
    *puVar8 = *puVar11;
    puVar11 = puVar11 + 1;
    puVar8 = puVar8 + 1;
  }
  for (uVar14 = uVar14 * 6 & 3; uVar14 != 0; uVar14 = uVar14 - 1) {
    *(undefined *)puVar8 = *(undefined *)puVar11;
    puVar11 = (undefined4 *)((int)puVar11 + 1);
    puVar8 = (undefined4 *)((int)puVar8 + 1);
  }
  if ((5000 < (int)in_stack_00000008[0xb]) || ((int)in_stack_00000008[0xb] < 1)) {
    return 0;
  }
  puStack_11c = (uint *)0x0;
  puStack_118 = (uint *)0x0;
  puVar18 = in_stack_00000008 + 0xc;
  do {
    puVar10 = (uint *)((byte *)((int)puStack_118 + *puVar18) + (int)puVar18);
    puVar9 = auStack_114;
    for (iVar15 = 8; iVar15 != 0; iVar15 = iVar15 + -1) {
      *puVar9 = *puVar10;
      puVar10 = puVar10 + 1;
      puVar9 = puVar9 + 1;
    }
    FUN_0059bd3e(auStack_114);
    uVar14 = *puVar18;
    iStack_e8 = 0;
    piStack_d8 = (int *)(uint)*(ushort *)((byte *)((int)puStack_118 + (int)puVar18) + uVar14 + 0x20)
    ;
    uStack_f0 = (uint)*(ushort *)((byte *)((int)puStack_118 + (int)puVar18) + uVar14 + 0x24);
    uStack_ec = (uint)*(ushort *)((byte *)((int)puStack_118 + (int)puVar18) + uVar14 + 0x22);
    uStack_e4 = (uint)*(ushort *)
                       ((byte *)((int)puStack_118 + (int)puVar18) +
                       *(int *)((byte *)((int)puStack_118 + (int)puVar18) + uVar14 + 0x28) +
                       uVar14 + 0x28);
    puStack_dc = FUN_00482fb0(in_stack_00000008[9] * 4 + 4);
    puStack_e0 = FUN_00482fb0(in_stack_00000008[9] * 4 + 4);
    uVar14 = in_stack_00000008[9];
    if (0 < (int)(uVar14 + 1)) {
      puStack_120 = (uint *)((byte *)((int)puStack_118 + (int)puVar18) + *puVar18 + 0x28);
      iVar15 = 0;
      do {
        iVar19 = iVar15 * 4;
        iStack_e8 = iStack_e8 +
                    (uint)*(ushort *)
                           ((byte *)((int)puStack_118 + *puVar18 + 0x2a) + (int)puVar18 +
                           iVar19 + *(int *)((byte *)((int)puStack_118 + *puVar18 + 0x28) +
                                            (int)puVar18));
        piVar13 = (int *)((byte *)((int)puStack_118 + *puVar18 + 0x28) + (int)puVar18);
        puStack_dc[iVar15] = (uint)*(ushort *)((int)piVar13 + *piVar13 + iVar19 + 2);
        iVar17 = iVar15 + 1;
        puStack_e0[iVar15] = *(ushort *)((int)puStack_120 + *puStack_120 + iVar19) - uStack_e4;
        iVar15 = iVar17;
      } while (iVar17 < (int)(uVar14 + 1));
    }
    uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    puStack_f4 = FUN_00482fb0(CONCAT31(extraout_var_00,uVar6) << 2);
    if ((*(byte *)in_stack_00000008 & 4) == 0) {
      iVar15 = 0;
      uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      piVar13 = (int *)puStack_118;
      if (CONCAT31(extraout_var_03,uVar6) != 0 && -1 < extraout_var_03) {
        do {
          pcVar4 = (this->cls_0x5a486c).vftptr_0x0;
          puStack_f4[iVar15] = 0xffffffff;
          iVar15 = iVar15 + 1;
          uVar6 = (*pcVar4->virt_meth_0x4101f0_60)(this);
          piVar13 = (int *)puStack_118;
        } while (iVar15 < CONCAT31(extraout_var_04,uVar6));
      }
    }
    else {
      iVar15 = 0;
      uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      piVar13 = (int *)puStack_118;
      if (CONCAT31(extraout_var_01,uVar6) != 0 && -1 < extraout_var_01) {
        puStack_124 = (uint *)0x0;
        do {
          if ((*(byte *)in_stack_00000008 & 0x80) == 0) {
            iVar19 = *(int *)((byte *)((int)piVar13 + *puVar18 + 0x2c) + (int)puVar18);
            if (iVar19 == 0) {
              pbVar20 = (byte *)0x0;
            }
            else {
              pbVar20 = (byte *)((int)piVar13 + *puVar18 + 0x2c) + (int)puVar18 + iVar19;
            }
            uVar7 = *(undefined4 *)(pbVar20 + iVar15 * 8);
          }
          else {
            uVar7 = *(undefined4 *)
                     ((int)((byte *)((int)piVar13 + (int)puVar18) + *puVar18 + 0x2c) +
                     *(int *)((byte *)((int)piVar13 + (int)puVar18) + *puVar18 + 0x2c) +
                     (int)puStack_124);
          }
          puStack_124 = (uint *)((int)puStack_124 + 0xc);
          pcVar4 = (this->cls_0x5a486c).vftptr_0x0;
          puStack_f4[iVar15] = uVar7;
          iVar15 = iVar15 + 1;
          uVar6 = (*pcVar4->virt_meth_0x4101f0_60)(this);
        } while (iVar15 < CONCAT31(extraout_var_02,uVar6));
      }
    }
    uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    piStack_d0 = FUN_00482fb0(CONCAT31(extraout_var_05,uVar6) << 2);
    uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    puStack_d4 = FUN_00482fb0(CONCAT31(extraout_var_06,uVar6) << 2);
    puStack_124 = (uint *)0x0;
    uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    if (CONCAT31(extraout_var_07,uVar6) != 0 && -1 < extraout_var_07) {
      puStack_120 = (uint *)0x0;
      do {
        if ((*(byte *)in_stack_00000008 & 0x80) == 0) {
          iVar15 = *(int *)((byte *)((int)piVar13 + (int)puVar18) + *puVar18 + 0x2c);
          if (iVar15 == 0) {
            pbVar20 = (byte *)0x0;
          }
          else {
            pbVar20 = (byte *)((int)piVar13 + (int)puVar18) + iVar15 + *puVar18 + 0x2c;
          }
          piVar13 = (int *)(pbVar20 + (int)puStack_124 * 8 + 4);
          if ((*(int *)(pbVar20 + (int)puStack_124 * 8 + 4) != 0) &&
             ((byte *)(*(int *)(pbVar20 + (int)puStack_124 * 8 + 4) + (int)piVar13) != (byte *)0x0))
          {
            uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            puVar8 = FUN_00482fb0(CONCAT31(extraout_var_08,uVar6) << 3);
            pcVar4 = (this->cls_0x5a486c).vftptr_0x0;
            puStack_d4[(int)puStack_124] = puVar8;
            uVar6 = (*pcVar4->virt_meth_0x4104a0_144)(this);
            puStack_dc[(int)puStack_124] = CONCAT31(extraout_var_09,uVar6);
            if (*piVar13 == 0) {
              puVar8 = (undefined4 *)0x0;
            }
            else {
              puVar8 = (undefined4 *)(*piVar13 + (int)piVar13);
            }
            uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            uVar14 = CONCAT31(extraout_var_10,uVar6) << 3;
            puVar11 = (undefined4 *)puStack_dc[unaff_EBX];
            goto LAB_00407a8d;
          }
          piStack_d0[(int)puStack_124] = 0;
          puStack_d4[(int)puStack_124] = 0;
        }
        else {
          iVar15 = *(int *)((byte *)((int)puStack_118 + *puVar18 + 0x2c) + (int)puVar18);
          pbVar20 = (byte *)((int)puStack_120 +
                            (int)((byte *)((int)puStack_118 + *puVar18 + 0x2c) + (int)puVar18));
          if ((*(int *)(pbVar20 + iVar15 + 8) == 0) ||
             (pbVar20 + *(int *)(pbVar20 + iVar15 + 8) + 8 + iVar15 == (byte *)0x0)) {
            piStack_d0[(int)puStack_124] = 0;
            puStack_d4[(int)puStack_124] = 0;
          }
          else {
            puVar8 = FUN_00482fb0(*(int *)(pbVar20 + iVar15 + 4) << 2);
            piStack_d0[(int)puStack_124] = (int)puVar8;
            puStack_d4[(int)puStack_124] = *(undefined4 *)(pbVar20 + iVar15 + 4);
            if (*(int *)(pbVar20 + iVar15 + 8) == 0) {
              puVar8 = (undefined4 *)0x0;
            }
            else {
              puVar8 = (undefined4 *)(pbVar20 + *(int *)(pbVar20 + iVar15 + 8) + 8 + iVar15);
            }
            puVar11 = (undefined4 *)piStack_d0[(int)puStack_124];
            uVar14 = *(int *)(pbVar20 + iVar15 + 4) << 2;
LAB_00407a8d:
            for (uVar14 = uVar14 >> 2; uVar14 != 0; uVar14 = uVar14 - 1) {
              *puVar11 = *puVar8;
              puVar8 = puVar8 + 1;
              puVar11 = puVar11 + 1;
            }
            for (iVar15 = 0; iVar15 != 0; iVar15 = iVar15 + -1) {
              *(undefined *)puVar11 = *(undefined *)puVar8;
              puVar8 = (undefined4 *)((int)puVar8 + 1);
              puVar11 = (undefined4 *)((int)puVar11 + 1);
            }
          }
        }
        puStack_124 = (uint *)((int)puStack_124 + 1);
        puStack_120 = puStack_120 + 3;
        uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        piVar13 = (int *)puStack_118;
      } while ((int)puStack_124 < CONCAT31(extraout_var_11,uVar6));
    }
    if ((this->cls_0x5a486c).mbr_0xc == 0) {
      if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
        cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
      }
      meth_0x407510(this);
    }
    puVar9 = FUN_00482fb0(0x4c);
    puVar10 = auStack_114;
    for (iVar15 = 0x13; iVar15 != 0; iVar15 = iVar15 + -1) {
      *puVar9 = *puVar10;
      puVar10 = puVar10 + 1;
      puVar9 = puVar9 + 1;
    }
    cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x64);
    puStack_118 = puStack_118 + 0xc;
    puStack_11c = (uint *)((int)puStack_11c + 1);
  } while ((int)puStack_11c < (int)in_stack_00000008[0xb]);
  uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
  puVar8 = FUN_00482fb0(CONCAT31(extraout_var_12,uVar6) << 2);
  *(undefined4 **)&(this->cls_0x5a486c).field_0x98 = puVar8;
  uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
  puVar8 = *(undefined4 **)&(this->cls_0x5a486c).field_0x98;
  for (uVar14 = CONCAT31(extraout_var_13,uVar6) & 0x3fffffff; uVar14 != 0; uVar14 = uVar14 - 1) {
    *puVar8 = 0;
    puVar8 = puVar8 + 1;
  }
  for (iVar15 = 0; iVar15 != 0; iVar15 = iVar15 + -1) {
    *(undefined *)puVar8 = 0;
    puVar8 = (undefined4 *)((int)puVar8 + 1);
  }
  if (((this->cls_0x5a486c).mbr_0x1c & 1) == 0) {
    iVar15 = 0;
    uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    if (CONCAT31(extraout_var_14,uVar6) != 0 && -1 < extraout_var_14) {
      puStack_11c = (uint *)0x0;
      puStack_118 = (uint *)0x0;
      puStack_124 = (uint *)0x0;
      do {
        if (in_stack_00000008[1] < 2) {
          piVar13 = (int *)puStack_118;
          if (in_stack_00000008[2] != 0) {
            piVar13 = (int *)((byte *)(in_stack_00000008[2] + 8 + (int)in_stack_00000008) +
                             (int)puStack_118);
          }
        }
        else if (in_stack_00000008[1] == 2) {
          piVar13 = (int *)puStack_124;
          if (in_stack_00000008[2] != 0) {
            piVar13 = (int *)((int)puStack_124 +
                             (int)(in_stack_00000008[2] + 8 + (int)in_stack_00000008));
          }
        }
        else {
          piVar13 = (int *)((byte *)((int)in_stack_00000008 + in_stack_00000008[2] + 8) +
                           (int)puStack_118);
        }
        if (piVar13[3] == 0) {
          dVar12 = (this->cls_0x5a486c).mbr_0x4;
          *(undefined4 *)(*(int *)&(this->cls_0x5a486c).field_0x98 + iVar15 * 4) = 0;
          pbVar20 = (byte *)(*(int *)(dVar12 + 0x54) + 0x31 + (int)puStack_11c);
          *pbVar20 = *pbVar20 | 0x20;
        }
        else {
          uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
          puVar18 = FUN_00482fb0(CONCAT31(extraout_var_15,uVar6) << 3);
          *(uint **)(*(int *)&(this->cls_0x5a486c).field_0x98 + iVar15 * 4) = puVar18;
          if ((*in_stack_00000008 & 0x40) == 0) {
            iVar19 = piVar13[3];
            if ((*in_stack_00000008 & 8) == 0) {
              if (iVar19 == 0) {
                puStack_124 = (uint *)0x0;
              }
              else {
                puStack_124 = (uint *)((int)piVar13 + iVar19 + 0xc);
              }
              iVar19 = 0;
              uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
              if (CONCAT31(extraout_var_19,uVar6) != 0 && -1 < extraout_var_19) {
                do {
                  uVar14 = *puStack_11c;
                  puStack_11c = puStack_11c + 1;
                  *puVar18 = uVar14 >> 8 & 0xffff;
                  pcVar4 = (this->cls_0x5a486c).vftptr_0x0;
                  puVar18[1] = uVar14 >> 0x18 ^ uVar14 & 0xff000000;
                  iVar19 = iVar19 + 1;
                  uVar6 = (*pcVar4->virt_meth_0x4104a0_144)(this);
                  puVar18 = puVar18 + 2;
                } while (iVar19 < CONCAT31(extraout_var_20,uVar6));
              }
            }
            else {
              if (iVar19 == 0) {
                puStack_124 = (uint *)0x0;
              }
              else {
                puStack_124 = (uint *)((int)piVar13 + iVar19 + 0xc);
              }
              iVar19 = 0;
              uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
              if (CONCAT31(extraout_var_17,uVar6) != 0 && -1 < extraout_var_17) {
                do {
                  auStack_94[0] = *puVar18;
                  uVar14 = *puStack_11c;
                  puStack_11c = puStack_11c + 1;
                  *puVar18 = uVar14 & 0xffff;
                  pcVar4 = (this->cls_0x5a486c).vftptr_0x0;
                  puVar18[1] = (uVar14 & 0xffff0000) << 8 | uVar14 >> 0x18;
                  iVar19 = iVar19 + 1;
                  uVar6 = (*pcVar4->virt_meth_0x4104a0_144)(this);
                  puVar18 = puVar18 + 2;
                } while (iVar19 < CONCAT31(extraout_var_18,uVar6));
              }
            }
          }
          else {
            if (piVar13[3] == 0) {
              puVar8 = (undefined4 *)0x0;
            }
            else {
              puVar8 = (undefined4 *)(piVar13[3] + 0xc + (int)piVar13);
            }
            uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4104a0_144)(this);
            puVar11 = *(undefined4 **)(*(int *)&(this->cls_0x5a486c).field_0x98 + iVar15 * 4);
            for (uVar14 = (uint)(CONCAT31(extraout_var_16,uVar6) << 3) >> 2; uVar14 != 0;
                uVar14 = uVar14 - 1) {
              *puVar11 = *puVar8;
              puVar8 = puVar8 + 1;
              puVar11 = puVar11 + 1;
            }
            for (iVar19 = 0; iVar19 != 0; iVar19 = iVar19 + -1) {
              *(undefined *)puVar11 = *(undefined *)puVar8;
              puVar8 = (undefined4 *)((int)puVar8 + 1);
              puVar11 = (undefined4 *)((int)puVar11 + 1);
            }
          }
        }
        puStack_11c = puStack_11c + 0x13;
        puStack_124 = puStack_124 + 0x14;
        puStack_118 = puStack_118 + 4;
        iVar15 = iVar15 + 1;
        uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      } while (iVar15 < CONCAT31(extraout_var_21,uVar6));
    }
  }
  pdVar1 = &(this->cls_0x5a486c).mbr_0x4c;
  (this->cls_0x5a486c).mbr_0x10 = (uint)(in_stack_00000008[9] != 0);
  iVar15 = 0;
  if (0 < (int)*pdVar1) {
    do {
      if ((-1 < iVar15) &&
         (pvVar5 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x5c + iVar15 * 4), pvVar5 != (LPCVOID)0x0)
         ) {
        FUN_004830f0(pvVar5);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar15 = iVar15 + 1;
    } while (iVar15 < (int)*pdVar1);
  }
  bVar23 = DAT_005c619c != 0;
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x50 = 0;
  if ((bVar23) && (puStack_11c = (uint *)0x0, 0 < (int)in_stack_00000008[9])) {
    puVar18 = in_stack_00000008 + 10;
    puStack_124 = (uint *)0x0;
    do {
      puVar10 = auStack_114;
      for (iVar15 = 0x1f; iVar15 != 0; iVar15 = iVar15 + -1) {
        *puVar10 = 0;
        puVar10 = puVar10 + 1;
      }
      piVar13 = (int *)((int)puVar18 + (int)puStack_124 + *puVar18 + 0x70);
      puVar10 = (uint *)((int)puVar18 + (int)puStack_124 + *puVar18);
      puVar9 = auStack_114;
      for (iVar15 = 0x1b; iVar15 != 0; iVar15 = iVar15 + -1) {
        *puVar9 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar9 = puVar9 + 1;
      }
      auStack_114[0] = 0x7c;
      iVar15 = *piVar13;
      if (iVar15 == 0) {
        puStack_120 = (uint *)0x0;
      }
      else {
        puStack_120 = (uint *)((int)piVar13 + iVar15);
      }
      puVar10 = auStack_94;
      for (iVar15 = 0x26; iVar15 != 0; iVar15 = iVar15 + -1) {
        *puVar10 = 0;
        puVar10 = puVar10 + 1;
      }
      iVar15 = meth_0x40bb30(this,auStack_94);
      if (iVar15 != 0) {
        puVar9 = FUN_00482fb0(0x98);
        puVar10 = auStack_94;
        for (iVar15 = 0x26; iVar15 != 0; iVar15 = iVar15 + -1) {
          *puVar9 = *puVar10;
          puVar10 = puVar10 + 1;
          puVar9 = puVar9 + 1;
        }
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x4c);
      }
      puStack_11c = (uint *)((int)puStack_11c + 1);
      puStack_124 = (uint *)((int)puStack_124 + 0x78);
    } while ((int)puStack_11c < (int)in_stack_00000008[9]);
  }
  pdVar1 = &(this->cls_0x5a486c).mbr_0x34;
  iVar15 = 0;
  if (0 < (int)(this->cls_0x5a486c).mbr_0x34) {
    do {
      if ((-1 < iVar15) &&
         (pvVar5 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x44 + iVar15 * 4), pvVar5 != (LPCVOID)0x0)
         ) {
        FUN_004830f0(pvVar5);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar15 = iVar15 + 1;
    } while (iVar15 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x38 = 0;
  puStack_11c = (uint *)0x0;
  if (0 < (int)in_stack_00000008[7]) {
    puStack_120 = (uint *)0x0;
    do {
      puVar18 = (uint *)((byte *)(in_stack_00000008[8] + (int)(in_stack_00000008 + 8)) +
                        (int)puStack_120);
      puVar10 = auStack_114;
      for (iVar15 = 0x14; iVar15 != 0; iVar15 = iVar15 + -1) {
        *puVar10 = *puVar18;
        puVar18 = puVar18 + 1;
        puVar10 = puVar10 + 1;
      }
      iVar15 = (**(code **)(*DAT_00668eb0 + 0x14))(DAT_00668eb0,auStack_c4,0);
      if (iVar15 != 0) {
        FUN_004a90d0(iVar15);
      }
      piStack_d8 = (int *)0x0;
      (**(code **)(*piStack_d0 + 0xc))(piStack_d0,&puStack_120);
      (**(code **)(*piStack_d8 + 0x14))(piStack_d8,DAT_00668f14,&puStack_d4);
      puVar10 = FUN_00482fb0(0x5c);
      puVar18 = auStack_114;
      for (iVar15 = 0x17; iVar15 != 0; iVar15 = iVar15 + -1) {
        *puVar10 = *puVar18;
        puVar18 = puVar18 + 1;
        puVar10 = puVar10 + 1;
      }
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)pdVar1);
      puStack_11c = (uint *)((int)puStack_11c + 1);
      puStack_120 = puStack_120 + 0x14;
    } while ((int)puStack_11c < (int)in_stack_00000008[7]);
  }
  *(undefined4 *)&(this->cls_0x5a486c).field_0x9c = 0;
  if (((this->cls_0x5a486c).mbr_0x1c & 2) != 0) {
    puStack_11c = (uint *)0x0;
    uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    if (CONCAT31(extraout_var_22,uVar6) != 0 && -1 < extraout_var_22) {
      puStack_124 = (uint *)0x0;
      puStack_120 = (uint *)0x0;
      do {
        if (in_stack_00000008[1] < 2) {
          puVar18 = puStack_124;
          if (in_stack_00000008[2] != 0) {
            puVar18 = (uint *)((byte *)((int)in_stack_00000008 + in_stack_00000008[2] + 8) +
                              (int)puStack_124);
          }
        }
        else if (in_stack_00000008[1] == 2) {
          puVar18 = puStack_120;
          if (in_stack_00000008[2] != 0) {
            puVar18 = (uint *)((int)puStack_120 +
                              (int)((int)in_stack_00000008 + in_stack_00000008[2] + 8));
          }
        }
        else {
          puVar18 = (uint *)((byte *)((int)(in_stack_00000008 + 2) + (int)puStack_124) +
                            in_stack_00000008[2]);
        }
        if (0 < (int)*puVar18) {
          if (*(int *)&(this->cls_0x5a486c).field_0x9c == 0) {
            uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
            puVar8 = FUN_00482fb0(CONCAT31(extraout_var_23,uVar6) << 3);
            *(undefined4 **)&(this->cls_0x5a486c).field_0x9c = puVar8;
          }
          uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
          puVar8 = *(undefined4 **)&(this->cls_0x5a486c).field_0x9c;
          for (uVar14 = (uint)(CONCAT31(extraout_var_24,uVar6) << 3) >> 2; uVar14 != 0;
              uVar14 = uVar14 - 1) {
            *puVar8 = 0;
            puVar8 = puVar8 + 1;
          }
          for (iVar15 = 0; iVar15 != 0; iVar15 = iVar15 + -1) {
            *(undefined *)puVar8 = 0;
            puVar8 = (undefined4 *)((int)puVar8 + 1);
          }
          puVar10 = FUN_00482fb0(*puVar18);
          if ((puVar18[1] == 0) || ((byte *)((int)puVar18 + puVar18[1] + 4) == (byte *)0x0)) {
            if ((puVar18[2] != 0) && ((byte *)((int)puVar18 + puVar18[2] + 8) != (byte *)0x0)) {
              if (puVar18[2] == 0) {
                puVar9 = (uint *)0x0;
              }
              else {
                puVar9 = (uint *)((int)puVar18 + puVar18[2] + 8);
              }
              uVar14 = *puVar18;
              puVar18 = puVar10;
              for (uVar16 = uVar14 >> 2; uVar16 != 0; uVar16 = uVar16 - 1) {
                *puVar18 = *puVar9;
                puVar9 = puVar9 + 1;
                puVar18 = puVar18 + 1;
              }
              iVar15 = *(int *)&(this->cls_0x5a486c).field_0x9c;
              for (uVar14 = uVar14 & 3; uVar14 != 0; uVar14 = uVar14 - 1) {
                *(undefined *)puVar18 = *(undefined *)puVar9;
                puVar9 = (uint *)((int)puVar9 + 1);
                puVar18 = (uint *)((int)puVar18 + 1);
              }
              iVar19 = 0;
              *(uint **)(iVar15 + 4 + (int)puStack_11c * 8) = puVar10;
              if (0 < (int)puVar10[3]) {
                puVar18 = puVar10 + 0xe;
                do {
                  if (*puVar18 == 0) {
                    puVar9 = (uint *)0x0;
                  }
                  else {
                    puVar9 = (uint *)(*puVar18 + (int)puVar18);
                  }
                  FUN_004b8820(puVar9);
                  iVar19 = iVar19 + 1;
                  puVar18 = puVar18 + 10;
                } while (iVar19 < (int)puVar10[3]);
              }
            }
          }
          else {
            if (puVar18[1] == 0) {
              puVar9 = (uint *)0x0;
            }
            else {
              puVar9 = (uint *)((int)puVar18 + puVar18[1] + 4);
            }
            uVar14 = *puVar18;
            puVar18 = puVar10;
            for (uVar16 = uVar14 >> 2; uVar16 != 0; uVar16 = uVar16 - 1) {
              *puVar18 = *puVar9;
              puVar9 = puVar9 + 1;
              puVar18 = puVar18 + 1;
            }
            iVar15 = *(int *)&(this->cls_0x5a486c).field_0x9c;
            for (uVar14 = uVar14 & 3; uVar14 != 0; uVar14 = uVar14 - 1) {
              *(undefined *)puVar18 = *(undefined *)puVar9;
              puVar9 = (uint *)((int)puVar9 + 1);
              puVar18 = (uint *)((int)puVar18 + 1);
            }
            *(uint **)(iVar15 + (int)puStack_11c * 8) = puVar10;
            FUN_004b8820(puVar10);
          }
        }
        puStack_120 = puStack_120 + 0x14;
        iVar15 = (int)puStack_11c + 1;
        puStack_124 = puStack_124 + 4;
        puStack_11c = (uint *)iVar15;
        uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      } while (iVar15 < CONCAT31(extraout_var_25,uVar6));
    }
  }
  pdVar1 = &(this->cls_0x5a486c).mbr_0x7c;
  iVar15 = 0;
  if (0 < (int)(this->cls_0x5a486c).mbr_0x7c) {
    do {
      if ((-1 < iVar15) &&
         (pvVar5 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x8c + iVar15 * 4), pvVar5 != (LPCVOID)0x0)
         ) {
        if (*(LPCVOID *)((int)pvVar5 + 8) != (LPCVOID)0x0) {
          FUN_00482f80(*(LPCVOID *)((int)pvVar5 + 8));
        }
        if (*(LPCVOID *)((int)pvVar5 + 0xc) != (LPCVOID)0x0) {
          FUN_00482f80(*(LPCVOID *)((int)pvVar5 + 0xc));
        }
        FUN_004830f0(pvVar5);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)pdVar1);
      iVar15 = iVar15 + 1;
    } while (iVar15 < (int)*pdVar1);
  }
  *pdVar1 = 0;
  *(undefined4 *)&(this->cls_0x5a486c).field_0x80 = 0;
  uVar14 = in_stack_00000008[1];
  if ((uVar14 < 2) && ((*(byte *)in_stack_00000008 & 0x20) != 0)) {
    if (in_stack_00000008[0xe] == 0) {
      puVar8 = (undefined4 *)0x0;
    }
    else {
      puVar8 = (undefined4 *)((int)in_stack_00000008 + in_stack_00000008[0xe] + 0x38);
    }
    puStack_120 = (uint *)0x0;
    if (0 < (int)in_stack_00000008[0xd]) {
      puVar18 = puVar8 + 3;
      do {
        puVar11 = FUN_00482fb0(0x10);
        if (puVar11 != (undefined4 *)0x0) {
          uVar14 = puVar18[-2];
          *puVar11 = *puVar8;
          puVar11[1] = uVar14;
          puVar10 = FUN_0059b6bc((uint *)&DAT_005c5d20);
          puVar11[2] = puVar10;
          puVar10 = FUN_0059b6bc(puVar18);
          puVar10 = FUN_0059bd3e(puVar10);
          puVar11[3] = puVar10;
        }
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x7c);
        puStack_120 = (uint *)((int)puStack_120 + 1);
        puVar8 = puVar8 + 0xb;
        puVar18 = puVar18 + 0xb;
      } while ((int)puStack_120 < (int)in_stack_00000008[0xd]);
    }
  }
  else if (uVar14 == 2) {
    if (in_stack_00000008[0xe] == 0) {
      puStack_118 = (uint *)0x0;
    }
    else {
      puStack_118 = (uint *)((int)in_stack_00000008 + in_stack_00000008[0xe] + 0x38);
    }
    puStack_120 = (uint *)0x0;
    puStack_11c = (uint *)0x0;
    uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
    if (CONCAT31(extraout_var_26,uVar6) != 0 && -1 < extraout_var_26) {
      puStack_124 = (uint *)0x0;
      do {
        if (in_stack_00000008[2] == 0) {
          pbVar20 = (byte *)0x0;
        }
        else {
          pbVar20 = (byte *)((int)in_stack_00000008 + in_stack_00000008[2] + 8);
        }
        if (pbVar20[(int)puStack_124 + 0x10] != 0) {
          piVar13 = FUN_00482fb0(0x10);
          if (piVar13 != (int *)0x0) {
            *piVar13 = (int)puStack_11c;
            piVar13[1] = 0;
            puVar18 = FUN_0059b6bc((uint *)&PTR_null_005c5d28);
            piVar13[2] = (int)puVar18;
            puVar18 = FUN_0059b6bc((uint *)(pbVar20 + (int)puStack_124 + 0x10));
            puVar18 = FUN_0059bd3e(puVar18);
            piVar13[3] = (int)puVar18;
          }
          cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x7c);
        }
        if (pbVar20[(int)puStack_124 + 0x30] != 0) {
          piVar13 = FUN_00482fb0(0x10);
          if (piVar13 != (int *)0x0) {
            *piVar13 = (int)puStack_11c;
            piVar13[1] = 0;
            puVar18 = FUN_0059b6bc((uint *)&PTR_null_005c5d2c);
            piVar13[2] = (int)puVar18;
            puVar18 = FUN_0059b6bc((uint *)(pbVar20 + (int)puStack_124 + 0x30));
            puVar18 = FUN_0059bd3e(puVar18);
            piVar13[3] = (int)puVar18;
          }
          cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x7c);
        }
        if (((*(byte *)in_stack_00000008 & 0x20) != 0) &&
           ((int)puStack_120 < (int)in_stack_00000008[0xd])) {
          puVar18 = puStack_118 + 3;
          do {
            if ((uint *)*puStack_118 != puStack_11c) break;
            piVar13 = FUN_00482fb0(0x10);
            if (piVar13 != (int *)0x0) {
              piVar13[1] = puVar18[-2];
              *piVar13 = *puStack_118;
              puVar10 = FUN_0059b6bc((uint *)&DAT_005c5d30);
              piVar13[2] = (int)puVar10;
              puVar10 = FUN_0059b6bc(puVar18);
              puVar10 = FUN_0059bd3e(puVar10);
              piVar13[3] = (int)puVar10;
            }
            cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x7c);
            puStack_118 = puStack_118 + 0xb;
            puVar18 = puVar18 + 0xb;
            puStack_120 = (uint *)((int)puStack_120 + 1);
          } while ((int)puStack_120 < (int)in_stack_00000008[0xd]);
        }
        puStack_124 = (uint *)((int)puStack_124 + 0x50);
        iVar15 = (int)puStack_11c + 1;
        puStack_11c = (uint *)iVar15;
        uVar6 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      } while (iVar15 < CONCAT31(extraout_var_27,uVar6));
    }
  }
  else if (2 < uVar14) {
    if (in_stack_00000008[0xe] == 0) {
      puVar8 = (undefined4 *)0x0;
    }
    else {
      puVar8 = (undefined4 *)((int)in_stack_00000008 + in_stack_00000008[0xe] + 0x38);
    }
    puStack_11c = (uint *)0x0;
    if (0 < (int)in_stack_00000008[0xd]) {
      do {
        puVar11 = FUN_00482fb0(0x10);
        if (puVar11 != (undefined4 *)0x0) {
          if (puVar8[3] == 0) {
            puVar18 = (uint *)0x0;
          }
          else {
            puVar18 = (uint *)(puVar8[3] + 0xc + (int)puVar8);
          }
          if (puVar8[2] == 0) {
            puVar10 = (uint *)0x0;
          }
          else {
            puVar10 = (uint *)(puVar8[2] + 8 + (int)puVar8);
          }
          uVar7 = puVar8[1];
          *puVar11 = *puVar8;
          puVar11[1] = uVar7;
          puVar10 = FUN_0059b6bc(puVar10);
          puVar11[2] = puVar10;
          puVar18 = FUN_0059b6bc(puVar18);
          puVar18 = FUN_0059bd3e(puVar18);
          puVar11[3] = puVar18;
        }
        cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x7c);
        puStack_11c = (uint *)((int)puStack_11c + 1);
        puVar8 = puVar8 + 4;
      } while ((int)puStack_11c < (int)in_stack_00000008[0xd]);
    }
  }
  iVar15 = 0;
  puStack_11c = (uint *)0x0;
  if (0 < (int)(this->cls_0x5a486c).mbr_0x7c) {
    do {
      dVar12 = *(dword *)((this->cls_0x5a486c).mbr_0x8c + iVar15);
      if (dVar12 == 0) {
        dVar12 = (this->cls_0x5a486c).mbr_0x90;
      }
      iVar19 = 5;
      bVar23 = true;
      pcVar21 = *(char **)(dVar12 + 8);
      pcVar22 = &DAT_005c5d38;
      do {
        if (iVar19 == 0) break;
        iVar19 = iVar19 + -1;
        bVar23 = *pcVar21 == *pcVar22;
        pcVar21 = pcVar21 + 1;
        pcVar22 = pcVar22 + 1;
      } while (bVar23);
      if (bVar23) {
        dVar12 = *(dword *)((this->cls_0x5a486c).mbr_0x8c + iVar15);
        if (dVar12 == 0) {
          dVar12 = (this->cls_0x5a486c).mbr_0x90;
        }
        iVar19 = FUN_004833a0(*(char **)(dVar12 + 0xc));
        iVar17 = 0;
        if (0 < iVar19) {
          do {
            dVar12 = *(dword *)((this->cls_0x5a486c).mbr_0x8c + iVar15);
            if (dVar12 == 0) {
              dVar12 = (this->cls_0x5a486c).mbr_0x90;
            }
            FUN_00483330(*(char **)(dVar12 + 0xc),iVar17,(char *)0x0,0);
            cls_0x41c7d0::meth_0x49b880((cls_0x41c7d0 *)&DAT_00667548);
            iVar17 = iVar17 + 1;
          } while (iVar17 < iVar19);
        }
      }
      puStack_11c = (uint *)((int)puStack_11c + 1);
      iVar15 = iVar15 + 4;
    } while ((int)puStack_11c < (int)(this->cls_0x5a486c).mbr_0x7c);
  }
  cls_0x5a486c::meth_0x447d80(&this->cls_0x5a486c);
  DAT_005e8300 = DAT_005e8300 + 1;
  return 1;
}



// Function at 004085f0

void cls_0x5a35ac::meth_0x4085f0()

{
  dword *pdVar1;
  LPCVOID pvVar2;
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
           (pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x74 + iVar6 * 4), pvVar2 != (LPCVOID)0x0
           )) {
          FUN_004830f0(pvVar2);
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
           (pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x5c + iVar6 * 4), pvVar2 != (LPCVOID)0x0
           )) {
          FUN_004830f0(pvVar2);
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
        pvVar2 = *(LPCVOID *)(*(int *)&(this->cls_0x5a486c).field_0x98 + iVar6 * 4);
        if (pvVar2 != (LPCVOID)0x0) {
          FUN_004830f0(pvVar2);
        }
        iVar6 = iVar6 + 1;
        uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      } while (iVar6 < CONCAT31(extraout_var_00,uVar3));
    }
    FUN_004830f0(*(LPCVOID *)&(this->cls_0x5a486c).field_0x98);
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
            FUN_004830f0(*(LPCVOID *)
                          (*(int *)((this->cls_0x5a486c).mbr_0x28 + iVar6 * 4) + iVar7 * 4));
            iVar7 = iVar7 + 1;
          } while (iVar7 < iVar4);
        }
        FUN_004830f0(*(LPCVOID *)((this->cls_0x5a486c).mbr_0x28 + iVar6 * 4));
        iVar6 = iVar6 + 1;
      } while (iVar6 < iStack_4);
    }
    iVar6 = 0;
    FUN_004830f0((LPCVOID)(this->cls_0x5a486c).mbr_0x28);
    (this->cls_0x5a486c).mbr_0x28 = 0;
    (this->cls_0x5a486c).mbr_0x24 = 0;
    FUN_004830f0((LPCVOID)(this->cls_0x5a486c).mbr_0x30);
    (this->cls_0x5a486c).mbr_0x30 = 0;
    if (*(int *)&(this->cls_0x5a486c).field_0x9c != 0) {
      iVar4 = 0;
      uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      if (CONCAT31(extraout_var_03,uVar3) != 0 && -1 < extraout_var_03) {
        do {
          iVar7 = *(int *)&(this->cls_0x5a486c).field_0x9c;
          pvVar2 = *(LPCVOID *)(iVar7 + iVar4 * 8);
          puVar5 = (undefined4 *)(iVar7 + iVar4 * 8);
          if (pvVar2 != (LPCVOID)0x0) {
            FUN_004830f0(pvVar2);
            puVar5 = (undefined4 *)(*(int *)&(this->cls_0x5a486c).field_0x9c + iVar4 * 8);
            *puVar5 = 0;
          }
          pvVar2 = (LPCVOID)puVar5[1];
          if (pvVar2 != (LPCVOID)0x0) {
            FUN_00418600();
            FUN_004830f0(pvVar2);
            *(undefined4 *)(*(int *)&(this->cls_0x5a486c).field_0x9c + 4 + iVar4 * 8) = 0;
          }
          iVar4 = iVar4 + 1;
          uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        } while (iVar4 < CONCAT31(extraout_var_04,uVar3));
      }
      FUN_004830f0(*(LPCVOID *)&(this->cls_0x5a486c).field_0x9c);
    }
    pdVar1 = &(this->cls_0x5a486c).mbr_0x7c;
    *(undefined4 *)&(this->cls_0x5a486c).field_0x9c = 0;
    if (0 < (int)(this->cls_0x5a486c).mbr_0x7c) {
      do {
        if ((-1 < iVar6) &&
           (pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x8c + iVar6 * 4), pvVar2 != (LPCVOID)0x0
           )) {
          if (*(LPCVOID *)((int)pvVar2 + 8) != (LPCVOID)0x0) {
            FUN_00482f80(*(LPCVOID *)((int)pvVar2 + 8));
          }
          if (*(LPCVOID *)((int)pvVar2 + 0xc) != (LPCVOID)0x0) {
            FUN_00482f80(*(LPCVOID *)((int)pvVar2 + 0xc));
          }
          FUN_004830f0(pvVar2);
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

undefined4 cls_0x5a35ac::virt_meth_0x4088e0()

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

void cls_0x5a35ac::virt_meth_0x408980()

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

void cls_0x5a35ac::meth_0x408ff0()

{
  uint uVar1;
  LPCVOID pvVar2;
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
      FUN_004830f0(*(LPCVOID *)(dVar4 + 0x38));
      FUN_004830f0(*(LPCVOID *)(dVar4 + 0x34));
      FUN_004830f0(*(LPCVOID *)(dVar4 + 0x20));
      iVar5 = 0;
      uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
      if (CONCAT31(extraout_var,uVar3) != 0 && -1 < extraout_var) {
        do {
          pvVar2 = *(LPCVOID *)(*(int *)(dVar4 + 0x44) + iVar5 * 4);
          if (pvVar2 != (LPCVOID)0x0) {
            FUN_004830f0(pvVar2);
          }
          iVar5 = iVar5 + 1;
          uVar3 = (*((this->cls_0x5a486c).vftptr_0x0)->virt_meth_0x4101f0_60)(this);
        } while (iVar5 < CONCAT31(extraout_var_00,uVar3));
      }
      FUN_004830f0(*(LPCVOID *)(dVar4 + 0x44));
      FUN_004830f0(*(LPCVOID *)(dVar4 + 0x40));
      pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x74 + in_stack_00000004 * 4);
      if (pvVar2 != (LPCVOID)0x0) {
        FUN_004830f0(pvVar2);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x64);
    }
  }
  return;
}



// Function at 0040bb30

undefined4 cls_0x5a35ac::meth_0x40bb30(undefined4 param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  undefined4 *unaff_ESI;
  uint *puVar5;
  uint *puVar6;
  uint *in_stack_00000008;
  uint in_stack_00000014;
  uint in_stack_0000001c;
  int *piVar7;
  undefined4 uVar8;
  int *piVar9;
  int *piStack_11c;
  int *piStack_118;
  int iStack_114;
  uint uStack_110;
  uint uStack_10c;
  int iStack_108;
  int *piStack_104;
  undefined4 *puStack_100;
  undefined4 uStack_fc;
  undefined4 uStack_f8;
  uint auStack_f4 [4];
  uint uStack_e4;
  uint uStack_c4;
  uint uStack_a8;
  int iStack_a0;
  undefined4 uStack_8c;
  int iStack_80;
  undefined4 uStack_7c;
  undefined4 auStack_78 [4];
  undefined4 uStack_68;
  uint uStack_44;
  uint uStack_18;
  uint uStack_14;
  undefined4 uStack_8;
  int iStack_4;
  
  if (in_stack_00000008[3] != in_stack_00000008[2]) {
    FUN_00481d10((byte *)s__s_has_non_square_texture_005c5f00);
  }
  uVar1 = in_stack_00000008[3];
  if (((((uVar1 != 0x80) && (uVar1 != 0x40)) && (uVar1 != 0x100)) &&
      ((uVar1 != 0x20 && (uVar1 != 0x200)))) && ((uVar1 != 0x10 && (uVar1 != 8)))) {
    FUN_00481d10((byte *)s__s_texture_is_not_a_power_of_two_005c5f1c);
  }
  puVar5 = in_stack_00000008;
  puVar6 = auStack_f4;
  for (iVar3 = 0x1f; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar6 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar6 = puVar6 + 1;
  }
  cls_0x4164c0::meth_0x4164c0((cls_0x4164c0 *)((int)&DAT_0065a578 + 4),in_stack_00000008);
  iVar3 = 0;
  if ((uStack_a8 & 0x30) == 0) {
    if ((uStack_a8 & 0x1808) != 0) {
      iVar3 = 1 << ((byte)iStack_a0 & 0x1f);
    }
  }
  else {
    iVar3 = 0x100;
  }
  in_stack_00000008[4] = in_stack_00000008[3];
  auStack_f4[1] = 0x100f;
  uStack_8c = 0x1000;
  if (DAT_005e88a4 < auStack_f4[3]) {
    do {
      auStack_f4[3] = auStack_f4[3] >> 1;
    } while (DAT_005e88a4 < auStack_f4[3]);
  }
  if (DAT_005e88a4 < auStack_f4[2]) {
    do {
      auStack_f4[2] = auStack_f4[2] >> 1;
    } while (DAT_005e88a4 < auStack_f4[2]);
  }
  uStack_e4 = auStack_f4[3];
  puVar5 = auStack_f4;
  puVar6 = in_stack_00000008;
  for (iVar4 = 0x1f; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar6 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar6 = puVar6 + 1;
  }
  if (iVar3 < 1) {
    puStack_100 = (undefined4 *)0x0;
  }
  else {
    puStack_100 = FUN_00482fb0(iVar3 << 2);
  }
  iVar3 = uStack_e4 * auStack_f4[2];
  iVar4 = FUN_004a8170();
  if ((int)(iVar4 - ((uint)(iVar3 * iStack_a0) >> 3) * in_stack_00000014) < 0x19000) {
    in_stack_0000001c = 1;
  }
  if ((int)in_stack_00000014 < 2) {
    in_stack_0000001c = 0;
  }
  in_stack_00000008[0x23] = in_stack_0000001c;
  in_stack_00000008[0x1f] = in_stack_00000014;
  in_stack_00000008[0x20] = 0;
  if ((int)in_stack_00000014 < 2) {
    in_stack_00000008[0x21] = 0;
    in_stack_00000008[0x22] = 0;
  }
  else {
    puVar2 = FUN_00482fb0(in_stack_00000014 * 4);
    in_stack_00000008[0x21] = (uint)puVar2;
    puVar2 = FUN_00482fb0(in_stack_00000014 * 4);
    in_stack_00000008[0x22] = (uint)puVar2;
  }
  in_stack_00000008[0x24] = 0;
  in_stack_00000008[0x25] = 0;
  piStack_118 = (int *)0x0;
  piStack_104 = (int *)0x0;
  uStack_f8 = 0;
  uStack_110 = 0;
  uStack_10c = 0;
  uStack_fc = 0;
  iStack_108 = 0;
  if (0 < (int)in_stack_00000014) {
    do {
      if (piStack_118 == (int *)0x0) {
        uStack_8c = 0x1800;
        (**(code **)(*DAT_006695ac + 0x18))(DAT_006695ac,auStack_f4,&piStack_118,0);
        if (piStack_118 == (int *)0x0) {
          return 0;
        }
      }
      puVar2 = auStack_78;
      for (iVar3 = 0x1f; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar2 = 0;
        puVar2 = puVar2 + 1;
      }
      piVar9 = (int *)0x0;
      puVar2 = auStack_78;
      uVar8 = 0x801;
      auStack_78[0] = 0x7c;
      iVar3 = (**(code **)(*piStack_118 + 100))(piStack_118,0,puVar2,0x801);
      if (iVar3 != 0) {
        FUN_004a90d0(iVar3);
      }
      iVar4 = iStack_114;
      iVar3 = *(int *)(iStack_4 + (int)piStack_11c * 4);
      if (iVar3 == 0) {
        iVar3 = 0;
      }
      else {
        iVar3 = iStack_4 + (int)piStack_11c * 4 + iVar3;
      }
      cls_0x4164c0::meth_0x416790
                ((cls_0x4164c0 *)((int)&DAT_0065a578 + 4),uStack_8,iVar3,param_1,&iStack_108,
                 uStack_68);
      iVar3 = (**(code **)(*piVar9 + 0x80))(piVar9,0);
      if (iVar3 != 0) {
        FUN_004a90d0(iVar3);
      }
      if (iVar4 != 0) {
        if ((uStack_c4 & 0x30) == 0) {
          if ((uStack_c4 & 8) == 0) {
            if ((uStack_c4 & 0x1000) == 0) {
              if ((uStack_c4 & 0x800) != 0) {
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
        iVar3 = (**(code **)(*DAT_006695ac + 0x14))(DAT_006695ac,uVar8,iVar4,&iStack_114);
        if (iVar3 != 0) {
          FUN_004a90d0(iVar3);
        }
        iVar3 = (**(code **)(*piVar9 + 0x7c))(piVar9,unaff_ESI);
        if (iVar3 != 0) {
          FUN_004a90d0(iVar3);
        }
      }
      iVar3 = (**(code **)*puVar2)(puVar2,&DAT_005a61f8,&stack0xfffffee0);
      if (iVar3 != 0) {
        FUN_004a90d0(iVar3);
      }
      if (uStack_110 == 0) {
        puVar2 = auStack_78;
        for (iVar3 = 0x1f; iVar3 != 0; iVar3 = iVar3 + -1) {
          *puVar2 = 0;
          puVar2 = puVar2 + 1;
        }
        auStack_78[0] = 0x7c;
        piVar9 = piStack_118;
        iVar3 = (**(code **)(*piStack_118 + 0x58))(piStack_118,auStack_78);
        if (iVar3 != 0) {
          FUN_004a90d0(iVar3);
        }
        uStack_7c = 0x1007;
        uStack_18 = 0x4001000;
        iVar3 = FUN_004a8370();
        if (iVar3 == 0) {
          uStack_18 = uStack_18 | 0x800;
        }
        else {
          uStack_18 = uStack_18 | 0x10004000;
          uStack_14 = uStack_14 | 8;
        }
        piVar7 = &iStack_80;
        (**(code **)(*DAT_006695ac + 0x18))(DAT_006695ac,piVar7,&piStack_118,0);
        if (unaff_ESI == (undefined4 *)0x0) {
          return 0;
        }
        if ((uStack_44 & 0x30) == 0) {
          if ((uStack_44 & 8) != 0) {
            uVar8 = 1;
            goto LAB_0040bfbd;
          }
          if ((uStack_44 & 0x1000) != 0) {
            uVar8 = 0x200;
            goto LAB_0040bfbd;
          }
          if ((uStack_44 & 0x800) != 0) {
            uVar8 = 0x100;
            goto LAB_0040bfbd;
          }
        }
        else {
          uVar8 = 0x44;
LAB_0040bfbd:
          iVar3 = (**(code **)(*DAT_006695ac + 0x14))(DAT_006695ac,uVar8,iVar4,&iStack_114,0);
          if (iVar3 != 0) {
            FUN_004a90d0(iVar3);
          }
          iVar3 = (**(code **)(*piVar7 + 0x7c))(piVar7,unaff_ESI);
          if (iVar3 != 0) {
            FUN_004a90d0(iVar3);
          }
        }
        iVar3 = (**(code **)*unaff_ESI)(unaff_ESI,&DAT_005a61f8,&stack0xfffffedc);
        if (iVar3 != 0) {
          FUN_004a90d0(iVar3);
        }
        iVar3 = (**(code **)(*piVar9 + 0x14))(piVar9,unaff_ESI);
        if (iVar3 != 0) {
          return 0;
        }
      }
      if ((int)in_stack_00000014 < 2) {
        in_stack_00000008[0x24] = uStack_110;
        in_stack_00000008[0x25] = uStack_10c;
      }
      else if ((in_stack_0000001c == 0) || (in_stack_00000008[0x21] == 0)) {
        puVar5 = (uint *)in_stack_00000008[0x21];
        if (puVar5 != (uint *)0x0) {
          puVar6 = (uint *)in_stack_00000008[0x22];
          puVar5[iStack_108] = uStack_110;
          puVar6[iStack_108] = uStack_10c;
          uStack_110 = 0;
          uStack_10c = 0;
          uVar1 = *puVar6;
          in_stack_00000008[0x24] = *puVar5;
          in_stack_00000008[0x25] = uVar1;
        }
      }
      else {
        uVar1 = in_stack_00000008[0x22];
        *(int **)(in_stack_00000008[0x21] + iStack_108 * 4) = piStack_118;
        *(int **)(uVar1 + iStack_108 * 4) = piStack_104;
        piStack_118 = (int *)0x0;
        piStack_104 = (int *)0x0;
        in_stack_00000008[0x24] = uStack_110;
        in_stack_00000008[0x25] = uStack_10c;
      }
      iStack_108 = iStack_108 + 1;
    } while (iStack_108 < (int)in_stack_00000014);
    if (piStack_104 != (int *)0x0) {
      (**(code **)(*piStack_104 + 8))(piStack_104);
      (**(code **)(*piStack_11c + 8))(piStack_11c);
    }
  }
  if (puStack_100 != (undefined4 *)0x0) {
    FUN_004830f0(puStack_100);
  }
  return 1;
}



// Function at 0040c190

void cls_0x5a35ac::meth_0x40c190()

{
  int *piVar1;
  LPCVOID pvVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  dword dVar6;
  int in_stack_00000004;
  
  iVar5 = 0;
  dVar6 = *(dword *)((this->cls_0x5a486c).mbr_0x5c + in_stack_00000004 * 4);
  if (dVar6 == 0) {
    dVar6 = (this->cls_0x5a486c).mbr_0x60;
  }
  FUN_004172b0(0,0,(int *)0x0);
  FUN_004172b0(1,0,(int *)0x0);
  if ((*(int *)(dVar6 + 0x7c) < 2) || (*(int *)(dVar6 + 0x8c) != 0)) {
    iVar3 = (**(code **)(**(int **)(dVar6 + 0x94) + 4))(*(int **)(dVar6 + 0x94));
    while (iVar3 = iVar3 + -1, iVar3 != 0) {
      (**(code **)(**(int **)(dVar6 + 0x94) + 8))(*(int **)(dVar6 + 0x94));
    }
    for (iVar3 = (**(code **)(**(int **)(dVar6 + 0x90) + 4))(*(int **)(dVar6 + 0x90)); iVar3 != 0;
        iVar3 = iVar3 + -1) {
      (**(code **)(**(int **)(dVar6 + 0x90) + 8))(*(int **)(dVar6 + 0x90));
    }
  }
  *(undefined4 *)(dVar6 + 0x94) = 0;
  *(undefined4 *)(dVar6 + 0x90) = 0;
  if (*(int *)(dVar6 + 0x84) != 0) {
    if (0 < *(int *)(dVar6 + 0x7c)) {
      iVar3 = 0;
      do {
        piVar1 = *(int **)(iVar3 + *(int *)(dVar6 + 0x88));
        iVar4 = (**(code **)(*piVar1 + 4))(piVar1);
        while (iVar4 = iVar4 + -1, iVar4 != 0) {
          piVar1 = *(int **)(iVar3 + *(int *)(dVar6 + 0x88));
          (**(code **)(*piVar1 + 8))(piVar1);
        }
        piVar1 = *(int **)(*(int *)(dVar6 + 0x84) + iVar3);
        for (iVar4 = (**(code **)(*piVar1 + 4))(piVar1); iVar4 != 0; iVar4 = iVar4 + -1) {
          piVar1 = *(int **)(*(int *)(dVar6 + 0x84) + iVar3);
          (**(code **)(*piVar1 + 8))(piVar1);
        }
        iVar5 = iVar5 + 1;
        iVar3 = iVar3 + 4;
      } while (iVar5 < *(int *)(dVar6 + 0x7c));
    }
    FUN_004830f0(*(LPCVOID *)(dVar6 + 0x84));
    FUN_004830f0(*(LPCVOID *)(dVar6 + 0x88));
    *(undefined4 *)(dVar6 + 0x84) = 0;
    *(undefined4 *)(dVar6 + 0x88) = 0;
  }
  *(undefined4 *)(dVar6 + 0x7c) = 0;
  *(undefined4 *)(dVar6 + 0x80) = 0;
  if ((-1 < in_stack_00000004) &&
     (pvVar2 = *(LPCVOID *)((this->cls_0x5a486c).mbr_0x5c + in_stack_00000004 * 4),
     pvVar2 != (LPCVOID)0x0)) {
    FUN_004830f0(pvVar2);
  }
  cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)&(this->cls_0x5a486c).mbr_0x4c);
  return;
}



// Function at 0040cc40

undefined4 cls_0x5a35ac::meth_0x40cc40(uint param_1, uint param_2, int *param_3, uint *param_4, uint *param_5, uint *param_6, uint *param_7)

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

int cls_0x5a35ac::virt_meth_0x40ce60()

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

undefined4 cls_0x5a35ac::virt_meth_0x40cef0()

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

void cls_0x5a35ac::virt_meth_0x40cf50()

{
  byte *pbVar1;
  byte *pbVar2;
  int iVar3;
  uint uVar4;
  cls_0x5a486c *extraout_ECX;
  cls_0x5a486c *extraout_ECX_00;
  cls_0x5a486c *extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  uint extraout_ECX_04;
  uint extraout_ECX_05;
  uint *puVar5;
  int *piVar6;
  int in_stack_00000008;
  
  if ((this->cls_0x5a486c).mbr_0xc == 0) {
    if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
      cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
    }
    iVar3 = meth_0x407510(this);
    this = (cls_0x5a35ac *)extraout_ECX;
    if (iVar3 == 0) {
      FUN_00481c10((byte *)s_Unable_to_initialize_3D_imagery_f_005c6020);
      this = (cls_0x5a35ac *)extraout_ECX_00;
    }
  }
  iVar3 = 0;
  pbVar1 = **(byte ***)(in_stack_00000008 + 0x4c);
  if (0 < DAT_005e872c) {
    puVar5 = &DAT_005e851c;
    do {
      uVar4 = FUN_0059a530_stricmp(*puVar5,pbVar1,*(byte **)(*puVar5 + 4));
      if (uVar4 == 0) {
        this = (cls_0x5a35ac *)(&DAT_005e851c)[iVar3];
        if ((cls_0x5a486c *)this != (cls_0x5a486c *)&DAT_005e8508) goto LAB_0040d076;
        break;
      }
      iVar3 = iVar3 + 1;
      puVar5 = puVar5 + 1;
      this = (cls_0x5a35ac *)extraout_ECX_01;
    } while (iVar3 < DAT_005e872c);
  }
  pbVar2 = *(byte **)(*(int *)(in_stack_00000008 + 0x48) + 4);
  uVar4 = FUN_0059a530_stricmp((uint)this,pbVar2,(byte *)s_weapon_005c6048);
  if ((((uVar4 == 0) ||
       (uVar4 = FUN_0059a530_stricmp(extraout_ECX_02,pbVar2,(byte *)s_armor_005c6050), uVar4 == 0)) ||
      (uVar4 = FUN_0059a530_stricmp(extraout_ECX_03,pbVar2,(byte *)s_lightsource_005c6058), uVar4 == 0)) ||
     ((uVar4 = FUN_0059a530_stricmp(extraout_ECX_04,pbVar2,&DAT_005c6064), uVar4 == 0 &&
      (uVar4 = FUN_0059a530_stricmp(extraout_ECX_05,pbVar1,(byte *)s_arrow_005c606c), uVar4 == 0)))) {
    iVar3 = 0;
    if (0 < DAT_005e872c) {
      piVar6 = &DAT_005e851c;
      do {
        uVar4 = FUN_0059a530_stricmp(*(uint *)(*piVar6 + 4),(byte *)s_equip_005c6074,
                             (byte *)*(uint *)(*piVar6 + 4));
        if (uVar4 == 0) {
LAB_0040d0bc:
          (***(code ***)(&DAT_005e851c)[iVar3])(in_stack_00000008);
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
      puVar5 = &DAT_005e851c;
      do {
        uVar4 = FUN_0059a530_stricmp(*puVar5,pbVar2,*(byte **)(*puVar5 + 4));
        if (uVar4 == 0) goto LAB_0040d0bc;
        iVar3 = iVar3 + 1;
        puVar5 = puVar5 + 1;
      } while (iVar3 < DAT_005e872c);
    }
  }
  this = (cls_0x5a35ac *)&DAT_005e8508;
LAB_0040d076:
  (*((this->cls_0x5a486c).vftptr_0x0)->~cls_0x5a486c_0)(&this->cls_0x5a486c);
  return;
}



// Function at 00410130

cls_0x5a35ac * cls_0x5a35ac::virt_meth_0x410130()

{
  byte in_stack_00000004;
  
  ~cls_0x5a35ac(this);
  if ((in_stack_00000004 & 1) != 0) {
    FUN_004830f0(this);
  }
  return this;
}



// Function at 004109d0

void cls_0x5a35ac::meth_0x4109d0()

{
  if (*(int *)((this->cls_0x5a486c).mbr_0x4 + 0x60) == 0) {
    cls_0x5a486c::meth_0x447ac0(&this->cls_0x5a486c);
  }
  return;
}



// Function at 004191b0

void cls_0x5a35ac::meth_0x4191b0()

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
            FUN_004830f0(*(LPCVOID *)(*(int *)(iVar4 + (this->cls_0x5a486c).mbr_0x10) + 8));
            FUN_004830f0(*(LPCVOID *)(iVar4 + (this->cls_0x5a486c).mbr_0x10));
            iVar5 = unaff_EDI;
            iVar6 = unaff_ESI;
          }
          iVar6 = iVar6 + 1;
          iVar4 = iVar4 + 4;
        } while (iVar6 < iVar5);
      }
      FUN_004830f0((LPCVOID)(this->cls_0x5a486c).mbr_0x10);
      (this->cls_0x5a486c).mbr_0x10 = 0;
    }
  }
  return;
}



// Function at 00446260

void cls_0x5a35ac::meth_0x446260()

{
  (this->cls_0x5a486c).vftptr_0x0 = &cls_0x5a486c__vftable_5a486c_005a486c;
  cls_0x5a486c::meth_0x447d80(&this->cls_0x5a486c);
  return;
}



