#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x42d730



// Function at 0042d730

/* WARNING: Removing unreachable block (ram,0x0042d82f) */
/* WARNING: Removing unreachable block (ram,0x0042d838) */
/* WARNING: Removing unreachable block (ram,0x0042d83a) */

undefined4 cls_0x42d730::meth_0x42d730(cls_0x42d730 *param_1, int *param_2)

{
  char cVar1;
  int iVar2;
  bool bVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined4 uVar4;
  undefined3 extraout_var_02;
  dword dVar5;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined3 extraout_var_05;
  undefined3 extraout_var_06;
  undefined3 extraout_var_07;
  int iVar6;
  uint uVar7;
  cls_0x42d730 *pcVar8;
  undefined4 *puVar9;
  char *pcVar10;
  uint *in_stack_0000000c;
  char *pcVar11;
  undefined4 uStack_a4;
  char local_a0 [70];
  char acStack_5a [9];
  undefined uStack_51;
  char acStack_50 [80];
  
  if (param_1 == (cls_0x42d730 *)0x0) {
    pcVar8 = this;
    for (iVar6 = 0x21; iVar6 != 0; iVar6 = iVar6 + -1) {
      pcVar8->mbr_0x0 = 0;
      pcVar8 = (cls_0x42d730 *)&pcVar8->mbr_0x4;
    }
    this->mbr_0x80 = 0x80000000;
    this->mbr_0x64 = 0x80000000;
    this->mbr_0x48 = 0x80000000;
    this->mbr_0x2c = 0x80000000;
  }
  else if (this != param_1) {
    pcVar8 = this;
    for (iVar6 = 0x21; iVar6 != 0; iVar6 = iVar6 + -1) {
      pcVar8->mbr_0x0 = param_1->mbr_0x0;
      param_1 = (cls_0x42d730 *)&param_1->mbr_0x4;
      pcVar8 = (cls_0x42d730 *)&pcVar8->mbr_0x4;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_BGBITMAP_005cd2f8,0);
  if (CONCAT31(extraout_var,bVar3) == 0) goto LAB_0042d8ef;
  FUN_00479580((byte *)in_stack_0000000c);
  iVar6 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd9ac);
  if (iVar6 == 0) {
    iVar6 = *param_2;
    pcVar11 = s_Bitmap_name_expected_005cd9b4;
    pcVar10 = (char *)0x0;
LAB_0042d7e7:
    iVar6 = (**(code **)(iVar6 + 0x98))(pcVar11,pcVar10);
    if (iVar6 == 0) {
      return 0;
    }
  }
  else {
    dVar5 = (**(code **)(*param_2 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x0 = dVar5;
    if (dVar5 == 0) {
      iVar6 = *param_2;
      pcVar10 = local_a0;
      pcVar11 = s_Unable_to_find_bitmap___s__in_re_005cd9cc;
      goto LAB_0042d7e7;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_FRAME_005cd304,0);
  if (CONCAT31(extraout_var_00,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x4 = 1;
    if (this == (cls_0x42d730 *)0xfffffff8) {
      puVar9 = (undefined4 *)0x0;
      for (iVar6 = 9; iVar6 != 0; iVar6 = iVar6 + -1) {
        *puVar9 = 0;
        puVar9 = puVar9 + 1;
      }
    }
    iVar6 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i__i__i_<MARGINS__i__i__i__i_005ccf24);
    if ((iVar6 == 0) &&
       (iVar6 = (**(code **)(*param_2 + 0x98))(s_Error_parsing_frame_data_005ccf54,0), iVar6 == 0))
    {
      return 0;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DRAWMODE_005cd30c,0);
  if (CONCAT31(extraout_var_01,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar6 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd318);
    if (iVar6 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))(s_FRAME_005cd31c);
      return uVar4;
    }
  }
LAB_0042d8ef:
  bVar3 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd324,0);
  if (CONCAT31(extraout_var_02,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar6 = FUN_0047a410(in_stack_0000000c,(float **)s__78s_<RELR>__i_<RELB>__i_005cd328);
    if (iVar6 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd344);
      return uVar4;
    }
    _strncpy(local_a0,acStack_50,0x4f);
    uVar7 = 0xffffffff;
    uStack_51 = 0;
    pcVar10 = local_a0;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    uStack_a4 = local_a0 + (~uVar7 - 1);
    _strncpy(uStack_a4,&DAT_005cd348,iVar2 + 0x4f);
    iVar6 = *param_2;
    uStack_a4[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar6 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x30 = dVar5;
    _strncpy(&stack0xffffff58,acStack_5a + 2,0x4f);
    uVar7 = 0xffffffff;
    acStack_5a[1] = 0;
    pcVar10 = &stack0xffffff58;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    _strncpy(local_a0 + (~uVar7 - 9),&DAT_005cd34c,iVar2 + 0x4f);
    iVar6 = *param_2;
    (local_a0 + (~uVar7 - 9))[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar6 + 0xa0))(&stack0xffffff58,0xffffffff);
    this->mbr_0x34 = dVar5;
    bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DRAWMODE_005cd350,0);
    if (CONCAT31(extraout_var_03,bVar3) != 0) {
      FUN_00479580((byte *)in_stack_0000000c);
      iVar6 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd35c);
      if (iVar6 == 0) {
        uVar4 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd360);
        return uVar4;
      }
    }
    if ((this->mbr_0x48 == 0x80000000) && (this->mbr_0x30 != 0)) {
      this->mbr_0x48 = *(uint *)(this->mbr_0x30 + 0x14) & 0xfffdffff | 0x20;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd364,0);
  if (CONCAT31(extraout_var_04,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar6 = FUN_0047a410(in_stack_0000000c,(float **)s__78s_<RELR>__i_<RELB>__i_005cd36c);
    if (iVar6 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd388);
      return uVar4;
    }
    _strncpy(local_a0,acStack_50,0x4f);
    uVar7 = 0xffffffff;
    uStack_51 = 0;
    pcVar10 = local_a0;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    uStack_a4 = local_a0 + (~uVar7 - 1);
    _strncpy(uStack_a4,&DAT_005cd390,iVar2 + 0x4f);
    iVar6 = *param_2;
    uStack_a4[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar6 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x4c = dVar5;
    _strncpy(&stack0xffffff58,acStack_5a + 2,0x4f);
    uVar7 = 0xffffffff;
    acStack_5a[1] = 0;
    pcVar10 = &stack0xffffff58;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    _strncpy(local_a0 + (~uVar7 - 9),&DAT_005cd394,iVar2 + 0x4f);
    iVar6 = *param_2;
    (local_a0 + (~uVar7 - 9))[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar6 + 0xa0))(&stack0xffffff58,0xffffffff);
    this->mbr_0x50 = dVar5;
    bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DRAWMODE_005cd398,0);
    if (CONCAT31(extraout_var_05,bVar3) != 0) {
      FUN_00479580((byte *)in_stack_0000000c);
      iVar6 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd3a4);
      if (iVar6 == 0) {
        uVar4 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd3a8);
        return uVar4;
      }
    }
    if ((this->mbr_0x64 == 0x80000000) && (this->mbr_0x4c != 0)) {
      this->mbr_0x64 = *(uint *)(this->mbr_0x4c + 0x14) & 0xfffdffff | 0x20;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_THUMB_005cd3b0,0);
  if (CONCAT31(extraout_var_06,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar6 = FUN_0047a410(in_stack_0000000c,(float **)s__78s__i__i_<RELBR>__i_005cd3b8);
    if (iVar6 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))(s_THUMB_005cd3d0);
      return uVar4;
    }
    _strncpy(local_a0,acStack_50,0x4f);
    uVar7 = 0xffffffff;
    uStack_51 = 0;
    pcVar10 = local_a0;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    uStack_a4 = local_a0 + (~uVar7 - 1);
    _strncpy(uStack_a4,&DAT_005cd3d8,iVar2 + 0x4f);
    iVar6 = *param_2;
    uStack_a4[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar6 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x68 = dVar5;
    _strncpy(&stack0xffffff58,acStack_5a + 2,0x4f);
    uVar7 = 0xffffffff;
    acStack_5a[1] = 0;
    pcVar10 = &stack0xffffff58;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    _strncpy(local_a0 + (~uVar7 - 9),&DAT_005cd3dc,iVar2 + 0x4f);
    iVar6 = *param_2;
    (local_a0 + (~uVar7 - 9))[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar6 + 0xa0))(&stack0xffffff58,0xffffffff);
    this->mbr_0x6c = dVar5;
    bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DRAWMODE_005cd3e0,0);
    if (CONCAT31(extraout_var_07,bVar3) != 0) {
      FUN_00479580((byte *)in_stack_0000000c);
      iVar6 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd3ec);
      if (iVar6 == 0) {
        uVar4 = (**(code **)(*param_2 + 0x9c))(s_THUMB_005cd3f0);
        return uVar4;
      }
    }
    if ((this->mbr_0x80 == 0x80000000) && (this->mbr_0x68 != 0)) {
      this->mbr_0x80 = *(uint *)(this->mbr_0x68 + 0x14) & 0xfffdffff | 0x20;
    }
  }
  return 1;
}



