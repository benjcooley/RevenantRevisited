// Decompiled methods and structure for class: cls_0x42bd90

/*
/OOAnalyzer/cls_0x42bd90
pack(disabled)
Structure cls_0x42bd90 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   44   dword   4   mbr_0x2c   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   108   dword   4   mbr_0x6c   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
   140   dword   4   mbr_0x8c   "Unsigned Double-Word (ddw, 4-bytes)"
   144   dword   4   mbr_0x90   "Unsigned Double-Word (ddw, 4-bytes)"
   148   dword   4   mbr_0x94   "Unsigned Double-Word (ddw, 4-bytes)"
   152   dword   4   mbr_0x98   "Unsigned Double-Word (ddw, 4-bytes)"
   156   dword   4   mbr_0x9c   "Unsigned Double-Word (ddw, 4-bytes)"
   160   dword   4   mbr_0xa0   "Unsigned Double-Word (ddw, 4-bytes)"
   164   dword   4   mbr_0xa4   "Unsigned Double-Word (ddw, 4-bytes)"
   168   dword   4   mbr_0xa8   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 172 Alignment: 1

*/

// Function at 0042bd90

undefined4 __thiscall
OOAnalyzer::cls_0x42bd90::meth_0x42bd90(cls_0x42bd90 *this,cls_0x42bd90 *param_1,int *param_2)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  dword dVar2;
  undefined3 extraout_var_03;
  undefined3 extraout_var_04;
  undefined4 uVar3;
  undefined3 extraout_var_05;
  dword *pdVar4;
  undefined3 extraout_var_06;
  undefined3 extraout_var_07;
  undefined3 extraout_var_08;
  undefined3 extraout_var_09;
  undefined3 extraout_var_10;
  int iVar5;
  cls_0x42bd90 *pcVar6;
  cls_0x42bd90 *pcVar7;
  dword *pdVar8;
  uint *in_stack_0000000c;
  char *pcVar9;
  undefined *puVar10;
  undefined local_50 [80];
  
  if (param_1 == (cls_0x42bd90 *)0x0) {
    pcVar6 = this;
    for (iVar5 = 0x2b; iVar5 != 0; iVar5 = iVar5 + -1) {
      pcVar6->mbr_0x0 = 0;
      pcVar6 = (cls_0x42bd90 *)&pcVar6->mbr_0x4;
    }
    this->mbr_0x38 = 0x80000000;
  }
  else if (this != param_1) {
    pcVar6 = param_1;
    pcVar7 = this;
    for (iVar5 = 0x2b; iVar5 != 0; iVar5 = iVar5 + -1) {
      pcVar7->mbr_0x0 = pcVar6->mbr_0x0;
      pcVar6 = (cls_0x42bd90 *)&pcVar6->mbr_0x4;
      pcVar7 = (cls_0x42bd90 *)&pcVar7->mbr_0x4;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd1fc,0);
  if (CONCAT31(extraout_var,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x10 = 0;
    iVar5 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd9ac);
    if (iVar5 == 0) {
      iVar5 = (**(code **)(*param_2 + 0x98))(s_Bitmap_name_expected_005cd9b4,0);
    }
    else {
      dVar2 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
      this->mbr_0x0 = dVar2;
      if (dVar2 != 0) goto LAB_0042be58;
      iVar5 = (**(code **)(*param_2 + 0x98))(s_Unable_to_find_bitmap___s__in_re_005cd9cc,local_50);
    }
    if (iVar5 == 0) {
      return 0;
    }
  }
LAB_0042be58:
  bVar1 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd200,0);
  if (CONCAT31(extraout_var_00,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x10 = 0;
    iVar5 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd9ac);
    if (iVar5 == 0) {
      iVar5 = *param_2;
      puVar10 = (undefined *)0x0;
      pcVar9 = s_Bitmap_name_expected_005cd9b4;
    }
    else {
      dVar2 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
      this->mbr_0x4 = dVar2;
      if (dVar2 != 0) goto LAB_0042bed2;
      iVar5 = *param_2;
      puVar10 = local_50;
      pcVar9 = s_Unable_to_find_bitmap___s__in_re_005cd9cc;
    }
    iVar5 = (**(code **)(iVar5 + 0x98))(pcVar9,puVar10);
    if (iVar5 == 0) {
      return 0;
    }
  }
LAB_0042bed2:
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)&PTR_LAB_005cd208,0);
  if (CONCAT31(extraout_var_01,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x10 = 0;
    iVar5 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd9ac);
    if (iVar5 == 0) {
      iVar5 = *param_2;
      puVar10 = (undefined *)0x0;
      pcVar9 = s_Bitmap_name_expected_005cd9b4;
    }
    else {
      dVar2 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
      this->mbr_0x8 = dVar2;
      if (dVar2 != 0) goto LAB_0042bf4a;
      iVar5 = *param_2;
      puVar10 = local_50;
      pcVar9 = s_Unable_to_find_bitmap___s__in_re_005cd9cc;
    }
    iVar5 = (**(code **)(iVar5 + 0x98))(pcVar9,puVar10);
    if (iVar5 == 0) {
      return 0;
    }
  }
