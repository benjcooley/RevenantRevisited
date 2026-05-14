// FUN_0041c000_TAreaMgr_Load @ 0041c000 size=925

undefined4 __fastcall FUN_0041c000_TAreaMgr_Load(int param_1)

{
  char cVar1;
  void **ppvVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  char *pcVar7;
  char *pcVar8;
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
  undefined1 *local_270;
  undefined1 local_26c;
  undefined4 local_268;
  int local_258;
  undefined1 local_254 [64];
  char local_214 [260];
  char local_110 [260];
  void *local_c;
  undefined1 *puStack_8;
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
  iVar6 = 0;
  ExceptionList = &local_c;
  ppvVar2 = &local_c;
  if (0 < *(int *)(param_1 + 4)) {
    do {
      if ((-1 < iVar6) && (*(int *)(*(int *)(param_1 + 0x14) + iVar6 * 4) != 0)) {
        FUN_0041c790_TArea_ScalarDtor(1);
      }
      FUN_0041cb40(iVar6);
      iVar6 = iVar6 + 1;
      ppvVar2 = ExceptionList;
    } while (iVar6 < *(int *)(param_1 + 4));
  }
  ExceptionList = ppvVar2;
  *(int *)(param_1 + 4) = 0;
  *(undefined4 *)(param_1 + 8) = 0;
  local_258 = FUN_00482fb0(0x260);
  local_4 = 0;
  if (local_258 == 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = FUN_0041a1f0_TArea_Ctor(1);
  }
  local_4 = 0xffffffff;
  *(undefined4 *)(param_1 + 0x18) = uVar3;
  FUN_0041c840(uVar3);
  FUN_0058b100(local_110,&DAT_005c6dc4,&DAT_0065bd48,s_area_def_005c6db8);
  if (local_29c != 0) {
    if (DAT_0065a784 < 0) {
      iVar6 = 0;
    }
    else {
      iVar6 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    FUN_0058b100(local_214,s__s_s__s_005c6dd8,&DAT_0065d6a4,iVar6 + 0x58,s_area_def_005c6dcc);
    iVar6 = FUN_004a1c00(local_214,0);
    if (iVar6 != 0) {
      uVar4 = 0xffffffff;
      pcVar7 = local_214;
      do {
        pcVar8 = pcVar7;
        if (uVar4 == 0) break;
        uVar4 = uVar4 - 1;
        pcVar8 = pcVar7 + 1;
        cVar1 = *pcVar7;
        pcVar7 = pcVar8;
      } while (cVar1 != '\0');
      uVar4 = ~uVar4;
      pcVar7 = pcVar8 + -uVar4;
      pcVar8 = local_110;
      for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
        *(undefined4 *)pcVar8 = *(undefined4 *)pcVar7;
        pcVar7 = pcVar7 + 4;
        pcVar8 = pcVar8 + 4;
      }
      for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
        *pcVar8 = *pcVar7;
        pcVar7 = pcVar7 + 1;
        pcVar8 = pcVar8 + 1;
      }
    }
  }
  FUN_00478720();
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
  local_270 = (undefined1 *)FUN_00482fb0(0x2000);
  *local_270 = 0;
  local_4 = 2;
  iVar6 = FUN_004789c0(local_110);
  if (iVar6 == 0) {
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
    FUN_00478730();
    uVar3 = 0;
  }
  else {
    iVar6 = FUN_00479450();
    if (iVar6 == 0) {
      FUN_00479950(s_Syntax_error_in_header_005c6de0,0);
    }
    while (local_288 != 10) {
      iVar6 = FUN_0047a410(&local_298,s_AREA__s_005c6df8,local_254);
      if (iVar6 == 0) {
        FUN_00479950(s_AREA__name__expected_005c6e04,0);
      }
      local_258 = FUN_00482fb0(0x260);
      local_4._0_1_ = 4;
      if (local_258 == 0) {
        uVar3 = 0;
      }
      else {
        uVar3 = FUN_0041a1f0_TArea_Ctor(0);
      }
      local_4 = CONCAT31(local_4._1_3_,2);
      iVar6 = FUN_0041a4e0_TArea_Load(local_254,&local_298);
      if (iVar6 == 0) {
        FUN_00479950(s_Error_loading_area_005c6e1c,0);
      }
      FUN_0041c840(uVar3);
      iVar6 = FUN_00479450();
      if (iVar6 == 0) {
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
    FUN_00478730();
    uVar3 = 1;
  }
  ExceptionList = local_c;
  return uVar3;
}


