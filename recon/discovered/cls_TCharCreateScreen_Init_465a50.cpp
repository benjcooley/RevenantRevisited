// FUN_00465a50_TCharCreateScreen_Initialize @ 00465a50 size=644

undefined4 __fastcall FUN_00465a50_TCharCreateScreen_Initialize(int *param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  char *pcVar6;
  uint uVar7;
  char *pcVar8;
  int *piVar9;
  char *pcVar10;
  bool bVar11;
  
  iVar3 = FUN_004a1ec0(100,100,*(uint *)(PTR_DAT_005d79e0 + 0x38) & 0x3001f,0);
  uVar7 = DAT_0065a258_TObjectClass_numclasses;
  param_1[0x9f] = iVar3;
  iVar3 = 0;
  uVar4 = 0;
  if (0 < (int)uVar7) {
    piVar9 = &DAT_0065a148_TObjectClass_classes;
    bVar11 = uVar7 != 0;
    do {
      if (((bVar11) && (*piVar9 != 0)) && (iVar2 = *(int *)(*piVar9 + 0x24), iVar3 < iVar2)) {
        iVar3 = iVar2;
      }
      uVar4 = uVar4 + 1;
      piVar9 = piVar9 + 1;
      bVar11 = uVar4 < uVar7;
    } while ((int)uVar4 < (int)uVar7);
  }
  iVar3 = FUN_00482fb0(iVar3 * 4);
  param_1[0xa2] = iVar3;
  iVar3 = FUN_0049d800(s_MPSTATINFO1_005d3298);
  param_1[0x6e] = iVar3;
  iVar3 = FUN_0049d800(s_MPSTATINFO2_005d32a4);
  param_1[0x6f] = iVar3;
  iVar3 = FUN_0049d800(s_MPSTATINFO3_005d32b0);
  param_1[0x70] = iVar3;
  iVar3 = FUN_0049d800(s_PLYRVALUES_005d32bc);
  param_1[0x75] = iVar3;
  iVar3 = FUN_0049d800(s_PLYRSTATS_005d32c8);
  param_1[0x76] = iVar3;
  iVar3 = FUN_0049d800(s_PLYRSKILLS_005d32d4);
  param_1[0x77] = iVar3;
  if ((param_1[100] == 1) || (param_1[100] == 2)) {
    uVar5 = 0x11;
  }
  else {
    uVar5 = 0;
  }
  if (DAT_0065c890 == 0) {
    iVar3 = FUN_00435150(s_createchar_005d3300,&DAT_00659c80,uVar5,0,0,0x280,0x1e0,0x1c2,0xa0,
                         s_widgets_005d32f8,s_createchar_005d32ec);
  }
  else {
    iVar3 = FUN_00435230(s_createchar_005d32e0,&DAT_00659c7c,uVar5,0,0,0x280,0x1e0,0x1c2,0xa0,
                         DAT_0066733c,DAT_0065c890);
  }
  if (iVar3 == 0) {
    return 0;
  }
  param_1[0x7d] = 1;
  param_1[0x7c] = 1;
  if (param_1[0x65] == 2) {
    iVar3 = FUN_00474e20_NewObjectByName(s_Locke_005d330c,0,0xffffffff,1);
    param_1[0x7e] = iVar3;
    FUN_0046e6f0(s_Player_Name_005d3314);
  }
  (**(code **)(*param_1 + 0x50))();
  (**(code **)(*param_1 + 0x2c))(0);
  if (DAT_00667fd0 != &DAT_0065caf0) {
    param_1[0x67] = 0;
    param_1[0x66] = 0;
    return 1;
  }
  uVar7 = 0xffffffff;
  pcVar8 = *(char **)(param_1[0x7e] + 0x38);
  pcVar6 = pcVar8;
  do {
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  pcVar6 = (char *)FUN_00482ef0(~uVar7);
  uVar7 = 0xffffffff;
  do {
    pcVar10 = pcVar8;
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    pcVar10 = pcVar8 + 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar10;
  } while (cVar1 != '\0');
  uVar7 = ~uVar7;
  pcVar8 = pcVar10 + -uVar7;
  pcVar10 = pcVar6;
  for (uVar4 = uVar7 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar10 = *(undefined4 *)pcVar8;
    pcVar8 = pcVar8 + 4;
    pcVar10 = pcVar10 + 4;
  }
  for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
    *pcVar10 = *pcVar8;
    pcVar8 = pcVar8 + 1;
    pcVar10 = pcVar10 + 1;
  }
  iVar3 = param_1[0x7e];
  uVar7 = 0xffffffff;
  param_1[0x66] = (int)pcVar6;
  pcVar8 = (char *)(iVar3 + 0x378);
  do {
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar8 + 1;
  } while (cVar1 != '\0');
  pcVar6 = (char *)FUN_00482ef0(~uVar7);
  uVar7 = 0xffffffff;
  pcVar8 = (char *)(iVar3 + 0x378);
  do {
    pcVar10 = pcVar8;
    if (uVar7 == 0) break;
    uVar7 = uVar7 - 1;
    pcVar10 = pcVar8 + 1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar10;
  } while (cVar1 != '\0');
  uVar7 = ~uVar7;
  pcVar8 = pcVar10 + -uVar7;
  pcVar10 = pcVar6;
  for (uVar4 = uVar7 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar10 = *(undefined4 *)pcVar8;
    pcVar8 = pcVar8 + 4;
    pcVar10 = pcVar10 + 4;
  }
  for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
    *pcVar10 = *pcVar8;
    pcVar8 = pcVar8 + 1;
    pcVar10 = pcVar10 + 1;
  }
  param_1[0x67] = (int)pcVar6;
  return 1;
}


