// FUN_004865a0 @ 004865a0 size=3410

/* WARNING: Removing unreachable block (ram,0x004865dc) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_004865a0(undefined4 param_1,undefined4 param_2,char *param_3)

{
  char cVar1;
  DWORD *pDVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  DWORD DVar5;
  LPCSTR lpText;
  LPCSTR lpCaption;
  int iVar6;
  undefined *puVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  char *pcVar11;
  undefined1 *puVar12;
  char *pcVar13;
  char *pcVar14;
  _STARTUPINFOA *p_Var15;
  bool bVar16;
  undefined1 local_750 [256];
  char local_650 [260];
  undefined1 local_54c [128];
  CHAR local_4cc [128];
  byte local_44c [20];
  undefined1 local_438 [259];
  char cStack_335;
  undefined4 local_334 [64];
  char acStack_232 [261];
  undefined1 local_12d;
  undefined1 local_12c [128];
  CHAR local_ac [6];
  char acStack_a6 [2];
  undefined4 local_a4 [4];
  CHAR local_94 [36];
  _STARTUPINFOA local_70;
  DWORD local_2c;
  DWORD local_28;
  uint local_24;
  undefined1 local_20 [28];
  
  InitializeCriticalSection((LPCRITICAL_SECTION)&DAT_0065b7c0);
  DAT_0065c138 = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,(LPCSTR)0x0);
  DAT_0065b8ac = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,0,(LPCSTR)0x0);
  pDVar2 = (DWORD *)cpuid_Version_info(1);
  local_28 = *pDVar2;
  local_24 = pDVar2[2];
  DAT_006680f8 = local_24 >> 0x17 & 1;
  _DAT_006672f0 = 0x20;
  GlobalMemoryStatus((LPMEMORYSTATUS)&DAT_006672f0);
  DAT_00667c28 = param_1;
  SetErrorMode(0x8001);
  FUN_0058d7a1(0x780);
  uVar4 = FUN_0058c5a4(0);
  FUN_0058c575(uVar4);
  pcVar13 = param_3;
  FUN_0059be72(param_3);
  FUN_00483670(pcVar13,&DAT_006666cc,&DAT_0065d254);
  FUN_0058d715(&DAT_006666cc);
  uVar8 = 0xffffffff;
  pcVar11 = &DAT_0065d254;
  do {
    pcVar14 = pcVar11;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar14 = pcVar11 + 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar14;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
  pcVar11 = pcVar14 + -uVar8;
  pcVar14 = (char *)&DAT_006663e0;
  for (uVar9 = uVar8 >> 2; uVar4 = s_Revenant_ini_005d7c34._8_4_, uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined4 *)pcVar14 = *(undefined4 *)pcVar11;
    pcVar11 = pcVar11 + 4;
    pcVar14 = pcVar14 + 4;
  }
  for (uVar8 = uVar8 & 3; uVar3 = s_Revenant_ini_005d7c34._4_4_, uVar8 != 0; uVar8 = uVar8 - 1) {
    *pcVar14 = *pcVar11;
    pcVar11 = pcVar11 + 1;
    pcVar14 = pcVar14 + 1;
  }
  iVar10 = -1;
  pcVar11 = (char *)&DAT_006663e0;
  do {
    pcVar14 = pcVar11;
    if (iVar10 == 0) break;
    iVar10 = iVar10 + -1;
    pcVar14 = pcVar11 + 1;
    cVar1 = *pcVar11;
    pcVar11 = pcVar14;
  } while (cVar1 != '\0');
  *(undefined4 *)(pcVar14 + -1) = s_Revenant_ini_005d7c34._0_4_;
  cVar1 = s_Revenant_ini_005d7c34[0xc];
  *(undefined4 *)(pcVar14 + 3) = uVar3;
  *(undefined4 *)(pcVar14 + 7) = uVar4;
  pcVar14[0xb] = cVar1;
  iVar10 = FUN_0059a530(&DAT_006666cc,&DAT_0065d254);
  if ((iVar10 != 0) && (iVar10 = FUN_0058b5db(&DAT_006663e0,&DAT_005d7c44), iVar10 == 0)) {
    uVar8 = 0xffffffff;
    pcVar11 = &DAT_006666cc;
    do {
      pcVar14 = pcVar11;
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      pcVar14 = pcVar11 + 1;
      cVar1 = *pcVar11;
      pcVar11 = pcVar14;
    } while (cVar1 != '\0');
    uVar8 = ~uVar8;
    pcVar11 = pcVar14 + -uVar8;
    pcVar14 = (char *)local_334;
    for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
      *(undefined4 *)pcVar14 = *(undefined4 *)pcVar11;
      pcVar11 = pcVar11 + 4;
      pcVar14 = pcVar14 + 4;
    }
    for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
      *pcVar14 = *pcVar11;
      pcVar11 = pcVar11 + 1;
      pcVar14 = pcVar14 + 1;
    }
    iVar10 = -1;
    pcVar11 = (char *)local_334;
    do {
      pcVar14 = pcVar11;
      if (iVar10 == 0) break;
      iVar10 = iVar10 + -1;
      pcVar14 = pcVar11 + 1;
      cVar1 = *pcVar11;
      pcVar11 = pcVar14;
    } while (cVar1 != '\0');
    *(undefined4 *)(pcVar14 + -1) = s_Revenant_ini_005d7c34._0_4_;
    *(undefined4 *)(pcVar14 + 3) = s_Revenant_ini_005d7c34._4_4_;
    *(undefined4 *)(pcVar14 + 7) = s_Revenant_ini_005d7c34._8_4_;
    pcVar14[0xb] = s_Revenant_ini_005d7c34[0xc];
    FUN_004814d0(local_334,&DAT_006663e0);
  }
  DVar5 = GetVersion();
  param_1 = CONCAT13((char)DVar5,(undefined3)param_1);
  if (3 < DAT_005d79e4) {
    DAT_005d79f8 = 0;
  }
  FUN_00484500();
  FUN_00484ae0();
  FUN_00483bc0(pcVar13,0);
  uVar8 = 0xffffffff;
  do {
    pcVar11 = pcVar13;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar11 = pcVar13 + 1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar11;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
  pcVar13 = pcVar11 + -uVar8;
  pcVar11 = (char *)&DAT_0066819c;
  for (uVar9 = uVar8 >> 2; cVar1 = DAT_0065dde8, uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined4 *)pcVar11 = *(undefined4 *)pcVar13;
    pcVar13 = pcVar13 + 4;
    pcVar11 = pcVar11 + 4;
  }
  bVar16 = DAT_0065dde8 == '\\';
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *pcVar11 = *pcVar13;
    pcVar13 = pcVar13 + 1;
    pcVar11 = pcVar11 + 1;
  }
  puVar12 = &DAT_0065dde8;
  if ((bVar16) || (DAT_0065dde9 == ':')) {
LAB_004867fd:
    _strncpy(acStack_232 + 2,&DAT_0065dde8,0x103);
    local_12d = 0;
  }
  else {
    if (cVar1 == '.') {
      if (DAT_0065dde9 == '.') goto LAB_004867fd;
      puVar12 = &DAT_0065dde9;
      cVar1 = DAT_0065dde9;
      while (cVar1 == '\\') {
        pcVar13 = puVar12 + 1;
        puVar12 = puVar12 + 1;
        cVar1 = *pcVar13;
      }
    }
    _strncpy(acStack_232 + 2,&DAT_0065d254,0x103);
    local_12d = 0;
    FUN_00487bd0(acStack_232 + 2,puVar12,0x104);
  }
  uVar8 = 0xffffffff;
  pcVar13 = acStack_232 + 2;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  acStack_232[~uVar8] = '\0';
  uVar8 = 0xffffffff;
  pcVar13 = acStack_232 + 2;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  _strncpy(acStack_232 + ~uVar8 + 1,&DAT_005d8c58,0x103 - (~uVar8 - 1));
  local_12d = 0;
  FUN_0049ee20(acStack_232 + 2,1);
  if (DAT_0065b2fc == 2) {
    iVar10 = 0;
  }
  else {
    iVar10 = DAT_0065b2fc;
    if (DAT_0065b2fc == 10) goto LAB_00487219;
  }
  if ((DAT_00666448 & 8) != 0) {
LAB_00487219:
    if (DAT_0065b9f8 != 0) {
                    /* WARNING: Subroutine does not return */
      FUN_0058d065(1);
    }
    DAT_0065b9f8 = 1;
    FUN_0058b100(local_94,s_Compressed_files_found_in_resour_005d8c60,0);
    FUN_004820b0(local_94);
    FUN_004820b0(s_Press_any_key_to_exit_005d7bdc);
    DAT_00667fd0 = 0;
    FUN_0049e410();
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x6c))();
    FUN_004a7460();
    FUN_00448c90();
    iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
    while (iVar10 != 0) {
      TranslateMessage((MSG *)local_20);
      DispatchMessageA((MSG *)local_20);
      iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
    }
    ShowWindow(DAT_0065b8ec,0);
    MessageBoxA((HWND)0x0,local_94,s_FATAL_ERROR_005d7bf4,0x10);
                    /* WARNING: Subroutine does not return */
    FUN_0058d065(1);
  }
  if (iVar10 != 0) {
    if (DAT_0065b9f8 != 0) {
                    /* WARNING: Subroutine does not return */
      FUN_0058d065(1);
    }
    DAT_0065b9f8 = 1;
    FUN_0058b100(local_94,s_Error_in_pack_file_RESOURCE_RVR_005d8c98,0);
    FUN_004820b0(local_94);
    FUN_004820b0(s_Press_any_key_to_exit_005d7bdc);
    DAT_00667fd0 = 0;
    FUN_0049e410();
    (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x6c))();
    FUN_004a7460();
    FUN_00448c90();
    iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
    while (iVar10 != 0) {
      TranslateMessage((MSG *)local_20);
      DispatchMessageA((MSG *)local_20);
      iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
    }
    ShowWindow(DAT_0065b8ec,0);
    MessageBoxA((HWND)0x0,local_94,s_FATAL_ERROR_005d7bf4,0x10);
                    /* WARNING: Subroutine does not return */
    FUN_0058d065(1);
  }
  puVar12 = &DAT_0065bc44;
  if ((DAT_0065bc44 == '\\') || (DAT_0065bc45 == ':')) {
LAB_004869ea:
    _strncpy(acStack_232 + 2,&DAT_0065bc44,0x103);
    local_12d = 0;
  }
  else {
    if (DAT_0065bc44 == '.') {
      if (DAT_0065bc45 == '.') goto LAB_004869ea;
      puVar12 = &DAT_0065bc45;
      cVar1 = DAT_0065bc45;
      while (cVar1 == '\\') {
        pcVar13 = puVar12 + 1;
        puVar12 = puVar12 + 1;
        cVar1 = *pcVar13;
      }
    }
    _strncpy(acStack_232 + 2,&DAT_0065d254,0x103);
    local_12d = 0;
    FUN_00487bd0(acStack_232 + 2,puVar12,0x104);
  }
  uVar8 = 0xffffffff;
  pcVar13 = acStack_232 + 2;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  acStack_232[~uVar8] = '\0';
  uVar8 = 0xffffffff;
  pcVar13 = acStack_232 + 2;
  do {
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  _strncpy(acStack_232 + ~uVar8 + 1,&DAT_005d8cb8,0x103 - (~uVar8 - 1));
  local_12d = 0;
  FUN_0049ee20(acStack_232 + 2,1);
  if (DAT_0065b2fc == 2) {
    iVar10 = 0;
  }
  else {
    iVar10 = DAT_0065b2fc;
    if (DAT_0065b2fc == 10) goto LAB_0048713e;
  }
  if ((DAT_0065c3c8 & 8) == 0) {
    if (iVar10 != 0) {
      if (DAT_0065b9f8 != 0) {
                    /* WARNING: Subroutine does not return */
        FUN_0058d065(1);
      }
      DAT_0065b9f8 = 1;
      FUN_0058b100(local_94,s_Error_in_pack_file_IMAGERY_RVI_005d8cf4,0);
      FUN_004820b0(local_94);
      FUN_004820b0(s_Press_any_key_to_exit_005d7bdc);
      DAT_00667fd0 = 0;
      FUN_0049e410();
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x6c))();
      FUN_004a7460();
      FUN_00448c90();
      iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
      while (iVar10 != 0) {
        TranslateMessage((MSG *)local_20);
        DispatchMessageA((MSG *)local_20);
        iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
      }
      ShowWindow(DAT_0065b8ec,0);
      MessageBoxA((HWND)0x0,local_94,s_FATAL_ERROR_005d7bf4,0x10);
                    /* WARNING: Subroutine does not return */
      FUN_0058d065(1);
    }
    if ((DAT_0065c558 == 0) && (iVar10 = FUN_00487300(&DAT_0065bc44,0,1), iVar10 == 0)) {
      uVar8 = 0xffffffff;
      pcVar13 = &DAT_0065dde8;
      do {
        pcVar11 = pcVar13;
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        pcVar11 = pcVar13 + 1;
        cVar1 = *pcVar13;
        pcVar13 = pcVar11;
      } while (cVar1 != '\0');
      uVar8 = ~uVar8;
      pcVar13 = pcVar11 + -uVar8;
      pcVar11 = &DAT_0065bc44;
      for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
        *(undefined4 *)pcVar11 = *(undefined4 *)pcVar13;
        pcVar13 = pcVar13 + 4;
        pcVar11 = pcVar11 + 4;
      }
      for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
        *pcVar11 = *pcVar13;
        pcVar13 = pcVar13 + 1;
        pcVar11 = pcVar11 + 1;
      }
    }
    FUN_0049ceb0();
    if (param_1._3_1_ < 4) {
      if (DAT_0065b9f8 != 0) {
                    /* WARNING: Subroutine does not return */
        FUN_0058d065(1);
      }
      DAT_0065b9f8 = 1;
      FUN_0058b100(local_94,s_This_game_requires_Windows__95_N_005d8d14,0);
      FUN_004820b0(local_94);
      FUN_004820b0(s_Press_any_key_to_exit_005d7bdc);
      DAT_00667fd0 = 0;
      FUN_0049e410();
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x6c))();
      FUN_004a7460();
      FUN_00448c90();
      iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
      while (iVar10 != 0) {
        TranslateMessage((MSG *)local_20);
        DispatchMessageA((MSG *)local_20);
        iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
      }
      ShowWindow(DAT_0065b8ec,0);
      MessageBoxA((HWND)0x0,local_94,s_FATAL_ERROR_005d7bf4,0x10);
                    /* WARNING: Subroutine does not return */
      FUN_0058d065(1);
    }
    FUN_00484400(&param_1);
    local_ac[0] = '\0';
    GetVolumeInformationA
              ((LPCSTR)&param_1,local_ac,0x80,&local_2c,&local_24,&local_28,local_4cc,0x80);
    iVar10 = FUN_0059a530(local_ac,s_REVENANT_DISK2_005d8d48);
    if (iVar10 == 0) {
      lpText = (LPCSTR)FUN_0049d800(s_DISK2_005d8d58);
      lpCaption = (LPCSTR)FUN_0049d800(s_DISK2TITLE_005d8d60);
      MessageBoxA((HWND)0x0,lpText,lpCaption,0x30);
    }
    FUN_0058d3f2(s_revboot_log_005d8d6c,&param_1,local_54c,local_750,local_12c);
    FUN_0058b100(local_650,&DAT_005d7b48,&param_1,local_54c);
    iVar6 = FUN_0058c680(s_revboot_log_005d8d6c,local_44c);
    iVar10 = iVar6;
    while (iVar10 != -1) {
      if ((local_44c[0] & 0x16) == 0) {
        _strncpy((char *)local_334,local_650,0x103);
        uVar8 = 0xffffffff;
        acStack_232[1] = 0;
        pcVar13 = (char *)local_334;
        do {
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          cVar1 = *pcVar13;
          pcVar13 = pcVar13 + 1;
        } while (cVar1 != '\0');
        FUN_00439fd0((int)local_334 + (~uVar8 - 1),local_438,0x104 - (~uVar8 - 1));
        DeleteFileA((LPCSTR)local_334);
      }
      iVar10 = FUN_0058c74d(iVar6,local_44c);
    }
    uVar4 = FUN_0058b5db(s_revboot_log_005d8d7c,&DAT_005d8d78);
    FUN_0058b56e(uVar4,s_COMMAND_LINE___s_005d8d88,param_3);
    FUN_0058c899(uVar4);
    FUN_0058b4f1(uVar4);
    iVar10 = FUN_00480440();
    if (iVar10 == 0) {
      iVar10 = 1;
    }
    else {
      iVar10 = (*DAT_0065ba04)(0x50);
    }
    if ((iVar10 < DAT_00668134) || (DAT_00668134 < 0)) {
      if (DAT_0065b9f8 != 0) {
                    /* WARNING: Subroutine does not return */
        FUN_0058d065(1);
      }
      DAT_0065b9f8 = 1;
      FUN_0058b100(local_94,s_Invalid_monitor_selected_005d8d9c,0);
      FUN_004820b0(local_94);
      FUN_004820b0(s_Press_any_key_to_exit_005d7bdc);
      DAT_00667fd0 = 0;
      FUN_0049e410();
      (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x6c))();
      FUN_004a7460();
      FUN_00448c90();
      iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
      while (iVar10 != 0) {
        TranslateMessage((MSG *)local_20);
        DispatchMessageA((MSG *)local_20);
        iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
      }
      ShowWindow(DAT_0065b8ec,0);
      MessageBoxA((HWND)0x0,local_94,s_FATAL_ERROR_005d7bf4,0x10);
                    /* WARNING: Subroutine does not return */
      FUN_0058d065(1);
    }
    FUN_00480b30(0,0,&LAB_00485670,0);
    CoInitialize((LPVOID)0x0);
    iVar10 = FUN_00575890();
    if (iVar10 == 0) {
      FUN_00481c10(s_Unable_to_initialize_Network_005d8db8,0);
    }
    iVar10 = FUN_00485870();
    if (iVar10 != 0) {
      if (DAT_00668128 == 0) {
        if (DAT_00668184 == 0) {
          if (DAT_00668158 == 0) {
            iVar10 = FUN_00577c10();
            if (iVar10 == 0) {
              uVar8 = 0xffffffff;
              pcVar13 = &DAT_00665f34;
              do {
                pcVar11 = pcVar13;
                if (uVar8 == 0) break;
                uVar8 = uVar8 - 1;
                pcVar11 = pcVar13 + 1;
                cVar1 = *pcVar13;
                pcVar13 = pcVar11;
              } while (cVar1 != '\0');
              uVar8 = ~uVar8;
              pcVar13 = pcVar11 + -uVar8;
              pcVar11 = (char *)local_a4;
              for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
                *(undefined4 *)pcVar11 = *(undefined4 *)pcVar13;
                pcVar13 = pcVar13 + 4;
                pcVar11 = pcVar11 + 4;
              }
              for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
                *pcVar11 = *pcVar13;
                pcVar13 = pcVar13 + 1;
                pcVar11 = pcVar11 + 1;
              }
              uVar8 = 0xffffffff;
              pcVar13 = (char *)local_a4;
              do {
                if (uVar8 == 0) break;
                uVar8 = uVar8 - 1;
                cVar1 = *pcVar13;
                pcVar13 = pcVar13 + 1;
              } while (cVar1 != '\0');
              if (acStack_a6[~uVar8] != '\\') {
                iVar10 = -1;
                pcVar13 = (char *)local_a4;
                do {
                  pcVar11 = pcVar13;
                  if (iVar10 == 0) break;
                  iVar10 = iVar10 + -1;
                  pcVar11 = pcVar13 + 1;
                  cVar1 = *pcVar13;
                  pcVar13 = pcVar11;
                } while (cVar1 != '\0');
                *(undefined2 *)(pcVar11 + -1) = DAT_005d8dd8;
              }
              iVar10 = -1;
              pcVar13 = (char *)local_a4;
              do {
                pcVar11 = pcVar13;
                if (iVar10 == 0) break;
                iVar10 = iVar10 + -1;
                pcVar11 = pcVar13 + 1;
                cVar1 = *pcVar13;
                pcVar13 = pcVar11;
              } while (cVar1 != '\0');
              *(undefined4 *)(pcVar11 + -1) = DAT_005d8ddc;
              *(undefined4 *)(pcVar11 + 3) = DAT_005d8de0;
              *(undefined4 *)(pcVar11 + 7) = DAT_005d8de4;
              pcVar11[0xb] = DAT_005d8de8;
              FUN_0049a560();
              FUN_004bc470(local_a4);
              puVar7 = &DAT_0065d358;
            }
            else {
              puVar7 = &DAT_00659bd8;
            }
          }
          else {
            FUN_0047f4c0(2,0xffffffff,0xffffffff,0);
            puVar7 = &DAT_0065caf0;
          }
        }
        else if (DAT_0066603c == '\0') {
          FUN_0047f4c0(0,0xffffffff,0xffffffff,0);
          puVar7 = &DAT_0065caf0;
        }
        else {
          FUN_0047f4c0(1,0xffffffff,0xffffffff,&DAT_0066603c);
          puVar7 = &DAT_0065caf0;
        }
      }
      else {
        DAT_005d7a44 = 1;
        puVar7 = &DAT_00659bd8;
      }
      for (; puVar7 != (undefined *)0x0; puVar7 = (undefined *)FUN_004909d0(puVar7,0)) {
      }
      FUN_004863b0();
      CloseHandle(DAT_0065c138);
      CloseHandle(DAT_0065b8ac);
      FUN_00484ed0();
      if (DAT_006682d0 != 0) {
        p_Var15 = &local_70;
        for (iVar10 = 0x11; iVar10 != 0; iVar10 = iVar10 + -1) {
          p_Var15->cb = 0;
          p_Var15 = (_STARTUPINFOA *)&p_Var15->lpReserved;
        }
        local_70.dwFlags = 0;
        local_70.cb = 0x44;
        CreateProcessA(s_MPlayNow_exe_005d8dec,(LPSTR)0x0,(LPSECURITY_ATTRIBUTES)0x0,
                       (LPSECURITY_ATTRIBUTES)0x0,0,0,(LPVOID)0x0,(LPCSTR)0x0,&local_70,
                       (LPPROCESS_INFORMATION)(local_20 + 0xc));
      }
    }
    return 0;
  }
LAB_0048713e:
  if (DAT_0065b9f8 != 0) {
                    /* WARNING: Subroutine does not return */
    FUN_0058d065(1);
  }
  DAT_0065b9f8 = 1;
  FUN_0058b100(local_94,s_Compressed_files_found_in_resour_005d8cc0,0);
  FUN_004820b0(local_94);
  FUN_004820b0(s_Press_any_key_to_exit_005d7bdc);
  DAT_00667fd0 = 0;
  FUN_0049e410();
  (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x6c))();
  FUN_004a7460();
  FUN_00448c90();
  iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
  while (iVar10 != 0) {
    TranslateMessage((MSG *)local_20);
    DispatchMessageA((MSG *)local_20);
    iVar10 = PeekMessageA((LPMSG)local_20,(HWND)0x0,0,0,1);
  }
  ShowWindow(DAT_0065b8ec,0);
  MessageBoxA((HWND)0x0,local_94,s_FATAL_ERROR_005d7bf4,0x10);
                    /* WARNING: Subroutine does not return */
  FUN_0058d065(1);
}