LAB_0042bf4a:
  bVar1 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd20c,0);
  if (CONCAT31(extraout_var_02,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x10 = 0;
    iVar5 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd9ac);
    if (iVar5 == 0) {
      iVar5 = *param_2;
      puVar10 = (undefined *)0x0;
      pcVar9 = s_Bitmap_name_expected_005cd9b4;
    }
    else {
      dVar2 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
      this->mbr_0xc = dVar2;
      if (dVar2 != 0) goto LAB_0042bfc2;
      iVar5 = *param_2;
      puVar10 = local_50;
      pcVar9 = s_Unable_to_find_bitmap___s__in_re_005cd9cc;
    }
    iVar5 = (**(code **)(iVar5 + 0x98))(pcVar9,puVar10);
    if (iVar5 == 0) {
      return 0;
    }
  }
LAB_0042bfc2:
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_FRAME_005cd214,0);
  if (CONCAT31(extraout_var_03,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x10 = 1;
    if (param_1 == (cls_0x42bd90 *)0x0) {
      pdVar4 = (dword *)0x0;
    }
    else {
      pdVar4 = &this->mbr_0x14;
    }
    pdVar8 = &this->mbr_0x14;
    if (pdVar4 == (dword *)0x0) {
      for (iVar5 = 9; iVar5 != 0; iVar5 = iVar5 + -1) {
        *pdVar8 = 0;
        pdVar8 = pdVar8 + 1;
      }
    }
    else if (pdVar8 != pdVar4) {
      for (iVar5 = 9; iVar5 != 0; iVar5 = iVar5 + -1) {
        *pdVar8 = *pdVar4;
        pdVar4 = pdVar4 + 1;
        pdVar8 = pdVar8 + 1;
      }
    }
    iVar5 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i__i__i_<MARGINS__i__i__i__i_005ccf24);
    if ((iVar5 == 0) &&
       (iVar5 = (**(code **)(*param_2 + 0x98))(s_Error_parsing_frame_data_005ccf54,0), iVar5 == 0))
    {
      return 0;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DRAWMODE_005cd21c,0);
  if (CONCAT31(extraout_var_04,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar5 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd228);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_DRAWMODE_005cd22c);
      return uVar3;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_UPLABEL_005cd238,0);
  if (CONCAT31(extraout_var_05,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x3c = 1;
    if (param_1 == (cls_0x42bd90 *)0x0) {
      pdVar4 = (dword *)0x0;
    }
    else {
      pdVar4 = &this->mbr_0x4c;
    }
    iVar5 = cls_0x429fd0::meth_0x429fd0((cls_0x429fd0 *)&this->mbr_0x4c,pdVar4,param_2);
    if (iVar5 == 0) {
      return 0;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DOWNLABEL_005cd240,0);
  if (CONCAT31(extraout_var_06,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x40 = 1;
    pdVar4 = &this->mbr_0x5c;
    if (param_1 == (cls_0x42bd90 *)0x0) {
      pdVar4 = &this->mbr_0x4c;
    }
    iVar5 = cls_0x429fd0::meth_0x429fd0((cls_0x429fd0 *)&this->mbr_0x5c,pdVar4,param_2);
    if (iVar5 == 0) {
      return 0;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_SELLABEL_005cd24c,0);
  if (CONCAT31(extraout_var_07,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x44 = 1;
    pdVar4 = &this->mbr_0x6c;
    if (param_1 == (cls_0x42bd90 *)0x0) {
      pdVar4 = &this->mbr_0x4c;
    }
    iVar5 = cls_0x429fd0::meth_0x429fd0((cls_0x429fd0 *)&this->mbr_0x6c,pdVar4,param_2);
    if (iVar5 == 0) {
      return 0;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_GRAYLABEL_005cd258,0);
  if (CONCAT31(extraout_var_08,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x48 = 1;
    pdVar4 = &this->mbr_0x7c;
    if (param_1 == (cls_0x42bd90 *)0x0) {
      pdVar4 = &this->mbr_0x4c;
    }
    iVar5 = cls_0x429fd0::meth_0x429fd0((cls_0x429fd0 *)&this->mbr_0x7c,pdVar4,param_2);
    if (iVar5 == 0) {
      return 0;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_UPLABELRECT_005cd264,0);
  if (CONCAT31(extraout_var_09,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar5 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i__i__i_005cd270);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_UPLABELRECT_005cd27c);
      return uVar3;
    }
    this->mbr_0x9c = this->mbr_0x8c;
    this->mbr_0xa0 = this->mbr_0x90;
    this->mbr_0xa4 = this->mbr_0x94;
    this->mbr_0xa8 = this->mbr_0x98;
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DOWNLABELRECT_005cd288,0);
  if (CONCAT31(extraout_var_10,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar5 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i__i__i_005cd298);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_DOWNLABELRECT_005cd2a4);
      return uVar3;
    }
  }
  return 1;
}



