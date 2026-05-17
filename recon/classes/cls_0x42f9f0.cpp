// Decompiled methods and structure for class: cls_0x42f9f0

/*
/OOAnalyzer/cls_0x42f9f0
pack(disabled)
Structure cls_0x42f9f0 {
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
   72   byte   1   mbr_0x48   "Unsigned Byte (db)"
   73   byte   1   mbr_0x49   "Unsigned Byte (db)"
   74   byte   1   mbr_0x4a   "Unsigned Byte (db)"
   75   byte   1   mbr_0x4b   "Unsigned Byte (db)"
   76   dword   4   mbr_0x4c   "Unsigned Double-Word (ddw, 4-bytes)"
   80   dword   4   mbr_0x50   "Unsigned Double-Word (ddw, 4-bytes)"
   84   byte   1   mbr_0x54   "Unsigned Byte (db)"
   85   byte   1   mbr_0x55   "Unsigned Byte (db)"
   86   byte   1   mbr_0x56   "Unsigned Byte (db)"
   88   dword   4   mbr_0x58   "Unsigned Double-Word (ddw, 4-bytes)"
   92   dword   4   mbr_0x5c   "Unsigned Double-Word (ddw, 4-bytes)"
   96   dword   4   mbr_0x60   "Unsigned Double-Word (ddw, 4-bytes)"
   100   dword   4   mbr_0x64   "Unsigned Double-Word (ddw, 4-bytes)"
   104   dword   4   mbr_0x68   "Unsigned Double-Word (ddw, 4-bytes)"
   108   dword   4   mbr_0x6c   "Unsigned Double-Word (ddw, 4-bytes)"
   112   dword   4   mbr_0x70   "Unsigned Double-Word (ddw, 4-bytes)"
   116   dword   4   mbr_0x74   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 120 Alignment: 1

*/

// Function at 0042f9f0

/* WARNING: Removing unreachable block (ram,0x0042fb3f) */
/* WARNING: Removing unreachable block (ram,0x0042fb48) */
/* WARNING: Removing unreachable block (ram,0x0042fb4a) */

undefined4 __thiscall
OOAnalyzer::cls_0x42f9f0::meth_0x42f9f0_DefWidget_LISTBOX_attr_parser(cls_0x42f9f0 *this,cls_0x42f9f0 *param_1,int *param_2)

{
  undefined4 uVar1;
  dword dVar2;
  int iVar3;
  cls_0x42f9f0 *pcVar4;
  undefined4 *puVar5;
  undefined local_a0 [160];
  
  if (param_1 == (cls_0x42f9f0 *)0x0) {
    pcVar4 = this;
    for (iVar3 = 0x3e; iVar3 != 0; iVar3 = iVar3 + -1) {
      pcVar4->mbr_0x0 = 0;
      pcVar4 = (cls_0x42f9f0 *)&pcVar4->mbr_0x4;
    }
    this->mbr_0x30 = 0x14;
    this->mbr_0x34 = 0x14;
    dVar2 = DAT_0065caec;
    this->mbr_0x50 = 1;
    this->mbr_0x4c = dVar2;
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
    pcVar4 = this;
    for (iVar3 = 0x3e; iVar3 != 0; iVar3 = iVar3 + -1) {
      pcVar4->mbr_0x0 = param_1->mbr_0x0;
      param_1 = (cls_0x42f9f0 *)&param_1->mbr_0x4;
      pcVar4 = (cls_0x42f9f0 *)&pcVar4->mbr_0x4;
    }
  }
  iVar3 = FUN_00479700(s_BGBITMAP_005cd51c,0);
  if (iVar3 == 0) goto LAB_0042fc04;
  FUN_00479580();
  iVar3 = FUN_0047a410();
  if (iVar3 == 0) {
    iVar3 = (**(code **)(*param_2 + 0x98))(s_Bitmap_name_expected_005cd9b4,0);
LAB_0042faf8:
    if (iVar3 == 0) {
      return 0;
    }
  }
  else {
    dVar2 = (**(code **)(*param_2 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x0 = dVar2;
    if (dVar2 == 0) {
      iVar3 = (**(code **)(*param_2 + 0x98))(s_Unable_to_find_bitmap___s__in_re_005cd9cc,local_a0);
      goto LAB_0042faf8;
    }
  }
  iVar3 = FUN_00479700(s_FRAME_005cd528,0);
  if (iVar3 != 0) {
    FUN_00479580();
    this->mbr_0x4 = 1;
    if (this == (cls_0x42f9f0 *)0xfffffff8) {
      puVar5 = (undefined4 *)0x0;
      for (iVar3 = 9; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar5 = 0;
        puVar5 = puVar5 + 1;
      }
    }
    iVar3 = FUN_0047a410();
    if ((iVar3 == 0) &&
       (iVar3 = (**(code **)(*param_2 + 0x98))(s_Error_parsing_frame_data_005ccf54,0), iVar3 == 0))
    {
      return 0;
    }
  }
  iVar3 = FUN_00479700(s_DRAWMODE_005cd530,0);
  if (iVar3 != 0) {
    FUN_00479580();
    iVar3 = FUN_0047a410();
    if (iVar3 == 0) {
      uVar1 = (**(code **)(*param_2 + 0x9c))(s_FRAME_005cd540);
      return uVar1;
    }
  }
LAB_0042fc04:
  iVar3 = FUN_00479700(&DAT_005cd548,0);
  if (iVar3 != 0) {
    FUN_00479580();
    iVar3 = FUN_0047a410();
    if (iVar3 == 0) {
      uVar1 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd55c);
      return uVar1;
    }
  }
  iVar3 = FUN_00479700(&DAT_005cd564,0);
  if (iVar3 != 0) {
    FUN_00479580();
    iVar3 = FUN_0047a410();
    if (iVar3 == 0) {
      uVar1 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd574);
      return uVar1;
    }
  }
  iVar3 = FUN_00479700(s_SELCOLOR_005cd57c,0);
  if (iVar3 != 0) {
    FUN_00479580();
    iVar3 = FUN_0047a410();
    if (iVar3 == 0) {
      uVar1 = (**(code **)(*param_2 + 0x9c))(s_SELCOLOR_005cd594);
      return uVar1;
    }
  }
  iVar3 = FUN_00479700(&DAT_005cd5a0,0);
  if (iVar3 != 0) {
    FUN_00479580();
    iVar3 = FUN_0047a410();
    if (iVar3 == 0) {
      uVar1 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd5d8);
      return uVar1;
    }
    dVar2 = cls_0x45f7c0::meth_0x4acb30((cls_0x45f7c0 *)&UNK_0065abf8.field_0x418);
    this->mbr_0x4c = dVar2;
    if (dVar2 == 0) {
      this->mbr_0x4c = DAT_0065caec;
    }
  }
  iVar3 = FUN_00479700(s_SCROLLBAR_005cd5e0,0);
  if (iVar3 != 0) {
    FUN_00479580();
    this->mbr_0x5c = 1;
    iVar3 = FUN_0047a410();
    if (iVar3 == 0) {
      uVar1 = (**(code **)(*param_2 + 0x9c))(s_SCROLLBAR_005cd600);
      return uVar1;
    }
    iVar3 = cls_0x42d730::meth_0x42d730_DefWidget_FRAME_attr_parser((cls_0x42d730 *)&this->mbr_0x74,0,param_2);
    if (iVar3 == 0) {
      return 0;
    }
  }
  return 1;
}



