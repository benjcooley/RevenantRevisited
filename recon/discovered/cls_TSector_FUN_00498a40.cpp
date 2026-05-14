// FUN_00498a40_TSector_Save @ 00498a40 size=368

bool __fastcall FUN_00498a40_TSector_Save(int param_1)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  char *pcVar7;
  char *pcVar8;
  char *pcVar9;
  bool bVar10;
  undefined1 local_124 [4];
  int local_120;
  int local_11c;
  char local_110 [260];
  void *local_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_0059db60;
  local_c = ExceptionList;
  ExceptionList = &local_c;
  FUN_0049cc20(0x8000,0x3f9c);
  local_4 = 0;
  iVar2 = FUN_00498c90_TSector_SaveToStream(local_124);
  if (iVar2 == 0) {
    local_4 = 0xffffffff;
    FUN_0049cc50();
    bVar10 = false;
  }
  else {
    uVar5 = 0xffffffff;
    pcVar7 = &DAT_0065c02c;
    do {
      pcVar9 = pcVar7;
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pcVar9 = pcVar7 + 1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar9;
    } while (cVar1 != '\0');
    uVar5 = ~uVar5;
    pcVar7 = pcVar9 + -uVar5;
    pcVar9 = local_110;
    for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
      *(undefined4 *)pcVar9 = *(undefined4 *)pcVar7;
      pcVar7 = pcVar7 + 4;
      pcVar9 = pcVar9 + 4;
    }
    for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
      *pcVar9 = *pcVar7;
      pcVar7 = pcVar7 + 1;
      pcVar9 = pcVar9 + 1;
    }
    iVar2 = -1;
    pcVar7 = local_110;
    do {
      pcVar9 = pcVar7;
      if (iVar2 == 0) break;
      iVar2 = iVar2 + -1;
      pcVar9 = pcVar7 + 1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar9;
    } while (cVar1 != '\0');
    *(undefined4 *)(pcVar9 + -1) = DAT_005da784;
    uVar5 = 0xffffffff;
    *(undefined **)(pcVar9 + 3) = PTR_DAT_005da788;
    pcVar7 = (char *)(param_1 + 0x14);
    do {
      pcVar9 = pcVar7;
      if (uVar5 == 0) break;
      uVar5 = uVar5 - 1;
      pcVar9 = pcVar7 + 1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar9;
    } while (cVar1 != '\0');
    uVar5 = ~uVar5;
    iVar2 = -1;
    pcVar7 = local_110;
    do {
      pcVar8 = pcVar7;
      if (iVar2 == 0) break;
      iVar2 = iVar2 + -1;
      pcVar8 = pcVar7 + 1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar8;
    } while (cVar1 != '\0');
    pcVar7 = pcVar9 + -uVar5;
    pcVar9 = pcVar8 + -1;
    for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
      *(undefined4 *)pcVar9 = *(undefined4 *)pcVar7;
      pcVar7 = pcVar7 + 4;
      pcVar9 = pcVar9 + 4;
    }
    for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
      *pcVar9 = *pcVar7;
      pcVar7 = pcVar7 + 1;
      pcVar9 = pcVar9 + 1;
    }
    iVar2 = FUN_004831c0(local_110);
    if (iVar2 == 0) {
      local_4 = 0xffffffff;
      FUN_0049cc50();
      bVar10 = false;
    }
    else {
      FUN_0058c3a5(iVar2,0,0);
      uVar3 = FUN_0049cdd0(local_11c - local_120,1,iVar2);
      iVar4 = FUN_0058beb8(uVar3);
      bVar10 = iVar4 != local_11c - local_120;
      FUN_0058b4f1(iVar2);
      local_4 = 0xffffffff;
      FUN_0049cc50();
    }
  }
  ExceptionList = local_c;
  return bVar10;
}


