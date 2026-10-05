// FUN_00460620 @ 00460620 size=967

undefined4 __fastcall FUN_00460620(undefined4 *param_1)

{
  int *piVar1;
  char cVar2;
  int iVar3;
  bool bVar4;
  bool bVar5;
  int iVar6;
  undefined4 *puVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  char *pcVar11;
  char *pcVar12;
  undefined4 *puVar13;
  byte local_430 [15];
  undefined1 auStack_421 [5];
  char local_41c [258];
  char acStack_31a [261];
  char cStack_215;
  undefined4 local_214 [64];
  char acStack_112 [262];
  void *pvStack_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059d121;
  pvStack_c = ExceptionList;
  piVar1 = param_1 + 1;
  ExceptionList = &pvStack_c;
  param_1[6] = 0xffffffff;
  param_1[7] = 0xffffffff;
  iVar10 = 0;
  if (0 < *piVar1) {
    do {
      if ((-1 < iVar10) && (*(int *)(param_1[5] + iVar10 * 4) != 0)) {
        FUN_004616e0(1);
      }
      FUN_0041cb40(iVar10);
      iVar10 = iVar10 + 1;
    } while (iVar10 < *piVar1);
  }
  *piVar1 = 0;
  param_1[2] = 0;
  FUN_00483120(&DAT_0065d6a4,acStack_31a + 2,0x104);
  uVar8 = 0xffffffff;
  pcVar11 = acStack_31a + 2;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar2 = *pcVar11;
    pcVar11 = pcVar11 + 1;
  } while (cVar2 != '\0');
  if (acStack_31a[~uVar8] == '\\') {
    uVar8 = 0xffffffff;
    pcVar11 = acStack_31a + 2;
    do {
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      cVar2 = *pcVar11;
      pcVar11 = pcVar11 + 1;
    } while (cVar2 != '\0');
    acStack_31a[~uVar8] = '\0';
  }
  CreateDirectoryA(acStack_31a + 2,(LPSECURITY_ATTRIBUTES)0x0);
  uVar8 = 0xffffffff;
  pcVar11 = acStack_31a + 2;
  do {
    pcVar12 = pcVar11;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar12 = pcVar11 + 1;
    cVar2 = *pcVar11;
    pcVar11 = pcVar12;
  } while (cVar2 != '\0');
  uVar8 = ~uVar8;
  pcVar11 = pcVar12 + -uVar8;
  pcVar12 = (char *)local_214;
  for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined4 *)pcVar12 = *(undefined4 *)pcVar11;
    pcVar11 = pcVar11 + 4;
    pcVar12 = pcVar12 + 4;
  }
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *pcVar12 = *pcVar11;
    pcVar11 = pcVar11 + 1;
    pcVar12 = pcVar12 + 1;
  }
  uVar8 = 0xffffffff;
  pcVar11 = (char *)local_214;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar2 = *pcVar11;
    pcVar11 = pcVar11 + 1;
  } while (cVar2 != '\0');
  iVar10 = -(~uVar8 - 1);
  pcVar11 = (char *)((int)local_214 + (~uVar8 - 1));
  _strncpy(pcVar11,&DAT_005d0d28,iVar10 + 0x103);
  pcVar11[iVar10 + 0x103] = '\0';
  iVar10 = FUN_0058c680(local_214,local_430);
  bVar4 = true;
  if (iVar10 != -1) {
    while (bVar4) {
      bVar5 = false;
      FUN_0059bd3e(local_41c);
      if (((local_430[0] & 0x10) == 0) || (local_41c[0] == '.')) {
        iVar6 = FUN_0058ad30(local_41c,&DAT_005d0d30);
        if (iVar6 != 0) {
          FUN_0058b100(acStack_112 + 2,s__s__s_005d0d38,acStack_31a + 2,local_41c);
          iVar6 = FUN_0049ee20(acStack_112 + 2,1);
          if (iVar6 == 0) {
            FUN_00481c10(s_Unable_to_open_module_file__s_005d0d40,acStack_112 + 2);
          }
          if ((DAT_0065b2f8 & 8) != 0) {
            FUN_00481c10(s_Compressed_files_found_in_module_005d0d60,acStack_112 + 2);
          }
          uVar8 = 0xffffffff;
          bVar5 = true;
          pcVar11 = local_41c;
          do {
            if (uVar8 == 0) break;
            uVar8 = uVar8 - 1;
            cVar2 = *pcVar11;
            pcVar11 = pcVar11 + 1;
          } while (cVar2 != '\0');
          auStack_421[~uVar8] = 0;
          goto LAB_00460836;
        }
      }
      else {
LAB_00460836:
        puVar7 = (undefined4 *)FUN_00482fb0(0x920);
        uStack_4 = 0;
        if (puVar7 == (undefined4 *)0x0) {
          puVar7 = (undefined4 *)0x0;
        }
        else {
          FUN_0041c7f0(4,4);
          *puVar7 = 0;
          *(undefined1 *)(puVar7 + 0x16) = 0;
          *(undefined1 *)(puVar7 + 2) = 0;
          puVar7[0x21] = 0;
          uStack_4 = CONCAT31(uStack_4._1_3_,1);
          puVar13 = puVar7 + 0x27;
          for (iVar6 = 0x216; iVar6 != 0; iVar6 = iVar6 + -1) {
            *puVar13 = 0;
            puVar13 = puVar13 + 1;
          }
          puVar13 = puVar7 + 0x23d;
          for (iVar6 = 0xb; iVar6 != 0; iVar6 = iVar6 + -1) {
            *puVar13 = 0;
            puVar13 = puVar13 + 1;
          }
          FUN_00461700();
        }
        uStack_4 = 0xffffffff;
        iVar6 = FUN_0045f980(local_41c,0);
        if (iVar6 == 0) {
          if (puVar7 != (undefined4 *)0x0) {
            iVar6 = 0;
            uStack_4 = 2;
            if (0 < (int)puVar7[0x22]) {
              do {
                if ((-1 < iVar6) && (iVar3 = *(int *)(puVar7[0x26] + iVar6 * 4), iVar3 != 0)) {
                  FUN_004830f0(iVar3);
                }
                FUN_0041cb40(iVar6);
                iVar6 = iVar6 + 1;
              } while (iVar6 < (int)puVar7[0x22]);
            }
            puVar7[0x22] = 0;
            puVar7[0x23] = 0;
            uStack_4 = 0xffffffff;
            FUN_004830f0(puVar7[0x26]);
            FUN_004830f0(puVar7);
          }
        }
        else {
          puVar7[1] = param_1[1];
          FUN_0041c840(puVar7);
          iVar6 = FUN_0059a530(puVar7 + 0x16,&DAT_0065c958);
          if (iVar6 == 0) {
            param_1[6] = param_1[1] + -1;
          }
        }
        if (bVar5) {
          FUN_0049eff0();
        }
      }
      iVar6 = FUN_0058c74d(iVar10,local_430);
      if (iVar6 != 0) {
        bVar4 = false;
      }
    }
  }
  if ((int)param_1[1] < 1) {
    FUN_00481c10(s_There_are_no_modules_to_play_005d0d8c,0);
  }
  if ((int)param_1[6] < 0) {
    FUN_00481c10(s_Unable_to_find_main_module___s__005d0dac,&DAT_0065c958);
  }
  *param_1 = 1;
  ExceptionList = pvStack_c;
  return 1;
}


