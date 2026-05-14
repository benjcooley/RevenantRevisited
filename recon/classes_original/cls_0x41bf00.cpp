// Decompiled methods and structure for class: cls_0x41bf00

/*
/OOAnalyzer/cls_0x41bf00
pack(disabled)
Structure cls_0x41bf00 {
   0   dword   4   mbr_0x0   "Unsigned Double-Word (ddw, 4-bytes)"
   4   dword   4   mbr_0x4   "Unsigned Double-Word (ddw, 4-bytes)"
   8   dword   4   mbr_0x8   "Unsigned Double-Word (ddw, 4-bytes)"
   20   dword   4   mbr_0x14   "Unsigned Double-Word (ddw, 4-bytes)"
   24   dword   4   mbr_0x18   "Unsigned Double-Word (ddw, 4-bytes)"
}
Length: 28 Alignment: 1

*/

// Function at 0041bf00

undefined4 __thiscall OOAnalyzer::cls_0x41bf00::meth_0x41bf00_TAreaMgr_Initialize(cls_0x41bf00 *this)

{
  dword *this_00;
  int iVar1;
  int iVar2;
  
  if (this->mbr_0x0 == 0) {
    this_00 = &this->mbr_0x4;
    iVar2 = 0;
    if (0 < (int)this->mbr_0x4) {
      do {
        if ((-1 < iVar2) && (iVar1 = *(int *)(this->mbr_0x14 + iVar2 * 4), iVar1 != 0)) {
          FUN_0041a3b0_TArea_Dtor();
          FUN_004830f0(iVar1);
        }
        cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
        iVar2 = iVar2 + 1;
      } while (iVar2 < (int)*this_00);
    }
    DAT_005e91f0 = 0xfffe7960;
    DAT_005e91f4 = 0xfffe7960;
    DAT_005e91f8._0_4_ = 0xfffe7960;
    *this_00 = 0;
    this->mbr_0x8 = 0;
    DAT_005e91ec = 0xff;
    DAT_005e91f8._4_4_ = 0xffffffff;
    DAT_005e91e8 = 0xffffffff;
    iVar2 = meth_0x41c000_TAreaMgr_Load(this);
    if (iVar2 == 0) {
      return 0;
    }
    this->mbr_0x0 = 1;
  }
  return 1;
}



// Function at 0041bfa0

void __thiscall OOAnalyzer::cls_0x41bf00::meth_0x41bfa0_TAreaMgr_Close(cls_0x41bf00 *this)

{
  dword *this_00;
  int iVar1;
  int iVar2;
  
  this_00 = &this->mbr_0x4;
  iVar2 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      if ((-1 < iVar2) && (iVar1 = *(int *)(this->mbr_0x14 + iVar2 * 4), iVar1 != 0)) {
        FUN_0041a3b0_TArea_Dtor();
        FUN_004830f0(iVar1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)*this_00);
  }
  *this_00 = 0;
  this->mbr_0x8 = 0;
  this->mbr_0x18 = 0;
  this->mbr_0x0 = 0;
  return;
}



// Function at 0041c000

undefined4 __thiscall OOAnalyzer::cls_0x41bf00::meth_0x41c000_TAreaMgr_Load(cls_0x41bf00 *this)

