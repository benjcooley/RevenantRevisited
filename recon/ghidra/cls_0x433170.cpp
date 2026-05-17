// Decompiled methods and structure for class: cls_0x433170

/*
/OOAnalyzer/cls_0x433170
pack(disabled)
Structure cls_0x433170 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
   28   dword   4   mbr_0x1c   "Unsigned Double-Word (ddw, 4-bytes)"
   32   dword   4   mbr_0x20   "Unsigned Double-Word (ddw, 4-bytes)"
   36   dword   4   mbr_0x24   "Unsigned Double-Word (ddw, 4-bytes)"
   40   dword   4   mbr_0x28   "Unsigned Double-Word (ddw, 4-bytes)"
   48   dword   4   mbr_0x30   "Unsigned Double-Word (ddw, 4-bytes)"
   52   dword   4   mbr_0x34   "Unsigned Double-Word (ddw, 4-bytes)"
   56   dword   4   mbr_0x38   "Unsigned Double-Word (ddw, 4-bytes)"
   60   dword   4   mbr_0x3c   "Unsigned Double-Word (ddw, 4-bytes)"
   64   dword   4   mbr_0x40   "Unsigned Double-Word (ddw, 4-bytes)"
   68   dword   4   mbr_0x44   "Unsigned Double-Word (ddw, 4-bytes)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   dword   4   mbr_0x54   "Unsigned Double-Word (ddw, 4-bytes)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   108   dword   4   mbr_0x6c   "Unsigned Double-Word (ddw, 4-bytes)"
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
   120   dword   4   mbr_0x78   "Unsigned Double-Word (ddw, 4-bytes)"
   124   dword   4   mbr_0x7c   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 128 Alignment: 1

*/

// Function at 00433170

/* WARNING: Removing unreachable block (ram,0x004332a9) */
/* WARNING: Removing unreachable block (ram,0x004332b2) */
/* WARNING: Removing unreachable block (ram,0x004332b4) */

undefined4 __thiscall
OOAnalyzer::cls_0x433170::meth_0x433170_DefWidget_DROPLIST_attr_parser(cls_0x433170 *this,cls_0x433170 *param_1,int *param_2)

