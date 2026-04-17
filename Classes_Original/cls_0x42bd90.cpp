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
  dword dVar1;
  undefined4 uVar2;
  dword *pdVar3;
  int iVar4;
  cls_0x42bd90 *pcVar5;
  cls_0x42bd90 *pcVar6;
  dword *pdVar7;
  char *pcVar8;
  undefined *puVar9;
  undefined local_50 [80];
  
  if (param_1 == (cls_0x42bd90 *)0x0) {
    pcVar5 = this;
    for (iVar4 = 0x2b; iVar4 != 0; iVar4 = iVar4 + -1) {
      pcVar5->mbr_0x0 = 0;
      pcVar5 = (cls_0x42bd90 *)&pcVar5->mbr_0x4;
    }
    this->mbr_0x38 = 0x80000000;
  }
  else if (this != param_1) {
    pcVar5 = param_1;
    pcVar6 = this;
    for (iVar4 = 0x2b; iVar4 != 0; iVar4 = iVar4 + -1) {
      pcVar6->mbr_0x0 = pcVar5->mbr_0x0;
      pcVar5 = (cls_0x42bd90 *)&pcVar5->mbr_0x4;
      pcVar6 = (cls_0x42bd90 *)&pcVar6->mbr_0x4;
    }
  }
  iVar4 = FUN_00479700(&DAT_005cd1fc,0);
  if (iVar4 != 0) {
    FUN_00479580();
    this->mbr_0x10 = 0;
    iVar4 = FUN_0047a410();
    if (iVar4 == 0) {
      iVar4 = (**(code **)(*param_2 + 0x98))(s_Bitmap_name_expected_005cd9b4,0);
    }
    else {
      dVar1 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
      this->mbr_0x0 = dVar1;
      if (dVar1 != 0) goto LAB_0042be58;
      iVar4 = (**(code **)(*param_2 + 0x98))(s_Unable_to_find_bitmap___s__in_re_005cd9cc,local_50);
    }
    if (iVar4 == 0) {
      return 0;
    }
  }
LAB_0042be58:
  iVar4 = FUN_00479700(&DAT_005cd200,0);
  if (iVar4 != 0) {
    FUN_00479580();
    this->mbr_0x10 = 0;
    iVar4 = FUN_0047a410();
    if (iVar4 == 0) {
      iVar4 = *param_2;
      puVar9 = (undefined *)0x0;
      pcVar8 = s_Bitmap_name_expected_005cd9b4;
    }
    else {
      dVar1 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
      this->mbr_0x4 = dVar1;
      if (dVar1 != 0) goto LAB_0042bed2;
      iVar4 = *param_2;
      puVar9 = local_50;
      pcVar8 = s_Unable_to_find_bitmap___s__in_re_005cd9cc;
    }
    iVar4 = (**(code **)(iVar4 + 0x98))(pcVar8,puVar9);
    if (iVar4 == 0) {
      return 0;
    }
  }
LAB_0042bed2:
  iVar4 = FUN_00479700(&PTR_LAB_005cd208,0);
  if (iVar4 != 0) {
    FUN_00479580();
    this->mbr_0x10 = 0;
    iVar4 = FUN_0047a410();
    if (iVar4 == 0) {
      iVar4 = *param_2;
      puVar9 = (undefined *)0x0;
      pcVar8 = s_Bitmap_name_expected_005cd9b4;
    }
    else {
      dVar1 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
      this->mbr_0x8 = dVar1;
      if (dVar1 != 0) goto LAB_0042bf4a;
      iVar4 = *param_2;
      puVar9 = local_50;
      pcVar8 = s_Unable_to_find_bitmap___s__in_re_005cd9cc;
    }
    iVar4 = (**(code **)(iVar4 + 0x98))(pcVar8,puVar9);
    if (iVar4 == 0) {
      return 0;
    }
  }
LAB_0042bf4a:
  iVar4 = FUN_00479700(&DAT_005cd20c,0);
  if (iVar4 != 0) {
    FUN_00479580();
    this->mbr_0x10 = 0;
    iVar4 = FUN_0047a410();
    if (iVar4 == 0) {
      iVar4 = *param_2;
      puVar9 = (undefined *)0x0;
      pcVar8 = s_Bitmap_name_expected_005cd9b4;
    }
    else {
      dVar1 = (**(code **)(*param_2 + 0xa0))(local_50,0xffffffff);
      this->mbr_0xc = dVar1;
      if (dVar1 != 0) goto LAB_0042bfc2;
      iVar4 = *param_2;
      puVar9 = local_50;
      pcVar8 = s_Unable_to_find_bitmap___s__in_re_005cd9cc;
    }
    iVar4 = (**(code **)(iVar4 + 0x98))(pcVar8,puVar9);
    if (iVar4 == 0) {
      return 0;
    }
  }
