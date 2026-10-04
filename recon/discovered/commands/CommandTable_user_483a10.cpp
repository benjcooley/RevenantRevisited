// FUN_00483a10 @ 00483a10 size=425

void FUN_00483a10(undefined4 param_1)

{
  int *piVar1;
  char cVar2;
  int iVar3;
  undefined **ppuVar4;
  undefined *puVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  int *piVar9;
  char *pcVar10;
  char *pcVar11;
  char *pcVar12;
  char local_200 [256];
  char local_100 [256];
  
  iVar3 = FUN_0058b5db(param_1,&DAT_005d7e24);
  if (iVar3 != 0) {
    if (PTR_s_activate_005c6e88 != (undefined *)0x0) {
      ppuVar4 = &PTR_s_activate_005c6e88;
      piVar9 = &DAT_005c6e9c;
      do {
        FUN_0058b56e(iVar3,&DAT_005d7e28,*ppuVar4);
        FUN_0058b56e(iVar3,&DAT_005d7e2c,piVar9[1]);
        puVar5 = &DAT_005d7e34;
        if (*piVar9 == 0) {
          puVar5 = &DAT_005d7e38;
        }
        FUN_0058b56e(iVar3,s_Available_in_Editor_Only___s_005d7e3c,puVar5);
        puVar5 = &DAT_005d7e5c;
        if (piVar9[-1] == 0) {
          puVar5 = &DAT_005d7e60;
        }
        FUN_0058b56e(iVar3,s_Parameters_Required____s_005d7e64,puVar5);
        uVar6 = 0xffffffff;
        pcVar10 = s_Context_Type_s____005d7e84;
        do {
          pcVar12 = pcVar10;
          if (uVar6 == 0) break;
          uVar6 = uVar6 - 1;
          pcVar12 = pcVar10 + 1;
          cVar2 = *pcVar10;
          pcVar10 = pcVar12;
        } while (cVar2 != '\0');
        uVar6 = ~uVar6;
        iVar8 = piVar9[-3];
        pcVar10 = pcVar12 + -uVar6;
        pcVar12 = local_200;
        for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
          *(undefined4 *)pcVar12 = *(undefined4 *)pcVar10;
          pcVar10 = pcVar10 + 4;
          pcVar12 = pcVar12 + 4;
        }
        for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
          *pcVar12 = *pcVar10;
          pcVar10 = pcVar10 + 1;
          pcVar12 = pcVar12 + 1;
        }
        FUN_00483850(iVar8,local_100);
        uVar6 = 0xffffffff;
        pcVar10 = local_100;
        do {
          pcVar12 = pcVar10;
          if (uVar6 == 0) break;
          uVar6 = uVar6 - 1;
          pcVar12 = pcVar10 + 1;
          cVar2 = *pcVar10;
          pcVar10 = pcVar12;
        } while (cVar2 != '\0');
        uVar6 = ~uVar6;
        iVar8 = -1;
        pcVar10 = local_200;
        do {
          pcVar11 = pcVar10;
          if (iVar8 == 0) break;
          iVar8 = iVar8 + -1;
          pcVar11 = pcVar10 + 1;
          cVar2 = *pcVar10;
          pcVar10 = pcVar11;
        } while (cVar2 != '\0');
        pcVar10 = pcVar12 + -uVar6;
        pcVar12 = pcVar11 + -1;
        for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
          *(undefined4 *)pcVar12 = *(undefined4 *)pcVar10;
          pcVar10 = pcVar10 + 4;
          pcVar12 = pcVar12 + 4;
        }
        for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
          *pcVar12 = *pcVar10;
          pcVar10 = pcVar10 + 1;
          pcVar12 = pcVar12 + 1;
        }
        iVar8 = FUN_00483850(piVar9[-2],local_100);
        if (iVar8 != 0) {
          iVar8 = -1;
          pcVar10 = local_200;
          do {
            pcVar12 = pcVar10;
            if (iVar8 == 0) break;
            iVar8 = iVar8 + -1;
            pcVar12 = pcVar10 + 1;
            cVar2 = *pcVar10;
            pcVar10 = pcVar12;
          } while (cVar2 != '\0');
          uVar6 = 0xffffffff;
          *(undefined2 *)(pcVar12 + -1) = DAT_005d7ea0;
          pcVar12[1] = DAT_005d7ea2;
          pcVar10 = local_100;
          do {
            pcVar12 = pcVar10;
            if (uVar6 == 0) break;
            uVar6 = uVar6 - 1;
            pcVar12 = pcVar10 + 1;
            cVar2 = *pcVar10;
            pcVar10 = pcVar12;
          } while (cVar2 != '\0');
          uVar6 = ~uVar6;
          iVar8 = -1;
          pcVar10 = local_200;
          do {
            pcVar11 = pcVar10;
            if (iVar8 == 0) break;
            iVar8 = iVar8 + -1;
            pcVar11 = pcVar10 + 1;
            cVar2 = *pcVar10;
            pcVar10 = pcVar11;
          } while (cVar2 != '\0');
          pcVar10 = pcVar12 + -uVar6;
          pcVar12 = pcVar11 + -1;
          for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
            *(undefined4 *)pcVar12 = *(undefined4 *)pcVar10;
            pcVar10 = pcVar10 + 4;
            pcVar12 = pcVar12 + 4;
          }
          for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
            *pcVar12 = *pcVar10;
            pcVar10 = pcVar10 + 1;
            pcVar12 = pcVar12 + 1;
          }
        }
        FUN_0058b56e(iVar3,&DAT_005d7ea4,local_200);
        FUN_0058b56e(iVar3,&DAT_005d7eac);
        piVar1 = piVar9 + 2;
        ppuVar4 = (undefined **)(piVar9 + 2);
        piVar9 = piVar9 + 7;
      } while (*piVar1 != 0);
    }
    FUN_0058b4f1(iVar3);
  }
  return;
}


