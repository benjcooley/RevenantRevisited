// Decompiled methods and structure for class: cls_0x42b340

/*
/OOAnalyzer/cls_0x42b340
pack(disabled)
Structure cls_0x42b340 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   12   dword   4   mbr_0xc   "Unsigned Double-Word (ddw, 4-bytes)"
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
   68   byte   1   mbr_0x44   "Unsigned Byte (db)"
   69   byte   1   mbr_0x45   "Unsigned Byte (db)"
   70   byte   1   mbr_0x46   "Unsigned Byte (db)"
   72   dword   4   mbr_0x48   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 76 Alignment: 1

*/

// Function at 0042b340

undefined4 __thiscall
OOAnalyzer::cls_0x42b340::meth_0x42b340(cls_0x42b340 *this,cls_0x42b340 *param_1,int *param_2)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  dword *pdVar2;
  undefined3 extraout_var_01;
  undefined4 uVar3;
  undefined3 extraout_var_02;
  dword dVar4;
  int iVar5;
  cls_0x42b340 *pcVar6;
  cls_0x42b340 *pcVar7;
  dword *pdVar8;
  uint *in_stack_0000000c;
  char *pcVar9;
  undefined *puVar10;
  undefined local_a0 [160];
  
  if (param_1 == (cls_0x42b340 *)0x0) {
    pcVar6 = this;
    for (iVar5 = 0x13; iVar5 != 0; iVar5 = iVar5 + -1) {
      pcVar6->mbr_0x0 = 0;
      pcVar6 = (cls_0x42b340 *)&pcVar6->mbr_0x4;
    }
    this->mbr_0x2c = DAT_0065caec;
    this->mbr_0x28 = 0x80000000;
    this->mbr_0x44 = 0xff;
    this->mbr_0x45 = 0xff;
    this->mbr_0x46 = 0xff;
    this->mbr_0x40 = 0x401;
    this->mbr_0x48 = 0x80000000;
  }
  else if (this != param_1) {
    pcVar6 = param_1;
    pcVar7 = this;
    for (iVar5 = 0x13; iVar5 != 0; iVar5 = iVar5 + -1) {
      pcVar7->mbr_0x0 = pcVar6->mbr_0x0;
      pcVar6 = (cls_0x42b340 *)&pcVar6->mbr_0x4;
      pcVar7 = (cls_0x42b340 *)&pcVar7->mbr_0x4;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_BGBITMAP_005cd174,0);
  if (CONCAT31(extraout_var,bVar1) == 0) goto LAB_0042b51b;
  FUN_00479580((byte *)in_stack_0000000c);
  iVar5 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd9ac);
  if (iVar5 == 0) {
    iVar5 = *param_2;
    pcVar9 = s_Bitmap_name_expected_005cd9b4;
    puVar10 = (undefined *)0x0;
LAB_0042b408:
    iVar5 = (**(code **)(iVar5 + 0x98))(pcVar9,puVar10);
    if (iVar5 == 0) {
      return 0;
    }
  }
  else {
    dVar4 = (**(code **)(*param_2 + 0xa0))(local_a0,0xffffffff);
    this->mbr_0x0 = dVar4;
    if (dVar4 == 0) {
      iVar5 = *param_2;
      puVar10 = local_a0;
      pcVar9 = s_Unable_to_find_bitmap___s__in_re_005cd9cc;
      goto LAB_0042b408;
    }
  }
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_FRAME_005cd180,0);
  if (CONCAT31(extraout_var_00,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    if (param_1 == (cls_0x42b340 *)0x0) {
      pdVar2 = (dword *)0x0;
    }
    else {
      pdVar2 = &this->mbr_0x4;
    }
    pdVar8 = &this->mbr_0x4;
    if (pdVar2 == (dword *)0x0) {
      for (iVar5 = 9; iVar5 != 0; iVar5 = iVar5 + -1) {
        *pdVar8 = 0;
        pdVar8 = pdVar8 + 1;
      }
    }
    else if (pdVar8 != pdVar2) {
      for (iVar5 = 9; iVar5 != 0; iVar5 = iVar5 + -1) {
        *pdVar8 = *pdVar2;
        pdVar2 = pdVar2 + 1;
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
  bVar1 = FUN_00479700((uint)in_stack_0000000c,(byte *)s_DRAWMODE_005cd188,0);
  if (CONCAT31(extraout_var_01,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar5 = FUN_0047a410(in_stack_0000000c,(float **)&DAT_005cd194);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(s_DRAWMODE_005cd198);
      return uVar3;
    }
  }
LAB_0042b51b:
  bVar1 = FUN_00479700((uint)in_stack_0000000c,&DAT_005cd1a4,0);
  if (CONCAT31(extraout_var_02,bVar1) != 0) {
    FUN_00479580((byte *)in_stack_0000000c);
    iVar5 = FUN_0047a410(in_stack_0000000c,(float **)s__30s_<RECT__i__i__i__i>_<COLOR___005cd1ac);
    if (iVar5 == 0) {
      uVar3 = (**(code **)(*param_2 + 0x9c))(&DAT_005cd1f0);
      return uVar3;
    }
    dVar4 = cls_0x45f7c0::meth_0x4acb30((cls_0x45f7c0 *)&UNK_0065abf8.field_0x418);
    this->mbr_0x2c = dVar4;
    if (dVar4 == 0) {
      this->mbr_0x2c = DAT_0065caec;
    }
  }
  return 1;
}



