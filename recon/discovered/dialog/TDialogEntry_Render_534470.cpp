// FUN_00534470 @ 00534470 size=1144

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Enum "SectionFlags": Some values do not have unique names */

void __fastcall FUN_00534470(int param_1)

{
  char *pcVar1;
  int *piVar2;
  IMAGE_SECTION_HEADER *pIVar3;
  int iVar4;
  int *piVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int *piVar8;
  undefined4 *puVar9;
  undefined4 uVar10;
  char **ppcVar11;
  int *piStack_13c;
  undefined4 uStack_138;
  undefined4 uStack_134;
  undefined4 uStack_130;
  undefined4 uStack_12c;
  undefined4 uStack_128;
  int iStack_124;
  int *piStack_120;
  char **ppcStack_11c;
  int *piStack_118;
  int iStack_114;
  undefined1 *puStack_110;
  char **ppcStack_10c;
  int *piStack_108;
  int iStack_104;
  undefined *puStack_100;
  undefined4 uStack_fc;
  undefined4 uStack_f8;
  undefined4 uStack_f4;
  int iStack_f0;
  int *piStack_ec;
  int *piStack_e8;
  IMAGE_SECTION_HEADER *pIStack_e4;
  char *pcStack_e0;
  undefined1 *puStack_dc;
  char *apcStack_84 [17];
  void *pvStack_40;
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 local_4;
  
  local_4 = 0xffffffff;
  puStack_8 = &LAB_005a1a70;
  pvStack_c = ExceptionList;
  puStack_dc = (undefined1 *)0x78;
  pcStack_e0 = (char *)0x534498;
  ExceptionList = &pvStack_c;
  piVar2 = (int *)FUN_00482fb0();
  local_4 = 0;
  if (piVar2 == (int *)0x0) {
    piVar2 = (int *)0x0;
  }
  else {
    uVar6 = *(undefined4 *)(*(int *)(param_1 + 0x48) + 8);
    uVar7 = *(undefined4 *)(*(int *)(param_1 + 0x48) + 4);
    puStack_dc = (undefined1 *)0x5344c2;
    FUN_004bcb00();
    pcStack_e0 = (char *)piVar2[4];
    puStack_dc = (undefined1 *)0x0;
    pIStack_e4 = &IMAGE_SECTION_HEADER_00400200;
    local_4 = CONCAT31(local_4._1_3_,1);
    *piVar2 = (int)&PTR_FUN_005a3980;
    piVar2[0x1a] = 0;
    iStack_f0 = 0x5344ea;
    piStack_ec = (int *)uVar7;
    piStack_e8 = (int *)uVar6;
    FUN_004a5740();
    piVar2[0x1c] = 1;
  }
  local_4 = 0xffffffff;
  pIVar3 = (IMAGE_SECTION_HEADER *)piVar2;
  if (DAT_006680c8 == 0) {
    puStack_dc = (undefined1 *)0x74;
    pcStack_e0 = (char *)0x53451b;
    pIVar3 = (IMAGE_SECTION_HEADER *)FUN_00482fb0();
    local_4 = 2;
    if (pIVar3 == (IMAGE_SECTION_HEADER *)0x0) {
      pIVar3 = (IMAGE_SECTION_HEADER *)(int *)0x0;
    }
    else {
      pcVar1 = *(char **)(*(int *)(param_1 + 0x48) + 8);
      uVar6 = *(undefined4 *)(*(int *)(param_1 + 0x48) + 4);
      puStack_dc = (undefined1 *)0x53454b;
      FUN_004bcb00();
      puStack_dc = (undefined1 *)0x488;
      local_4 = CONCAT31(local_4._1_3_,3);
      *(undefined ***)pIVar3 = &PTR_FUN_005a3e7c;
      piStack_e8 = (int *)0x53456f;
      pIStack_e4 = (IMAGE_SECTION_HEADER *)uVar6;
      pcStack_e0 = pcVar1;
      iVar4 = FUN_004bb5c0();
      if (iVar4 == 0) {
        puStack_dc = (undefined1 *)0x0;
        pcStack_e0 = s_Couldn_t_initialize_mosaic_surfa_005cdc70;
        pIStack_e4 = (IMAGE_SECTION_HEADER *)0x53457e;
        FUN_00481c10();
      }
    }
  }
  local_4 = 0xffffffff;
  piStack_ec = (int *)piVar2[2];
  iStack_f0 = piVar2[1];
  puStack_dc = (undefined1 *)0x80000000;
  pcStack_e0 = (char *)0x7f7f;
  pIStack_e4 = (IMAGE_SECTION_HEADER *)0xffff;
  piStack_e8 = (int *)0x1;
  uStack_f4 = 0;
  uStack_f8 = 0;
  uStack_fc = 0x5345b6;
  (**(code **)(*piVar2 + 100))();
  uStack_fc = 1;
  puStack_100 = (undefined *)0x5345bf;
  (**(code **)(*piVar2 + 0x1c))();
  iStack_104 = *(undefined4 *)(param_1 + 4);
  puStack_100 = (undefined *)0x0;
  piStack_108 = (int *)0x5345ce;
  piVar5 = (int *)FUN_00452690();
  if (piVar5 != (int *)0x0) {
    puStack_100 = (undefined *)0x5345dc;
    piVar5 = (int *)(**(code **)(*piVar5 + 0x130))();
    if (piVar5 != (int *)0x0) {
      puStack_100 = (undefined *)0x0;
      piVar5[6] = _DAT_006668d0;
      iStack_104 = 0x100;
      ppcStack_10c = (char **)(0x16 - piVar5[1] / 2);
      puStack_110 = (undefined1 *)(0x16 - *piVar5 / 2);
      iStack_114 = 0x534617;
      piStack_108 = piVar5;
      FUN_004bd680();
    }
  }
  if (DAT_006680c8 == 0) {
    piStack_ec = (int *)0xffffff;
    pcStack_e0 = (char *)0xffffff;
  }
  else {
    pcStack_e0 = *(char **)(param_1 + 0xc);
  }
  piStack_e8 = (int *)0x0;
  if (0 < *(int *)(param_1 + 0x5c)) {
    piVar5 = (int *)(param_1 + 0x60);
    piVar8 = (int *)(param_1 + 0x80);
    piStack_ec = piVar5;
    do {
      puStack_100 = (undefined *)0x80000000;
      iStack_104 = 0x401;
      puStack_dc = (undefined1 *)&piStack_108;
      ppcStack_10c = &pcStack_e0;
      puStack_110 = (undefined1 *)0x534675;
      piStack_108 = piVar5;
      FUN_00419dd0();
      ppcStack_10c = DAT_0065c134;
      puStack_110 = (undefined1 *)0x0;
      iStack_114 = *piStack_ec;
      piStack_118 = (int *)0x53468a;
      piStack_118 = (int *)FUN_00444e10();
      ppcStack_11c = (char **)0x534692;
      ppcStack_11c = (char **)FUN_00444e00();
      iStack_124 = *piVar8;
      piStack_120 = (int *)piVar8[1];
      uStack_128 = 0x5346a1;
      FUN_004be2b0();
      piStack_ec = piStack_ec + 1;
      piStack_e8 = (int *)((int)piStack_e8 + 1);
      piVar5 = *(int **)(param_1 + 0x5c);
      piVar8 = piVar8 + 4;
      pIVar3 = pIStack_e4;
    } while ((int)piStack_e8 < (int)piVar5);
  }
  if (DAT_006680c8 == 0) {
    iStack_104 = piVar2[2];
    piStack_108 = (int *)piVar2[1];
    puStack_100 = (undefined *)0x100;
    ppcStack_10c = (char **)0x0;
    puStack_110 = (undefined1 *)0x0;
    iStack_114 = 0;
    ppcStack_11c = apcStack_84;
    piStack_118 = (int *)0x0;
    piStack_120 = (int *)0x5346e9;
    FUN_00438d80();
    ppcStack_10c = apcStack_84;
    puStack_100 = (undefined *)0x0;
    iStack_104 = 0;
    puStack_110 = (undefined1 *)0x5346fb;
    piStack_108 = piVar2;
    (**(code **)(*(int *)pIVar3 + 0x5c))();
  }
  puStack_100 = &DAT_005e3f4c;
  iStack_104 = 0x53470b;
  piStack_108 = (int *)FUN_0046d710();
  puStack_100 = (undefined *)0x0;
  iStack_104 = 0x2000;
  ppcStack_10c = (char **)(0x16 - piStack_108[1] / 2);
  puStack_110 = (undefined1 *)(0x16 - *piStack_108 / 2);
  iStack_114 = 0x53473a;
  FUN_004bd680();
  iStack_104 = *(int *)((int)pIVar3 + 8);
  piStack_108 = *(int **)((int)pIVar3 + 4);
  piStack_e8 = *(int **)(param_1 + 0x48);
  puStack_100 = (undefined *)0x80000000;
  ppcStack_10c = (char **)0x0;
  puStack_110 = (undefined1 *)0x0;
  iStack_114 = 0;
  ppcStack_11c = apcStack_84;
  piStack_118 = (int *)0x0;
  piStack_120 = (int *)0x53475f;
  FUN_00438d80();
  ppcStack_10c = apcStack_84;
  puStack_100 = (undefined *)0x0;
  iStack_104 = 0;
  puStack_110 = (undefined1 *)0x534773;
  piStack_108 = (int *)pIVar3;
  (**(code **)(*piStack_e8 + 0x5c))();
  if (DAT_006680c8 != 0) {
    piStack_120 = (int *)piVar2[2];
    iStack_124 = piVar2[1];
    puStack_110 = (undefined1 *)0x80000000;
    iStack_114 = 0x7f7f;
    piStack_118 = (int *)0xffff;
    ppcStack_11c = (char **)0x1;
    uStack_128 = 0;
    uStack_12c = 0;
    uStack_130 = 0x5347a1;
    (**(code **)(*piVar2 + 100))();
    uStack_130 = 1;
    uStack_134 = 0x5347aa;
    (**(code **)(*piVar2 + 0x1c))();
    ppcStack_11c = (char **)0x0;
    if (0 < *(int *)(param_1 + 0x5c)) {
      piStack_13c = (int *)(param_1 + 0x60);
      puVar9 = (undefined4 *)(param_1 + 0x80);
      piStack_120 = piStack_13c;
      do {
        uStack_134 = 0x80000000;
        uStack_138 = 0x401;
        puStack_110 = (undefined1 *)&piStack_13c;
        FUN_00419dd0(param_1 + 0x10);
        uVar10 = 0;
        iVar4 = *piStack_120;
        ppcVar11 = DAT_0065c134;
        uVar6 = FUN_00444e10(iVar4,0,DAT_0065c134);
        uVar7 = FUN_00444e00(uVar6);
        FUN_004be2b0(*puVar9,puVar9[1],uVar7,uVar6,iVar4,uVar10,ppcVar11);
        piStack_120 = piStack_120 + 1;
        ppcStack_11c = (char **)((int)ppcStack_11c + 1);
        piStack_13c = *(int **)(param_1 + 0x5c);
        puVar9 = puVar9 + 4;
        pIVar3 = (IMAGE_SECTION_HEADER *)piStack_118;
      } while ((int)ppcStack_11c < (int)piStack_13c);
    }
    puStack_dc = *(undefined1 **)((int)pIVar3 + 4);
    uStack_134 = 0;
    uStack_138 = 0;
    ppcStack_10c = (char **)0x80000000;
    piStack_108 = (int *)0x0;
    iStack_104 = 0;
    puStack_100 = (undefined *)0x0;
    pIStack_e4 = (IMAGE_SECTION_HEADER *)0x0;
    pcStack_e0 = (char *)0x0;
    uStack_f8 = 0;
    uStack_fc = 0;
    piStack_e8 = (int *)0x0;
    piStack_ec = (int *)0x0;
    iStack_f0 = 0;
    uStack_f4 = 0;
    piStack_13c = (int *)pIVar3;
    (**(code **)(**(int **)(param_1 + 0x4c) + 0x5c))(&ppcStack_10c);
  }
  if (piVar2 != (int *)0x0) {
    puStack_110 = (undefined1 *)0x1;
    iStack_114 = 0x5348bb;
    (**(code **)*piVar2)();
  }
  if ((DAT_006680c8 == 0) && (pIVar3 != (IMAGE_SECTION_HEADER *)0x0)) {
    puStack_110 = (undefined1 *)0x1;
    iStack_114 = 0x5348cf;
    (*(code *)**(undefined4 **)pIVar3)();
  }
  ExceptionList = pvStack_40;
  return;
}


