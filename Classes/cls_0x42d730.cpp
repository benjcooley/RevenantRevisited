// Decompiled methods and structure for class: cls_0x42d730

/*
/OOAnalyzer/cls_0x42d730
pack(disabled)
Structure cls_0x42d730 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   16   dword   4   mbr_0x10   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
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
   128   dword   4   mbr_0x80   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 132 Alignment: 1

*/

// Function at 0042d730

/* WARNING: Removing unreachable block (ram,0x0042d82f) */
/* WARNING: Removing unreachable block (ram,0x0042d838) */
/* WARNING: Removing unreachable block (ram,0x0042d83a) */

undefined4 __thiscall
OOAnalyzer::cls_0x42d730::meth_0x42d730(cls_0x42d730 *this,cls_0x42d730 *param_1,int *param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  dword dVar4;
  int iVar5;
  uint uVar6;
  cls_0x42d730 *pcVar7;
  undefined4 *puVar8;
  char *pcVar9;
  undefined4 in_stack_0000000c;
  char *pcVar10;
  undefined4 uStack_a4;
  char local_a0 [70];
  char acStack_5a [9];
  undefined uStack_51;
  char acStack_50 [80];
  
  if (param_1 == (cls_0x42d730 *)0x0) {
    pcVar7 = this;
    for (iVar5 = 0x21; iVar5 != 0; iVar5 = iVar5 + -1) {
      pcVar7->mbr_0x0 = 0;
      pcVar7 = (cls_0x42d730 *)&pcVar7->mbr_0x4;
    }
    this->mbr_0x80 = 0x80000000;
    this->mbr_0x64 = 0x80000000;
    this->mbr_0x48 = 0x80000000;
    this->mbr_0x2c = 0x80000000;
  }
  else if (this != param_1) {
    pcVar7 = this;
    for (iVar5 = 0x21; iVar5 != 0; iVar5 = iVar5 + -1) {
      pcVar7->mbr_0x0 = param_1->mbr_0x0;
      param_1 = (cls_0x42d730 *)&param_1->mbr_0x4;
      pcVar7 = (cls_0x42d730 *)&pcVar7->mbr_0x4;
    }
  }
  iVar5 = FUN_00479700(s_BGBITMAP_005cd2f8,0);
  if (iVar5 == 0) goto LAB_0042d8ef;
  FUN_00479580();
  iVar5 = FUN_0047a410();
  if (iVar5 == 0) {
    iVar5 = *param_2;
    pcVar10 = s_Bitmap_name_expected_005cd9b4;
    pcVar9 = (char *)0x0;
LAB_0042d7e7:
    iVar5 = (**(code **)(iVar5 + 0x98))(pcVar10,pcVar9);
    if (iVar5 == 0) {
      return 0;
    }
  }
  else {
    dVar4 = (**(code **)(*param_2 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x0 = dVar4;
    if (dVar4 == 0) {
      iVar5 = *param_2;
      pcVar9 = local_a0;
      pcVar10 = s_Unable_to_find_bitmap___s__in_re_005cd9cc;
      goto LAB_0042d7e7;
    }
  }
  iVar5 = FUN_00479700(s_FRAME_005cd304,0);
  if (iVar5 != 0) {
    FUN_00479580();
    this->mbr_0x4 = 1;
    if (&this->mbr_0x8 == (dword *)0x0) {
      puVar8 = (undefined4 *)0x0;
      for (iVar5 = 9; iVar5 != 0; iVar5 = iVar5 + -1) {
        *puVar8 = 0;
        puVar8 = puVar8 + 1;
      }
    }
    iVar5 = FUN_0047a410(in_stack_0000000c,s__i__i__i__i_<MARGINS__i__i__i__i_005ccf24,
                         &this->field_0xc,&this->mbr_0x10,&this->mbr_0x14,&this->mbr_0x18,
                         &this->mbr_0x1c,&this->mbr_0x20,&this->mbr_0x24,&this->mbr_0x28,
                         &this->mbr_0x8);
    if ((iVar5 == 0) &&
       (iVar5 = (**(code **)(*param_2 + 0x98))(s_Error_parsing_frame_data_005ccf54,0), iVar5 == 0))
    {
      return 0;
    }
  }
  iVar5 = FUN_00479700(s_DRAWMODE_005cd30c,0);
  if (iVar5 != 0) {
    FUN_00479580();
    iVar5 = FUN_0047a410(in_stack_0000000c,&DAT_005cd318,&this->mbr_0x2c);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_FRAME_005cd31c);
      return uVar3;
    }
  }
LAB_0042d8ef:
  iVar5 = FUN_00479700(&DAT_005cd324,0);
  if (iVar5 != 0) {
    FUN_00479580();
    iVar5 = FUN_0047a410(in_stack_0000000c,s__78s_<RELR>__i_<RELB>__i_005cd328,acStack_50,
                         &this->mbr_0x40,&this->mbr_0x38,&this->mbr_0x44,&this->mbr_0x3c);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd344);
      return uVar3;
    }
    _strncpy(local_a0,acStack_50,0x4f);
    uVar6 = 0xffffffff;
    uStack_51 = 0;
    pcVar9 = local_a0;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar6 - 1);
    uStack_a4 = local_a0 + (~uVar6 - 1);
    _strncpy(uStack_a4,&DAT_005cd348,iVar2 + 0x4f);
    iVar5 = *param_2;
    uStack_a4[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar5 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x30 = dVar4;
    _strncpy(&stack0xffffff58,acStack_5a + 2,0x4f);
    uVar6 = 0xffffffff;
    acStack_5a[1] = 0;
    pcVar9 = &stack0xffffff58;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar6 - 1);
    _strncpy(local_a0 + (~uVar6 - 9),&DAT_005cd34c,iVar2 + 0x4f);
    iVar5 = *param_2;
    (local_a0 + (~uVar6 - 9))[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar5 + 0xa0))(&stack0xffffff58,0xffffffff);
    this->mbr_0x34 = dVar4;
    iVar5 = FUN_00479700(s_DRAWMODE_005cd350,0);
    if (iVar5 != 0) {
      FUN_00479580();
      iVar5 = FUN_0047a410(in_stack_0000000c,&DAT_005cd35c,&this->mbr_0x48);
      if (iVar5 == 0) {
        uVar3 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd360);
        return uVar3;
      }
    }
    if ((this->mbr_0x48 == 0x80000000) && (this->mbr_0x30 != 0)) {
      this->mbr_0x48 = *(uint *)(this->mbr_0x30 + 0x14) & 0xfffdffff | 0x20;
    }
  }
  iVar5 = FUN_00479700(&DAT_005cd364,0);
  if (iVar5 != 0) {
    FUN_00479580();
    iVar5 = FUN_0047a410(in_stack_0000000c,s__78s_<RELR>__i_<RELB>__i_005cd36c,acStack_50,
                         &this->mbr_0x5c,&this->mbr_0x54,&this->mbr_0x60,&this->mbr_0x58);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd388);
      return uVar3;
    }
    _strncpy(local_a0,acStack_50,0x4f);
    uVar6 = 0xffffffff;
    uStack_51 = 0;
    pcVar9 = local_a0;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar6 - 1);
    uStack_a4 = local_a0 + (~uVar6 - 1);
    _strncpy(uStack_a4,&DAT_005cd390,iVar2 + 0x4f);
    iVar5 = *param_2;
    uStack_a4[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar5 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x4c = dVar4;
    _strncpy(&stack0xffffff58,acStack_5a + 2,0x4f);
    uVar6 = 0xffffffff;
    acStack_5a[1] = 0;
    pcVar9 = &stack0xffffff58;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar6 - 1);
    _strncpy(local_a0 + (~uVar6 - 9),&DAT_005cd394,iVar2 + 0x4f);
    iVar5 = *param_2;
    (local_a0 + (~uVar6 - 9))[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar5 + 0xa0))(&stack0xffffff58,0xffffffff);
    this->mbr_0x50 = dVar4;
    iVar5 = FUN_00479700(s_DRAWMODE_005cd398,0);
    if (iVar5 != 0) {
      FUN_00479580();
      iVar5 = FUN_0047a410(in_stack_0000000c,&DAT_005cd3a4,&this->mbr_0x64);
      if (iVar5 == 0) {
        uVar3 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd3a8);
        return uVar3;
      }
    }
    if ((this->mbr_0x64 == 0x80000000) && (this->mbr_0x4c != 0)) {
      this->mbr_0x64 = *(uint *)(this->mbr_0x4c + 0x14) & 0xfffdffff | 0x20;
    }
  }
  iVar5 = FUN_00479700(s_THUMB_005cd3b0,0);
  if (iVar5 != 0) {
    FUN_00479580();
    iVar5 = FUN_0047a410(in_stack_0000000c,s__78s__i__i_<RELBR>__i_005cd3b8,acStack_50,
                         &this->mbr_0x70,&this->mbr_0x74,&this->mbr_0x7c,&this->mbr_0x78);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_THUMB_005cd3d0);
      return uVar3;
    }
    _strncpy(local_a0,acStack_50,0x4f);
    uVar6 = 0xffffffff;
    uStack_51 = 0;
    pcVar9 = local_a0;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar6 - 1);
    uStack_a4 = local_a0 + (~uVar6 - 1);
    _strncpy(uStack_a4,&DAT_005cd3d8,iVar2 + 0x4f);
    iVar5 = *param_2;
    uStack_a4[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar5 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x68 = dVar4;
    _strncpy(&stack0xffffff58,acStack_5a + 2,0x4f);
    uVar6 = 0xffffffff;
    acStack_5a[1] = 0;
    pcVar9 = &stack0xffffff58;
    do {
      if (uVar6 == 0) break;
      uVar6 = uVar6 - 1;
      cVar1 = *pcVar9;
      pcVar9 = pcVar9 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar6 - 1);
    _strncpy(local_a0 + (~uVar6 - 9),&DAT_005cd3dc,iVar2 + 0x4f);
    iVar5 = *param_2;
    (local_a0 + (~uVar6 - 9))[iVar2 + 0x4f] = '\0';
    dVar4 = (**(code **)(iVar5 + 0xa0))(&stack0xffffff58,0xffffffff);
    this->mbr_0x6c = dVar4;
    iVar5 = FUN_00479700(s_DRAWMODE_005cd3e0,0);
    if (iVar5 != 0) {
      FUN_00479580();
      iVar5 = FUN_0047a410(in_stack_0000000c,&DAT_005cd3ec,&this->mbr_0x80);
      if (iVar5 == 0) {
        uVar3 = (**(code **)(*param_2 + 0x9c))(s_THUMB_005cd3f0);
        return uVar3;
      }
    }
    if ((this->mbr_0x80 == 0x80000000) && (this->mbr_0x68 != 0)) {
      this->mbr_0x80 = *(uint *)(this->mbr_0x68 + 0x14) & 0xfffdffff | 0x20;
    }
  }
  return 1;
}



