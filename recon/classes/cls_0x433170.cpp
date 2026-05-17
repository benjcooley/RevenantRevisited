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
  undefined4 uVar3;
  dword dVar4;
  dword *pdVar5;
  int iVar6;
  uint uVar7;
  cls_0x433170 *pcVar8;
  cls_0x433170 *pcVar9;
  undefined4 *puVar10;
  char *pcVar11;
  undefined4 in_stack_0000000c;
  char *pcVar12;
  char local_a0 [70];
  char acStack_5a [8];
  char acStack_52 [82];
  
  if (param_1 == (cls_0x433170 *)0x0) {
    pcVar8 = this;
    for (iVar6 = 0x5d; iVar6 != 0; iVar6 = iVar6 + -1) {
      pcVar8->mbr_0x0 = 0;
      pcVar8 = (cls_0x433170 *)&pcVar8->mbr_0x4;
    }
    this->mbr_0x78 = 200;
    this->mbr_0x4c = 0x80000000;
    this->mbr_0x64 = 0x80000000;
  }
  else if (this != param_1) {
    pcVar8 = param_1;
    pcVar9 = this;
    for (iVar6 = 0x5d; iVar6 != 0; iVar6 = iVar6 + -1) {
      pcVar9->mbr_0x0 = pcVar8->mbr_0x0;
      pcVar8 = (cls_0x433170 *)&pcVar8->mbr_0x4;
      pcVar9 = (cls_0x433170 *)&pcVar9->mbr_0x4;
    }
  }
  iVar6 = FUN_00479700(s_BGBITMAP_005cd7a4,0);
  if (iVar6 == 0) goto LAB_00433367;
  FUN_00479580();
  iVar6 = FUN_0047a410();
  if (iVar6 == 0) {
    iVar6 = *param_2;
    pcVar12 = s_Bitmap_name_expected_005cd9b4;
    pcVar11 = (char *)0x0;
LAB_00433223:
    iVar6 = (**(code **)(iVar6 + 0x98))(pcVar12,pcVar11);
    if (iVar6 == 0) {
      return 0;
    }
  }
  else {
    dVar4 = (**(code **)(*param_2 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x10 = dVar4;
    if (dVar4 == 0) {
      iVar6 = *param_2;
      pcVar11 = local_a0;
      pcVar12 = s_Unable_to_find_bitmap___s__in_re_005cd9cc;
      goto LAB_00433223;
    }
  }
  iVar6 = FUN_0047a410();
  if (iVar6 == 0) {
    uVar3 = (**(code **)(*param_2 + 0x9c))(s_BGBITMAP_005cd7c4);
    return uVar3;
  }
  iVar6 = FUN_00479700(s_FRAME_005cd7d0,0);
  if (iVar6 != 0) {
    FUN_00479580();
    this->mbr_0x24 = 1;
    if (&this->mbr_0x28 == (dword *)0x0) {
      puVar10 = (undefined4 *)0x0;
      for (iVar6 = 9; iVar6 != 0; iVar6 = iVar6 + -1) {
        *puVar10 = 0;
        puVar10 = puVar10 + 1;
      }
    }
    iVar6 = FUN_0047a410(in_stack_0000000c,s__i__i__i__i_<MARGINS__i__i__i__i_005ccf24,
                         &this->field_0x2c,&this->mbr_0x30,&this->mbr_0x34,&this->mbr_0x38,
                         &this->mbr_0x3c,&this->mbr_0x40,&this->mbr_0x44,&this->mbr_0x48,
                         &this->mbr_0x28);
    if ((iVar6 == 0) &&
       (iVar6 = (**(code **)(*param_2 + 0x98))(s_Error_parsing_frame_data_005ccf54,0), iVar6 == 0))
    {
      return 0;
    }
  }
  iVar6 = FUN_00479700(s_DRAWMODE_005cd7d8,0);
  if (iVar6 != 0) {
    FUN_00479580();
    iVar6 = FUN_0047a410(in_stack_0000000c,&DAT_005cd7e4,&this->mbr_0x4c);
    if (iVar6 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_FRAME_005cd7e8);
      return uVar3;
    }
  }
LAB_00433367:
  iVar6 = FUN_00479700(s_ITEMRECT_005cd7f0,0);
  if (iVar6 != 0) {
    FUN_00479580();
    iVar6 = FUN_0047a410(in_stack_0000000c,s__i__i__i__i_005cd7fc,this,&this->mbr_0x4,&this->mbr_0x8
                         ,&this->mbr_0xc);
    if (iVar6 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_ITEMRECT_005cd808);
      return uVar3;
    }
  }
  iVar6 = FUN_00479700(s_DROPBUTTON_005cd814,0);
  if (iVar6 != 0) {
    FUN_00479580();
    iVar6 = FUN_0047a410(in_stack_0000000c,s__78s_<RELR>__i__i_<DRAWMODE__d>_005cd820,acStack_52 + 2
                         ,&this->mbr_0x60,&this->mbr_0x58,&this->mbr_0x5c,&this->mbr_0x64);
    if (iVar6 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_DROPBUTTON_005cd840);
      return uVar3;
    }
    _strncpy(local_a0,acStack_52 + 2,0x4f);
    uVar7 = 0xffffffff;
    acStack_52[1] = 0;
    pcVar11 = local_a0;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar11;
      pcVar11 = pcVar11 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    _strncpy(local_a0 + (~uVar7 - 1),&DAT_005cd84c,iVar2 + 0x4f);
    iVar6 = *param_2;
    (local_a0 + (~uVar7 - 1))[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar6 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x50 = dVar4;
    _strncpy(&stack0xffffff58,acStack_5a + 2,0x4f);
    uVar7 = 0xffffffff;
    acStack_5a[1] = 0;
    pcVar11 = &stack0xffffff58;
    do {
      if (uVar7 == 0) break;
      uVar7 = uVar7 - 1;
      cVar1 = *pcVar11;
      pcVar11 = pcVar11 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar7 - 1);
    _strncpy(local_a0 + (~uVar7 - 9),&DAT_005cd850,iVar2 + 0x4f);
    iVar6 = *param_2;
    (local_a0 + (~uVar7 - 9))[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar6 + 0xa0))(&stack0xffffff58,0xffffffff);
    this->mbr_0x54 = dVar4;
    if ((this->mbr_0x64 == 0x80000000) && (this->mbr_0x50 != 0)) {
      this->mbr_0x64 = *(uint *)(this->mbr_0x50 + 0x14) & 0xfffdffff | 0x20;
    }
  }
  iVar6 = FUN_00479700(s_LISTHEIGHT_005cd854,0);
  if (iVar6 != 0) {
    FUN_00479580();
    iVar6 = FUN_0047a410(in_stack_0000000c,&DAT_005cd860,&this->mbr_0x78);
    if (iVar6 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_LISTHEIGHT_005cd864);
      return uVar3;
    }
  }
  iVar6 = FUN_00479700(s_LISTBOX_005cd870,0);
  if (iVar6 != 0) {
    FUN_00479580();
    iVar6 = FUN_0047a410(in_stack_0000000c,s__i__i__i__i_005cd878,&this->mbr_0x68,&this->mbr_0x6c,
                         &this->mbr_0x70,&this->mbr_0x74);
    if (iVar6 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_LISTBOX_005cd884);
      return uVar3;
    }
    if (param_1 == (cls_0x433170 *)0x0) {
      pdVar5 = (dword *)0x0;
    }
    else {
      pdVar5 = &this->mbr_0x7c;
    }
    iVar6 = cls_0x42f9f0::meth_0x42f9f0_DefWidget_LISTBOX_attr_parser((cls_0x42f9f0 *)&this->mbr_0x7c,pdVar5,param_2);
    if (iVar6 == 0) {
      return 0;
    }
  }
  return 1;
}



