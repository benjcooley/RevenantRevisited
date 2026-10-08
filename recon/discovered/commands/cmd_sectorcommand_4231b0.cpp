// FUN_004231b0 @ 004231b0 size=802

undefined4 FUN_004231b0(undefined4 param_1,int param_2)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  char *pcVar7;
  int iStack_27c;
  int iStack_278;
  int iStack_274;
  int iStack_270;
  int iStack_26c;
  undefined4 uStack_268;
  int iStack_264;
  int iStack_260;
  undefined ***pppuStack_25c;
  undefined4 uStack_258;
  undefined4 uStack_254;
  undefined4 uStack_250;
  undefined4 uStack_248;
  undefined4 uStack_244;
  undefined1 *puStack_240;
  undefined1 uStack_23c;
  undefined4 uStack_238;
  undefined **ppuStack_228;
  char *pcStack_224;
  char *pcStack_220;
  char *pcStack_21c;
  undefined4 uStack_218;
  char acStack_214 [512];
  void *pvStack_14;
  undefined1 *puStack_10;
  undefined4 uStack_c;
  
  uStack_c = 0xffffffff;
  puStack_10 = &LAB_0059c98f;
  pvStack_14 = ExceptionList;
  ExceptionList = &pvStack_14;
  pcVar2 = (char *)(**(code **)(**(int **)(param_2 + 0xc) + 0xc))();
  uVar5 = 0xffffffff;
  do {
    pcVar7 = pcVar2;
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    pcVar7 = pcVar2 + 1;
    cVar1 = *pcVar2;
    pcVar2 = pcVar7;
  } while (cVar1 != '\0');
  uVar5 = ~uVar5;
  pcVar2 = pcVar7 + -uVar5;
  pcVar7 = acStack_214;
  for (uVar6 = uVar5 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
    *(undefined4 *)pcVar7 = *(undefined4 *)pcVar2;
    pcVar2 = pcVar2 + 4;
    pcVar7 = pcVar7 + 4;
  }
  for (uVar5 = uVar5 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
    *pcVar7 = *pcVar2;
    pcVar2 = pcVar2 + 1;
    pcVar7 = pcVar7 + 1;
  }
  iVar3 = FUN_0047a410(param_2,&DAT_005cb6f0,&iStack_270);
  if (((iVar3 == 0) || (*(int *)(param_2 + 0x10) == 10)) || (*(int *)(param_2 + 0x10) == 9)) {
    ExceptionList = pvStack_14;
    return 4;
  }
  FUN_0058b100(&DAT_00654a88,s_Processing_command___s__on_level_005cb6f4,acStack_214,iStack_270);
  FUN_0041ee50(&DAT_00654a88);
  if (DAT_00666970 == iStack_270) {
    FUN_00458ff0();
    FUN_004546a0();
  }
  DAT_00655490 = 1;
  iStack_278 = 0;
  do {
    iStack_26c = 0;
    do {
      iVar3 = iStack_26c;
      iVar4 = FUN_00482fb0(0x15c);
      uStack_c = 0;
      if (iVar4 == 0) {
        iStack_274 = 0;
      }
      else {
        iStack_274 = FUN_00498020(iStack_270,iVar3,iStack_278);
      }
      uStack_c = 0xffffffff;
      FUN_004984d0(1);
      iStack_27c = 0;
      if (0 < *(int *)(iStack_274 + 0xb8)) {
        do {
          iVar3 = *(int *)(*(int *)(iStack_274 + 200) + iStack_27c * 4);
          if (iVar3 != 0) {
            uVar5 = 0xffffffff;
            pcStack_220 = acStack_214;
            pcVar2 = acStack_214;
            do {
              if (uVar5 == 0) break;
              uVar5 = uVar5 - 1;
              cVar1 = *pcVar2;
              pcVar2 = pcVar2 + 1;
            } while (cVar1 != '\0');
            ppuStack_228 = &PTR_LAB_005a36f8;
            pcStack_224 = s_String_005cb71c;
            pcStack_21c = acStack_214 + (~uVar5 - 1);
            uStack_218 = acStack_214;
            FUN_00478720();
            pppuStack_25c = &ppuStack_228;
            uStack_c = 1;
            uStack_268 = 0;
            iStack_264 = 0;
            iStack_260 = 0;
            uStack_258 = 0;
            uStack_254 = 0;
            uStack_250 = 0;
            uStack_248 = 0;
            uStack_244 = 0;
            uStack_23c = 0;
            uStack_238 = 1;
            puStack_240 = (undefined1 *)FUN_00482fb0(0x2000);
            *puStack_240 = 0;
            uStack_c = 2;
            FUN_00478a10();
            FUN_0041e8e0(iVar3,&uStack_268,1,0);
            uStack_c = 3;
            FUN_004830f0(puStack_240);
            if (iStack_264 == 0) {
              if (iStack_260 != 0) {
                FUN_004830f0(pppuStack_25c);
                FUN_004a1540(iStack_260);
              }
            }
            else {
              FUN_004830f0(pppuStack_25c);
              FUN_004830f0(iStack_264);
            }
            uStack_c = 0xffffffff;
            FUN_00478730();
          }
          iStack_27c = iStack_27c + 1;
          iVar3 = iStack_26c;
        } while (iStack_27c < *(int *)(iStack_274 + 0xb8));
      }
      FUN_00498a40();
      if (iStack_274 != 0) {
        FUN_00498190();
        FUN_004830f0(iStack_274);
      }
      iStack_26c = iVar3 + 1;
    } while (iStack_26c < 0x20);
    iStack_278 = iStack_278 + 1;
    if (0x1f < iStack_278) {
      iVar3 = *(int *)(param_2 + 0x10);
      while ((iVar3 != 9 && (iVar3 != 10))) {
        FUN_00479580();
        iVar3 = *(int *)(param_2 + 0x10);
      }
      DAT_00655490 = 0;
      ExceptionList = pvStack_14;
      return 0;
    }
  } while( true );
}