{
  dword *this_00;
  char cVar1;
  void **ppvVar2;
  dword dVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  undefined4 *puVar8;
  char *pcVar9;
  char *pcVar10;
  undefined4 *puVar11;
  int local_29c;
  undefined4 local_298;
  int local_294;
  int local_290;
  undefined4 local_28c;
  int local_288;
  undefined4 local_284;
  undefined4 local_280;
  undefined4 local_278;
  undefined4 local_274;
  undefined *local_270;
  undefined local_26c;
  undefined4 local_268;
  cls_0x478720 local_260;
  int local_258;
  undefined local_254 [64];
  char local_214 [260];
  undefined4 local_110 [65];
  void *local_c;
  undefined *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059c956;
  local_c = ExceptionList;
  if (DAT_0065a784 < 0) {
    local_29c = 0;
  }
  else {
    local_29c = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
  }
  this_00 = &this->mbr_0x4;
  iVar7 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < (int)this->mbr_0x4) {
    do {
      if ((-1 < iVar7) && (*(int *)(this->mbr_0x14 + iVar7 * 4) != 0)) {
        FUN_0041c790_TArea_ScalarDtor(1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar7 = iVar7 + 1;
      ppvVar2 = (void **)ExceptionList;
    } while (iVar7 < (int)*this_00);
  }
  ExceptionList = ppvVar2;
  *this_00 = 0;
  this->mbr_0x8 = 0;
  local_258 = FUN_00482fb0(0x260);
  local_4 = 0;
  if (local_258 == 0) {
    dVar3 = 0;
  }
  else {
    dVar3 = FUN_0041a1f0_TArea_Ctor(1);
  }
  local_4 = 0xffffffff;
  this->mbr_0x18 = dVar3;
  cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)this_00);
  FUN_0058b100(local_110,&DAT_005c6dc4,0x65bd48,s_area_def_005c6db8);
  if (local_29c != 0) {
    if (DAT_0065a784 < 0) {
      iVar7 = 0;
    }
    else {
      iVar7 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    FUN_0058b100(local_214,s__s_s__s_005c6dd8,&DAT_0065d6a4,iVar7 + 0x58,s_area_def_005c6dcc);
    iVar7 = FUN_004a1c00(local_214,0);
    if (iVar7 != 0) {
      uVar5 = 0xffffffff;
      pcVar9 = local_214;
      do {
        pcVar10 = pcVar9;
        if (uVar5 == 0) break;
        uVar5 = uVar5 - 1;
        pcVar10 = pcVar9 + 1;
        cVar1 = *pcVar9;
        pcVar9 = pcVar10;
      } while (cVar1 != '\0');
      uVar5 = ~uVar5;
      puVar8 = (undefined4 *)(pcVar10 + -uVar5);
      puVar11 = local_110;
      for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
        *puVar11 = *puVar8;
        puVar8 = puVar8 + 1;
        puVar11 = puVar11 + 1;
      }
      for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
        *(undefined *)puVar11 = *(undefined *)puVar8;
        puVar8 = (undefined4 *)((int)puVar8 + 1);
        puVar11 = (undefined4 *)((int)puVar11 + 1);
      }
    }
  }
  cls_0x478720::cls_0x478720(&local_260);
  local_4 = 1;
  local_298 = 0;
  local_294 = 0;
  local_290 = 0;
  local_28c = 0;
  local_288 = 0;
  local_284 = 0;
  local_280 = 0;
  local_278 = 0;
  local_274 = 0;
  local_26c = 0;
  local_268 = 1;
  local_270 = (undefined *)FUN_00482fb0(0x2000);
  *local_270 = 0;
  local_4 = 2;
  iVar7 = FUN_004789c0(local_110);
  if (iVar7 == 0) {
    local_4 = 3;
    FUN_004830f0(local_270);
    if (local_294 == 0) {
      if (local_290 != 0) {
        FUN_004830f0(local_28c);
        FUN_004a1540(local_290);
      }
    }
    else {
      FUN_004830f0(local_28c);
      FUN_004830f0(local_294);
    }
    local_4 = 0xffffffff;
    cls_0x478720::meth_0x478730(&local_260);
    uVar4 = 0;
  }
  else {
    iVar7 = FUN_00479450();
    if (iVar7 == 0) {
      FUN_00479950(s_Syntax_error_in_header_005c6de0,0);
    }
    while (local_288 != 10) {
      iVar7 = FUN_0047a410(&local_298,s_AREA__s_005c6df8,local_254);
      if (iVar7 == 0) {
        FUN_00479950(s_AREA__name__expected_005c6e04,0);
      }
      local_258 = FUN_00482fb0(0x260);
      local_4._0_1_ = 4;
      if (local_258 != 0) {
        FUN_0041a1f0_TArea_Ctor(0);
      }
      local_4 = CONCAT31(local_4._1_3_,2);
      iVar7 = FUN_0041a4e0_TArea_Load(local_254,&local_298);
      if (iVar7 == 0) {
        FUN_00479950(s_Error_loading_area_005c6e1c,0);
      }
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)this_00);
      iVar7 = FUN_00479450();
      if (iVar7 == 0) {
        FUN_00479950(s_Syntax_error_between_area_blocks_005c6e30,0);
      }
    }
    local_4 = 5;
    FUN_004830f0(local_270);
    if (local_294 == 0) {
      if (local_290 != 0) {
        FUN_004830f0(local_28c);
        FUN_004a1540(local_290);
      }
    }
    else {
      FUN_004830f0(local_28c);
      FUN_004830f0(local_294);
    }
    local_4 = 0xffffffff;
    cls_0x478720::meth_0x478730(&local_260);
    uVar4 = 1;
  }
  ExceptionList = local_c;
  return uVar4;
}



