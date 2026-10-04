// FUN_004911b0 @ 004911b0 size=1720

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_004911b0(int param_1)

{
  int *piVar1;
  int iVar2;
  DWORD DVar3;
  undefined *puVar4;
  undefined4 uVar5;
  int iVar6;
  uint uVar7;
  char *pcVar8;
  undefined4 extraout_ECX;
  char *pcVar9;
  undefined *puVar10;
  undefined *puVar11;
  undefined *puVar12;
  char *pcVar13;
  bool bVar14;
  undefined1 *puVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  float local_17c;
  uint local_178;
  undefined *local_174;
  float local_170;
  uint uStack_16c;
  uint uStack_168;
  char *pcStack_164;
  char *pcStack_160;
  char *pcStack_15c;
  char *pcStack_158;
  undefined *puStack_154;
  undefined *puStack_150;
  char *pcStack_14c;
  float local_148;
  char *pcStack_144;
  char *pcStack_140;
  char *pcStack_13c;
  undefined *puStack_138;
  char *pcStack_134;
  tagMSG tStack_11c;
  undefined1 auStack_100 [256];
  
  uStack_168 = 0;
  DAT_00668508 = 0;
  local_170 = 24.0;
  local_148 = 24.0;
  local_17c = 0.0;
  local_178 = 0;
  uStack_16c = 0;
  bVar14 = false;
  do {
    piVar1 = DAT_00667fd0;
    if ((DAT_00667fd0[0x15] != 0) &&
       (((((int *)DAT_00667fd0[0x10] == (int *)0x0 ||
          (iVar2 = (**(code **)(*(int *)DAT_00667fd0[0x10] + 0x18))(), iVar2 == 0)) &&
         (((int *)piVar1[0x11] == (int *)0x0 ||
          (iVar2 = (**(code **)(*(int *)piVar1[0x11] + 0x18))(), iVar2 == 0)))) ||
        (((int *)DAT_00667fd0[0x11] != (int *)0x0 &&
         (iVar2 = (**(code **)(*(int *)DAT_00667fd0[0x11] + 0x24))(), iVar2 != 0)))))) {
      if (DAT_006682bc != 0) {
        return 0;
      }
      FUN_004a9ee0(1);
      return 0;
    }
    uVar5 = DAT_006682d8;
    if (DAT_006682a8 == 0) {
      FUN_0049e450();
      uVar5 = DAT_006682d8;
    }
    while ((((DAT_006682d8 = uVar5, DAT_005d7a20 != 0 && (DAT_006582c8 == 0)) && (DAT_006682b8 == 0)
            ) && (DAT_006682bc == 0))) {
      PeekMessageA(&tStack_11c,(HWND)0x0,0,0,1);
      if (tStack_11c.message == 0x12) goto LAB_0049183a;
      if (tStack_11c.message == 0x105) {
        if ((DAT_00668190 != 0) || (tStack_11c.wParam == 0x73)) {
          TranslateMessage(&tStack_11c);
          goto LAB_004912ce;
        }
        uVar5 = DAT_006682d8;
        if (tStack_11c.wParam == 9) {
          DAT_00668508 = 1;
        }
      }
      else {
        TranslateMessage(&tStack_11c);
LAB_004912ce:
        DispatchMessageA(&tStack_11c);
        uVar5 = DAT_006682d8;
      }
    }
    if (DAT_00667fd0 != (int *)0x0) {
      if (((DAT_00668508 != 0) && (DAT_0066850c == 0)) && (DAT_006682bc == 0)) {
        DAT_006682d8 = 0;
        DAT_0066850c = 1;
        iVar2 = FUN_0053c060(s_EXITGAMEYN_005d9ed0,3);
        if (iVar2 != 0) {
LAB_0049183a:
          PostQuitMessage(0);
          DAT_006682b8 = 1;
          return 0;
        }
        DAT_0066850c = 0;
      }
      DAT_006682d8 = uVar5;
      DVar3 = GetTickCount();
      DAT_00667fc0 = DVar3 - _DAT_00668500;
      _DAT_00668500 = GetTickCount();
      uStack_16c = uStack_16c + DAT_00667fc0;
      uStack_168 = uStack_168 + 1;
      if (bVar14) {
        local_178 = local_178 + 1;
      }
      if (DAT_006680b4 == 0) {
        if (4 < uStack_168) {
          uVar7 = uStack_168 - local_178;
          local_148 = _DAT_005a4958 / (float)(uStack_16c / uStack_168);
          if ((int)uVar7 < 1) {
            uVar7 = 1;
          }
          local_170 = _DAT_005a4958 / (float)(uStack_16c / uVar7);
          if (uStack_168 <= local_178) {
            DAT_006680b4 = 1;
          }
          uStack_16c = 0;
          uStack_168 = 0;
          local_178 = 0;
        }
      }
      else {
        local_178 = 0;
        uStack_16c = 0;
        uStack_168 = 0;
      }
      local_17c = local_17c + local_170;
      if ((((float)_DAT_005a5fe0 < local_17c) || (DAT_00667fd0[0x13] != 0)) ||
         ((DAT_006680b4 != 0 || ((DAT_006680b0 != 0 || (DAT_0065c8a0 != 0)))))) {
        if ((DAT_006682bc == 0) && ((DAT_006682ac != 0 || (DAT_006682b0 != 0)))) {
          (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x24))();
          (**(code **)(*(int *)PTR_DAT_005d79e0 + 100))
                    (0,0,*(undefined4 *)(PTR_DAT_005d79e0 + 4),0xc,0,0,0,0x20);
          puVar11 = &DAT_005d9edc;
          if (DAT_00668114 == 0) {
            puVar11 = &DAT_0066851c;
          }
          local_174 = &DAT_005d9ee4;
          if (DAT_00668110 == 0) {
            local_174 = &DAT_00668520;
          }
          pcStack_14c = &DAT_00668524;
          if (DAT_005d7a14 == 0) {
            pcStack_14c = s_NInt_005d9eec;
          }
          pcStack_15c = s_NAni_005d9ef4;
          if (DAT_006680d8 == 0) {
            pcStack_15c = &DAT_00668528;
          }
          pcStack_134 = s_NPls_005d9efc;
          if (DAT_006680d4 == 0) {
            pcStack_134 = &DAT_0066852c;
          }
          puStack_154 = &DAT_005d9f04;
          if (DAT_005d7a10 != 0) {
            puStack_154 = &DAT_00668530;
          }
          puStack_138 = &DAT_005d9f0c;
          if (DAT_005e91bc == 0) {
            puStack_138 = &DAT_00668534;
          }
          pcStack_164 = s_NFlt_005d9f14;
          if (DAT_005c61b0 != 0) {
            pcStack_164 = &DAT_00668538;
          }
          pcStack_140 = s_NTex_005d9f1c;
          if (DAT_005c619c != 0) {
            pcStack_140 = &DAT_0066853c;
          }
          pcStack_144 = s_NSpc_005d9f24;
          if (DAT_005c61a8 != 0) {
            pcStack_144 = &DAT_00668540;
          }
          pcStack_13c = s_NZbf_005d9f2c;
          if (DAT_005c61ac != 0) {
            pcStack_13c = &DAT_00668544;
          }
          pcStack_160 = s_NBln_005d9f34;
          if (DAT_005c61a4 != 0) {
            pcStack_160 = &DAT_00668548;
          }
          pcStack_158 = s_NDth_005d9f3c;
          if (DAT_005c61a0 != 0) {
            pcStack_158 = &DAT_0066854c;
          }
          puStack_150 = &DAT_005d9f44;
          if (DAT_005e91b0 == 0) {
            puStack_150 = &DAT_00668550;
          }
          puVar10 = &DAT_005d9f4c;
          if (DAT_005d79f4 == 0) {
            puVar10 = &DAT_00668554;
          }
          pcVar13 = s_NNrm_005d9f54;
          if (DAT_005d7a00 == 0) {
            pcVar13 = &DAT_00668558;
          }
          puVar12 = &DAT_005d9f5c;
          if (DAT_006680b8 == 0) {
            puVar12 = &DAT_0066855c;
          }
          pcVar9 = s_NSZb_005d9f64;
          if (DAT_005d79f8 == 0) {
            pcVar9 = &DAT_00668560;
          }
          pcVar8 = s_NSkp_005d9f6c;
          if (DAT_006680b0 == 0) {
            pcVar8 = &DAT_00668564;
          }
          puVar4 = &DAT_005d9f74;
          if (DAT_005d7a04 == 0) {
            puVar4 = &DAT_00668568;
          }
          uVar5 = FUN_004a8170(DAT_005e8300,puVar4,pcVar8,pcVar9,puVar12,pcVar13,puVar10,puStack_150
                               ,pcStack_158,pcStack_160,pcStack_13c,pcStack_144,pcStack_140,
                               pcStack_164,puStack_138,puStack_154,pcStack_134,pcStack_15c,
                               pcStack_14c,local_174,puVar11);
          FUN_004811b0(auStack_100,0x100,s_Frames___4_1f__4_1f_Mem__d_VidMe_005d9f7c,
                       (double)local_148,(double)local_170,DAT_0065ba08,uVar5);
          uVar18 = 0x80000000;
          uVar17 = 0;
          uVar5 = FUN_00429950(0xff,0xff,0);
          uVar16 = extraout_ECX;
          FUN_00419dd0(uVar5);
          puVar15 = auStack_100;
          uVar5 = DAT_0065abc4;
          iVar2 = FUN_0047f550(puVar15,DAT_0065abc4,uVar16,uVar17,uVar18);
          iVar2 = iVar2 + 9;
          iVar6 = FUN_0047f540(iVar2);
          FUN_00438ed0(iVar6 + 5,iVar2,puVar15,uVar5,uVar16,uVar17,uVar18);
        }
        DAT_006680b4 = 0;
        FUN_0049e480();
        if (local_17c <= _DAT_005a3510) {
          local_17c = local_17c - _DAT_005a5fdc;
        }
        else {
          local_17c = 0.0;
        }
        if ((DAT_00667fd0[0x13] == 0) && (DAT_006682bc == 0)) {
          FUN_004a9ee0(1);
        }
        piVar1 = DAT_00667fd0;
        bVar14 = DAT_006682bc == 0;
        DAT_00667fd0[0x13] = 0;
        iVar2 = (**(code **)(*piVar1 + 0x44))(bVar14);
        if (iVar2 == 0) {
          return 0;
        }
        if ((DAT_0065c8a0 != 0) && (DAT_006682bc == 0)) {
          FUN_0049e790();
        }
        bVar14 = false;
        DAT_00667fd0[0x12] = DAT_00667fd0[0x12] + 1;
      }
      else {
        iVar2 = (**(code **)(*DAT_00667fd0 + 0x44))(0);
        if (iVar2 == 0) {
          return 0;
        }
        bVar14 = true;
        DAT_00667fd0[0x12] = DAT_00667fd0[0x12] + 1;
      }
    }
    if ((param_1 != 0) && (param_1 = param_1 + -1, param_1 == 0)) {
      return 1;
    }
  } while( true );
}