LAB_0042bfc2:
  iVar4 = FUN_00479700(s_FRAME_005cd214,0);
  if (iVar4 != 0) {
    FUN_00479580();
    this->mbr_0x10 = 1;
    if (param_1 == (cls_0x42bd90 *)0x0) {
      pdVar3 = (dword *)0x0;
    }
    else {
      pdVar3 = &this->mbr_0x14;
    }
    pdVar7 = &this->mbr_0x14;
    if (pdVar3 == (dword *)0x0) {
      for (iVar4 = 9; iVar4 != 0; iVar4 = iVar4 + -1) {
        *pdVar7 = 0;
        pdVar7 = pdVar7 + 1;
      }
    }
    else if (pdVar7 != pdVar3) {
      for (iVar4 = 9; iVar4 != 0; iVar4 = iVar4 + -1) {
        *pdVar7 = *pdVar3;
        pdVar3 = pdVar3 + 1;
        pdVar7 = pdVar7 + 1;
      }
    }
    iVar4 = FUN_0047a410();
    if ((iVar4 == 0) &&
       (iVar4 = (**(code **)(*param_2 + 0x98))(s_Error_parsing_frame_data_005ccf54,0), iVar4 == 0))
    {
      return 0;
    }
  }
  iVar4 = FUN_00479700(s_DRAWMODE_005cd21c,0);
  if (iVar4 != 0) {
    FUN_00479580();
    iVar4 = FUN_0047a410();
    if (iVar4 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(s_DRAWMODE_005cd22c);
      return uVar2;
    }
  }
  iVar4 = FUN_00479700(s_UPLABEL_005cd238,0);
  if (iVar4 != 0) {
    FUN_00479580();
    this->mbr_0x3c = 1;
    if (param_1 == (cls_0x42bd90 *)0x0) {
      pdVar3 = (dword *)0x0;
    }
    else {
      pdVar3 = &this->mbr_0x4c;
    }
    iVar4 = cls_0x429fd0::meth_0x429fd0((cls_0x429fd0 *)&this->mbr_0x4c,pdVar3,param_2);
    if (iVar4 == 0) {
      return 0;
    }
  }
  iVar4 = FUN_00479700(s_DOWNLABEL_005cd240,0);
  if (iVar4 != 0) {
    FUN_00479580();
    this->mbr_0x40 = 1;
    pdVar3 = &this->mbr_0x5c;
    if (param_1 == (cls_0x42bd90 *)0x0) {
      pdVar3 = &this->mbr_0x4c;
    }
    iVar4 = cls_0x429fd0::meth_0x429fd0((cls_0x429fd0 *)&this->mbr_0x5c,pdVar3,param_2);
    if (iVar4 == 0) {
      return 0;
    }
  }
  iVar4 = FUN_00479700(s_SELLABEL_005cd24c,0);
  if (iVar4 != 0) {
    FUN_00479580();
    this->mbr_0x44 = 1;
    pdVar3 = &this->mbr_0x6c;
    if (param_1 == (cls_0x42bd90 *)0x0) {
      pdVar3 = &this->mbr_0x4c;
    }
    iVar4 = cls_0x429fd0::meth_0x429fd0((cls_0x429fd0 *)&this->mbr_0x6c,pdVar3,param_2);
    if (iVar4 == 0) {
      return 0;
    }
  }
  iVar4 = FUN_00479700(s_GRAYLABEL_005cd258,0);
  if (iVar4 != 0) {
    FUN_00479580();
    this->mbr_0x48 = 1;
    pdVar3 = &this->mbr_0x7c;
    if (param_1 == (cls_0x42bd90 *)0x0) {
      pdVar3 = &this->mbr_0x4c;
    }
    iVar4 = cls_0x429fd0::meth_0x429fd0((cls_0x429fd0 *)&this->mbr_0x7c,pdVar3,param_2);
    if (iVar4 == 0) {
      return 0;
    }
  }
  iVar4 = FUN_00479700(s_UPLABELRECT_005cd264,0);
  if (iVar4 != 0) {
    FUN_00479580();
    iVar4 = FUN_0047a410();
    if (iVar4 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(s_UPLABELRECT_005cd27c);
      return uVar2;
    }
    this->mbr_0x9c = this->mbr_0x8c;
    this->mbr_0xa0 = this->mbr_0x90;
    this->mbr_0xa4 = this->mbr_0x94;
    this->mbr_0xa8 = this->mbr_0x98;
  }
  iVar4 = FUN_00479700(s_DOWNLABELRECT_005cd288,0);
  if (iVar4 != 0) {
    FUN_00479580();
    iVar4 = FUN_0047a410();
    if (iVar4 == 0) {
      uVar2 = (**(code **)(*param_2 + 0x9c))(s_DOWNLABELRECT_005cd2a4);
      return uVar2;
    }
  }
  return 1;
}



