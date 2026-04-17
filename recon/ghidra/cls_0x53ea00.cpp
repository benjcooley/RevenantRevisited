// Decompiled methods and structure for class: cls_0x53ea00

/*
/OOAnalyzer/cls_0x53ea00
pack(disabled)
Structure cls_0x53ea00 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 24 Alignment: 1

*/

// Function at 0053ea00

undefined4 __thiscall OOAnalyzer::cls_0x53ea00::meth_0x53ea00(cls_0x53ea00 *this)

{
  dword *this_00;
  cls_0x45f7c0 *this_01;
  int iVar1;
  
  if (this->mbr_0x0 == 0) {
    this_00 = &this->mbr_0x4;
    iVar1 = 0;
    if (0 < (int)this->mbr_0x4) {
      do {
        if ((-1 < iVar1) &&
           (this_01 = *(cls_0x45f7c0 **)(this->mbr_0x14 + iVar1 * 4), this_01 != (cls_0x45f7c0 *)0x0
           )) {
          cls_0x45f7c0::meth_0x53db70(this_01);
          FUN_004830f0(this_01);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
        iVar1 = iVar1 + 1;
      } while (iVar1 < (int)*this_00);
    }
    *this_00 = 0;
    this->mbr_0x8 = 0;
    iVar1 = meth_0x53ead0(this);
    if (iVar1 == 0) {
      return 0;
    }
    this->mbr_0x0 = 1;
  }
  return 1;
}



// Function at 0053ead0

undefined4 __thiscall OOAnalyzer::cls_0x53ea00::meth_0x53ead0(cls_0x53ea00 *this)

{
  int iVar1;
  cls_0x45f7c0 *this_00;
  uint local_194;
  LPCVOID local_190;
  FILE **local_18c;
  LPCVOID local_188;
  int local_184;
  undefined4 local_180;
  undefined4 local_17c;
  undefined4 local_174;
  undefined4 local_170;
  undefined4 *local_16c;
  undefined local_168;
  undefined4 local_164;
  cls_0x478720 local_15c;
  cls_0x45f7c0 *local_154;
  undefined local_150 [64];
  cls_0x5a36f8__vftable_5a36f8 local_110 [13];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1dbe;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0058b100((char *)local_110,&DAT_005e4e1c);
  cls_0x478720::cls_0x478720(&local_15c);
  local_4 = 0;
  local_194 = 0;
  local_190 = (LPCVOID)0x0;
  local_18c = (FILE **)0x0;
  local_188 = (LPCVOID)0x0;
  local_184 = 0;
  local_180 = 0;
  local_17c = 0;
  local_174 = 0;
  local_170 = 0;
  local_168 = 0;
  local_164 = 1;
  local_16c = FUN_00482fb0(0x2000);
  *(undefined *)local_16c = 0;
  local_4 = 1;
  iVar1 = FUN_004789c0((int)&local_194,local_110);
  if (iVar1 == 0) {
    local_4 = 2;
    FUN_004830f0(local_16c);
    if (local_190 == (LPCVOID)0x0) {
      if (local_18c != (FILE **)0x0) {
        FUN_004830f0(local_188);
        FUN_004a1540(local_18c);
      }
    }
    else {
      FUN_004830f0(local_188);
      FUN_004830f0(local_190);
    }
    local_4 = 0xffffffff;
    cls_0x478720::meth_0x478730(&local_15c);
    ExceptionList = local_c;
    return 0;
  }
  iVar1 = FUN_00479450(&local_194);
  if (iVar1 == 0) {
    FUN_00479950(&local_194,(byte *)s_Syntax_error_in_header_005e4e24);
  }
  if (local_184 != 10) {
    do {
      iVar1 = FUN_0047a410(&local_194,(float **)s_SPELL__s_005e4e3c);
      if (iVar1 == 0) {
        FUN_00479950(&local_194,(byte *)s_SPELL__name__expected_005e4e48);
      }
      local_154 = (cls_0x45f7c0 *)FUN_00482fb0(0xcc);
      local_4._0_1_ = 3;
      if (local_154 == (cls_0x45f7c0 *)0x0) {
        this_00 = (cls_0x45f7c0 *)0x0;
      }
      else {
        this_00 = cls_0x45f7c0::cls_0x45f7c0(local_154);
      }
      local_4 = CONCAT31(local_4._1_3_,1);
      iVar1 = cls_0x45f7c0::meth_0x53e4e0(this_00,local_150);
      if (iVar1 == 0) {
        FUN_00479950(&local_194,(byte *)s_Error_loading_spell_data_005e4e60);
      }
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)&this->mbr_0x4);
      iVar1 = FUN_00479450(&local_194);
      if (iVar1 == 0) {
        FUN_00479950(&local_194,(byte *)s_Syntax_error_between_spell_block_005e4e7c);
      }
    } while (local_184 != 10);
  }
  local_4 = 4;
  FUN_004830f0(local_16c);
  if (local_190 == (LPCVOID)0x0) {
    if (local_18c != (FILE **)0x0) {
      FUN_004830f0(local_188);
      FUN_004a1540(local_18c);
    }
  }
  else {
    FUN_004830f0(local_188);
    FUN_004830f0(local_190);
  }
  local_4 = 0xffffffff;
  cls_0x478720::meth_0x478730(&local_15c);
  ExceptionList = local_c;
  return 1;
}



