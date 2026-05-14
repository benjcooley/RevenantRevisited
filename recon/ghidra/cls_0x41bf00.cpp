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
  LPCVOID pvVar1;
  int iVar2;
  
  if (this->mbr_0x0 == 0) {
    this_00 = &this->mbr_0x4;
    iVar2 = 0;
    if (0 < (int)this->mbr_0x4) {
      do {
        if ((-1 < iVar2) &&
           (pvVar1 = *(LPCVOID *)(this->mbr_0x14 + iVar2 * 4), pvVar1 != (LPCVOID)0x0)) {
          FUN_0041a3b0_TArea_Dtor((int)pvVar1);
          FUN_004830f0(pvVar1);
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
  LPCVOID pvVar1;
  int iVar2;
  
  this_00 = &this->mbr_0x4;
  iVar2 = 0;
  if (0 < (int)this->mbr_0x4) {
    do {
      if ((-1 < iVar2) &&
         (pvVar1 = *(LPCVOID *)(this->mbr_0x14 + iVar2 * 4), pvVar1 != (LPCVOID)0x0)) {
        FUN_0041a3b0_TArea_Dtor((int)pvVar1);
        FUN_004830f0(pvVar1);
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
  LPCVOID pvVar2;
  void **ppvVar3;
  undefined *puVar4;
  undefined4 uVar5;
  char *pcVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  virt_meth_0x411380 **ppvVar10;
  char *pcVar11;
  cls_0x5a36f8__vftable_5a36f8 *pcVar12;
  int local_29c;
  uint local_298;
  LPCVOID local_294;
  FILE **local_290;
  LPCVOID local_28c;
  int local_288;
  undefined4 local_284;
  undefined4 local_280;
  undefined4 local_278;
  undefined4 local_274;
  undefined4 *local_270;
  undefined local_26c;
  undefined4 local_268;
  cls_0x478720 local_260;
  undefined4 *local_258;
  char local_254 [64];
  char local_214 [260];
  cls_0x5a36f8__vftable_5a36f8 local_110 [13];
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
  iVar9 = 0;
  ExceptionList = &local_c;
  ppvVar3 = &local_c;
  if (0 < (int)this->mbr_0x4) {
    do {
      if ((-1 < iVar9) &&
         (pvVar2 = *(LPCVOID *)(this->mbr_0x14 + iVar9 * 4), pvVar2 != (LPCVOID)0x0)) {
        FUN_0041c790_TArea_ScalarDtor(pvVar2,1);
      }
      cls_0x45f7c0::meth_0x41cb40((cls_0x45f7c0 *)this_00);
      iVar9 = iVar9 + 1;
      ppvVar3 = (void **)ExceptionList;
    } while (iVar9 < (int)*this_00);
  }
  ExceptionList = ppvVar3;
  *this_00 = 0;
  this->mbr_0x8 = 0;
  local_258 = FUN_00482fb0(0x260);
  local_4 = 0;
  if (local_258 == (undefined4 *)0x0) {
    puVar4 = (undefined *)0x0;
  }
  else {
    puVar4 = FUN_0041a1f0_TArea_Ctor((undefined *)local_258,1);
  }
  local_4 = 0xffffffff;
  this->mbr_0x18 = (dword)puVar4;
  cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)this_00);
  FUN_0058b100((char *)local_110,&DAT_005c6dc4);
  if (local_29c != 0) {
    FUN_0058b100(local_214,(byte *)s__s_s__s_005c6dd8);
    iVar9 = FUN_004a1c00(local_214,(char *)0x0);
    if (iVar9 != 0) {
      uVar7 = 0xffffffff;
      pcVar6 = local_214;
      do {
        pcVar11 = pcVar6;
        if (uVar7 == 0) break;
        uVar7 = uVar7 - 1;
        pcVar11 = pcVar6 + 1;
        cVar1 = *pcVar6;
        pcVar6 = pcVar11;
      } while (cVar1 != '\0');
      uVar7 = ~uVar7;
      ppvVar10 = (virt_meth_0x411380 **)(pcVar11 + -uVar7);
      pcVar12 = local_110;
      for (uVar8 = uVar7 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
        pcVar12->virt_meth_0x411380_0 = *ppvVar10;
        ppvVar10 = ppvVar10 + 1;
        pcVar12 = (cls_0x5a36f8__vftable_5a36f8 *)&pcVar12->virt_meth_0x411390_4;
      }
      for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
        *(undefined *)&pcVar12->virt_meth_0x411380_0 = *(undefined *)ppvVar10;
        ppvVar10 = (virt_meth_0x411380 **)((int)ppvVar10 + 1);
        pcVar12 = (cls_0x5a36f8__vftable_5a36f8 *)((int)&pcVar12->virt_meth_0x411380_0 + 1);
      }
    }
  }
  cls_0x478720::cls_0x478720(&local_260);
  local_4 = 1;
  local_298 = 0;
  local_294 = (LPCVOID)0x0;
  local_290 = (FILE **)0x0;
  local_28c = (LPCVOID)0x0;
  local_288 = 0;
  local_284 = 0;
  local_280 = 0;
  local_278 = 0;
  local_274 = 0;
  local_26c = 0;
  local_268 = 1;
  local_270 = FUN_00482fb0(0x2000);
  *(undefined *)local_270 = 0;
  local_4 = 2;
  iVar9 = FUN_004789c0((int)&local_298,local_110);
  if (iVar9 == 0) {
    local_4 = 3;
    FUN_004830f0(local_270);
    if (local_294 == (LPCVOID)0x0) {
      if (local_290 != (FILE **)0x0) {
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
    uVar5 = 0;
  }
  else {
    iVar9 = FUN_00479450(&local_298);
    if (iVar9 == 0) {
      FUN_00479950(&local_298,(byte *)s_Syntax_error_in_header_005c6de0);
    }
    while (local_288 != 10) {
      iVar9 = FUN_0047a410(&local_298,(float **)s_AREA__s_005c6df8);
      if (iVar9 == 0) {
        FUN_00479950(&local_298,(byte *)s_AREA__name__expected_005c6e04);
      }
      local_258 = FUN_00482fb0(0x260);
      local_4._0_1_ = 4;
      if (local_258 == (undefined4 *)0x0) {
        pcVar6 = (char *)0x0;
      }
      else {
        pcVar6 = FUN_0041a1f0_TArea_Ctor((undefined *)local_258,0);
      }
      local_4 = CONCAT31(local_4._1_3_,2);
      iVar9 = FUN_0041a4e0_TArea_Load(pcVar6,local_254,&local_298);
      if (iVar9 == 0) {
        FUN_00479950(&local_298,(byte *)s_Error_loading_area_005c6e1c);
      }
      cls_0x45f7c0::meth_0x41c840((cls_0x45f7c0 *)this_00);
      iVar9 = FUN_00479450(&local_298);
      if (iVar9 == 0) {
        FUN_00479950(&local_298,(byte *)s_Syntax_error_between_area_blocks_005c6e30);
      }
    }
    local_4 = 5;
    FUN_004830f0(local_270);
    if (local_294 == (LPCVOID)0x0) {
      if (local_290 != (FILE **)0x0) {
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
    uVar5 = 1;
  }
  ExceptionList = local_c;
  return uVar5;
}