{
  char cVar1;
  int iVar2;
  bool bVar3;
  undefined3 extraout_var;
  undefined4 uVar4;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  dword dVar5;
  undefined3 extraout_var_04;
  undefined3 extraout_var_05;
  dword *pdVar6;
  int iVar7;
  uint uVar8;
  cls_0x433170 *pcVar9;
  cls_0x433170 *pcVar10;
  undefined4 *puVar11;
  char *pcVar12;
  uint *in_stack_0000000c;
  char *pcVar13;
  char local_a0 [70];
  char acStack_5a [8];
  char acStack_52 [82];
  
  if (param_1 == (cls_0x433170 *)0x0) {
    pcVar9 = this;
    for (iVar7 = 0x5d; iVar7 != 0; iVar7 = iVar7 + -1) {
      pcVar9->mbr_0x0 = 0;
      pcVar9 = (cls_0x433170 *)&pcVar9->mbr_0x4;
    }
    this->mbr_0x78 = 200;
    this->mbr_0x4c = 0x80000000;
    this->mbr_0x64 = 0x80000000;
  }
  else if (this != param_1) {
    pcVar9 = param_1;
    pcVar10 = this;
    for (iVar7 = 0x5d; iVar7 != 0; iVar7 = iVar7 + -1) {
      pcVar10->mbr_0x0 = pcVar9->mbr_0x0;
      pcVar9 = (cls_0x433170 *)&pcVar9->mbr_0x4;
      pcVar10 = (cls_0x433170 *)&pcVar10->mbr_0x4;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_BGBITMAP_005cd7a4,0);
  if (CONCAT31(extraout_var,bVar3) == 0) goto LAB_00433367;
  FUN_00479580((byte *)in_stack_0000000c);
  iVar7 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd9ac);
  if (iVar7 == 0) {
    iVar7 = *param_2;
    pcVar13 = s_Bitmap_name_expected_005cd9b4;
    pcVar12 = (char *)0x0;
LAB_00433223:
    iVar7 = (**(code **)(iVar7 + 0x98))(pcVar13,pcVar12);
    if (iVar7 == 0) {
      return 0;
    }
  }
  else {
    dVar5 = (**(code **)(*param_2 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x10 = dVar5;
    if (dVar5 == 0) {
      iVar7 = *param_2;
      pcVar12 = local_a0;
      pcVar13 = s_Unable_to_find_bitmap___s__in_re_005cd9cc;
      goto LAB_00433223;
    }
  }
  iVar7 = FUN_0047a410(in_stack_0000000c,(float **)s_<RECT__d__d__d__d>_005cd7b0);
  if (iVar7 == 0) {
    uVar4 = (**(code **)(*param_2 + 0x9c))(s_BGBITMAP_005cd7c4);
    return uVar4;
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_FRAME_005cd7d0,0);
  if (CONCAT31(extraout_var_00,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    this->mbr_0x24 = 1;
    if (this == (cls_0x433170 *)0xffffffd8) {
      puVar11 = (undefined4 *)0x0;
      for (iVar7 = 9; iVar7 != 0; iVar7 = iVar7 + -1) {
        *puVar11 = 0;
        puVar11 = puVar11 + 1;
      }
    }
    iVar7 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i__i__i_<MARGINS__i__i__i__i_005ccf24);
    if ((iVar7 == 0) &&
       (iVar7 = (**(code **)(*param_2 + 0x98))(s_Error_parsing_frame_data_005ccf54,0), iVar7 == 0))
    {
      return 0;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DRAWMODE_005cd7d8,0);
  if (CONCAT31(extraout_var_01,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar7 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd7e4);
    if (iVar7 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))(s_FRAME_005cd7e8);
      return uVar4;
    }
  }
LAB_00433367:
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_ITEMRECT_005cd7f0,0);
  if (CONCAT31(extraout_var_02,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar7 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i__i__i_005cd7fc);
    if (iVar7 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))(s_ITEMRECT_005cd808);
      return uVar4;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DROPBUTTON_005cd814,0);
  if (CONCAT31(extraout_var_03,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar7 = FUN_0047a410(in_stack_0000000c,(float **)s__78s_<RELR>__i__i_<DRAWMODE__d>_005cd820);
    if (iVar7 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))(s_DROPBUTTON_005cd840);
      return uVar4;
    }
    _strncpy(local_a0,acStack_52 + 2,0x4f);
    uVar8 = 0xffffffff;
    acStack_52[1] = 0;
    pcVar12 = local_a0;
    do {
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      cVar1 = *pcVar12;
      pcVar12 = pcVar12 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar8 - 1);
    _strncpy(local_a0 + (~uVar8 - 1),&DAT_005cd84c,iVar2 + 0x4f);
    iVar7 = *param_2;
    (local_a0 + (~uVar8 - 1))[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar7 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x50 = dVar5;
    _strncpy(&stack0xffffff58,acStack_5a + 2,0x4f);
    uVar8 = 0xffffffff;
    acStack_5a[1] = 0;
    pcVar12 = &stack0xffffff58;
    do {
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      cVar1 = *pcVar12;
      pcVar12 = pcVar12 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar8 - 1);
    _strncpy(local_a0 + (~uVar8 - 9),&DAT_005cd850,iVar2 + 0x4f);
    iVar7 = *param_2;
    (local_a0 + (~uVar8 - 9))[iVar2 + 0x4f] = '\0';
    dVar5 = (**(code **)(iVar7 + 0xa0))(&stack0xffffff58,0xffffffff);
    this->mbr_0x54 = dVar5;
    if ((this->mbr_0x64 == 0x80000000) && (this->mbr_0x50 != 0)) {
      this->mbr_0x64 = *(uint *)(this->mbr_0x50 + 0x14) & 0xfffdffff | 0x20;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_LISTHEIGHT_005cd854,0);
  if (CONCAT31(extraout_var_04,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar7 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd860);
    if (iVar7 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))(s_LISTHEIGHT_005cd864);
      return uVar4;
    }
  }
  bVar3 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_LISTBOX_005cd870,0);
  if (CONCAT31(extraout_var_05,bVar3) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar7 = FUN_0047a410(in_stack_0000000c,(float **)s__i__i__i__i_005cd878);
    if (iVar7 == 0) {
      uVar4 = (**(code **)(*param_2 + 0x9c))(s_LISTBOX_005cd884);
      return uVar4;
    }
    if (param_1 == (cls_0x433170 *)0x0) {
      pdVar6 = (dword *)0x0;
    }
    else {
      pdVar6 = &this->mbr_0x7c;
    }
    iVar7 = cls_0x42f9f0::meth_0x42f9f0_DefWidget_LISTBOX_attr_parser((cls_0x42f9f0 *)&this->mbr_0x7c,pdVar6,param_2);
    if (iVar7 == 0) {
      return 0;
    }
  }
  return 1;
}



