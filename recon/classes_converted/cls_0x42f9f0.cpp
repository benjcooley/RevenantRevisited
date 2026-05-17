#include "classes_all.h"

// Decompiled methods and structure for class: cls_0x42f9f0



// Function at 0042f9f0

/* WARNING: Removing unreachable block (ram,0x0042fb3f) */
/* WARNING: Removing unreachable block (ram,0x0042fb48) */
/* WARNING: Removing unreachable block (ram,0x0042fb4a) */

undefined4 cls_0x42f9f0::meth_0x42f9f0_DefWidget_LISTBOX_attr_parser(cls_0x42f9f0 *param_1, int *param_2)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined4 uVar2;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined3 extraout_var_05;
  dword dVar3;
  undefined3 extraout_var_06;
  int iVar4;
  cls_0x42f9f0 *pcVar5;
  undefined4 *puVar6;
  uint *in_stack_0000000c;
  undefined local_a0 [160];
  
  if (param_1 == (cls_0x42f9f0 *)0x0) {
    pcVar5 = this;
    for (iVar4 = 0x3e; iVar4 != 0; iVar4 = iVar4 + -1) {
      pcVar5->mbr_0x0 = 0;
      pcVar5 = (cls_0x42f9f0 *)&pcVar5->mbr_0x4;
    }
    this->mbr_0x30 = 0x14;
    this->mbr_0x34 = 0x14;
    dVar3 = DAT_0065caec;
    this->mbr_0x50 = 1;
    this->mbr_0x4c = dVar3;
    this->mbr_0x54 = 0xff;
    this->mbr_0x55 = 0xff;
    this->mbr_0x56 = 0xff;
    this->mbr_0x58 = 0x80000000;
    this->mbr_0x2c = 0x80000000;
    this->mbr_0x4a = 0;
    this->mbr_0x49 = 0xb4;
    this->mbr_0x48 = 100;
    this->mbr_0x4b = 0xff;
    this->mbr_0x5c = 0;
  }
  else if (this != param_1) {
    pcVar5 = this;
    for (iVar4 = 0x3e; iVar4 != 0; iVar4 = iVar4 + -1) {
      pcVar5->mbr_0x0 = param_1->mbr_0x0;
      param_1 = (cls_0x42f9f0 *)&param_1->mbr_0x4;
      pcVar5 = (cls_0x42f9f0 *)&pcVar5->mbr_0x4;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_BGBITMAP_005cd51c,0);
  if (CONCAT31(extraout_var,bVar1) == 0) goto LAB_0042fc04;
  FUN_00479580((byte *)in_stack_0000000c);
  iVar4 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd9ac);
  if (iVar4 == 0) {
    iVar4 = (**(code **)(*param_2 + 0x98))(s_Bitmap_name_expected_005cd9b4,0);
LAB_0042faf8:
    if (iVar4 == 0) {
      return 0;
    }
  }
  else {
    dVar3 = (**(code **)(*param_2 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x0 = dVar3;
    if (dVar3 == 0) {
      iVar4 = (**(code **)(*param_2 + 0x98))(s_Unable_to_find_bitmap___s__in_re_005cd9cc,local_a0);
      goto LAB_0042faf8;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_FRAME_005cd528,0);
  if (CONCAT31(extraout_var_00,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x4 = 1;
    if (this == (cls_0x42f9f0 *)0xfffffff8) {
      puVar6 = (undefined4 *)0x0;
      for (iVar4 = 9; iVar4 != 0; iVar4 = iVar4 + -1) {
        *puVar6 = 0;
        puVar6 = puVar6 + 1;
      }
    }
    iVar4 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i__i__i_<MARGINS__i__i__i__i_005ccf24);
    if ((iVar4 == 0) &&
       (iVar4 = (**(code **)(*param_2 + 0x98))(s_Error_parsing_frame_data_005ccf54,0), iVar4 == 0))
    {
      return 0;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DRAWMODE_005cd530,0);
  if (CONCAT31(extraout_var_01,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar4 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd53c);
    if (iVar4 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(s_FRAME_005cd540);
      return uVar2;
    }
  }
LAB_0042fc04:
  bVar1 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd548,0);
  if (CONCAT31(extraout_var_02,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar4 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i__i__i_005cd550);
    if (iVar4 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd55c);
      return uVar2;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd564,0);
  if (CONCAT31(extraout_var_03,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar4 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i_005cd56c);
    if (iVar4 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd574);
      return uVar2;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_SELCOLOR_005cd57c,0);
  if (CONCAT31(extraout_var_04,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar4 = FUN_0047a410(in_stack_0000000c,(float **)s__b__b__b_005cd588);
    if (iVar4 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(s_SELCOLOR_005cd594);
      return uVar2;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd5a0,0);
  if (CONCAT31(extraout_var_05,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar4 = FUN_0047a410(in_stack_0000000c,(float **)s__30s_<COLOR__b__b__b>_<FLAGS__d>_005cd5a8);
    if (iVar4 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd5d8);
      return uVar2;
    }
    dVar3 = cls_0x45f7c0::meth_0x4acb30((cls_0x45f7c0 *)&UNK_0065abf8.field_0x418);
    this->mbr_0x4c = dVar3;
    if (dVar3 == 0) {
      this->mbr_0x4c = DAT_0065caec;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_SCROLLBAR_005cd5e0,0);
  if (CONCAT31(extraout_var_06,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x5c = 1;
    iVar4 = FUN_0047a410(in_stack_0000000c,(float **)s_<RELRB>__i__i__i__i_005cd5ec);
    if (iVar4 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(s_SCROLLBAR_005cd600);
      return uVar2;
    }
    iVar4 = cls_0x42d730::meth_0x42d730_DefWidget_FRAME_attr_parser((cls_0x42d730 *)&this->mbr_0x74,0,param_2);
    if (iVar4 == 0) {
      return 0;
    }
  }
  return 1;
}



