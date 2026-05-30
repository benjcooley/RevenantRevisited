// FUN_004be2b0_CompositeBuffer @ 004be2b0 size=3377

undefined4 __thiscall
FUN_004be2b0_CompositeBuffer(int *param_1,int param_2,int param_3,int param_4,int param_5,char *param_6,
            LONG *param_7,uint param_8,LONG *param_9,uint param_10,uint param_11)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  HDC pHVar5;
  int *piVar6;
  code *pcVar7;
  undefined4 uVar8;
  uint uVar9;
  UINT unaff_EBX;
  int unaff_EBP;
  char *pcVar10;
  HDC hdc;
  undefined4 *puVar11;
  undefined4 unaff_retaddr;
  LONG *pLStack_318;
  uint uStack_300;
  int iStack_2fc;
  LPCSTR pCStack_2f8;
  int iStack_2f4;
  char *pcStack_2f0;
  int *piStack_2ec;
  tagRECT tStack_2e8;
  HGDIOBJ pvStack_2d8;
  int *piStack_2d0;
  int iStack_2cc;
  uint uStack_2c8;
  undefined4 uStack_2c4;
  uint uStack_2c0;
  code *pcStack_2bc;
  undefined4 uStack_2b8;
  uint *puStack_2b4;
  undefined4 uStack_2b0;
  undefined4 uStack_2ac;
  undefined4 uStack_2a8;
  undefined4 uStack_2a4;
  undefined4 uStack_2a0;
  undefined4 uStack_29c;
  int iStack_298;
  int iStack_294;
  int iStack_290;
  int iStack_28c;
  undefined4 uStack_288;
  undefined4 uStack_284;
  int iStack_280;
  int iStack_27c;
  undefined4 uStack_278;
  undefined4 uStack_274;
  int *piStack_270;
  int iStack_26c;
  int iStack_268;
  int iStack_264;
  undefined4 uStack_260;
  undefined4 uStack_25c;
  undefined4 uStack_258;
  undefined4 uStack_254;
  undefined4 uStack_250;
  undefined4 uStack_24c;
  undefined4 uStack_248;
  undefined4 uStack_244;
  void *pvStack_240;
  undefined1 *puStack_23c;
  int iStack_238;
  undefined4 uStack_230;
  undefined4 uStack_22c;
  int iStack_228;
  undefined4 uStack_220;
  undefined4 uStack_21c;
  undefined2 uStack_218;
  undefined2 uStack_216;
  int *piStack_20c;
  uint uStack_208;
  int *piStack_204;
  tagPOINT atStack_200 [2];
  uint uStack_1f0;
  uint uStack_1ec;
  char *pcStack_1e8;
  int iStack_1e4;
  int iStack_1e0;
  int iStack_1dc;
  int iStack_1d8;
  LONG *pLStack_1d4;
  int iStack_1d0;
  int iStack_1cc;
  undefined4 uStack_1c8;
  undefined4 uStack_1c4;
  undefined4 uStack_1c0;
  undefined4 auStack_1b4 [7];
  int iStack_198;
  int iStack_194;
  int iStack_190;
  undefined4 uStack_17c;
  undefined4 uStack_178;
  undefined4 uStack_174;
  undefined1 auStack_15c [8];
  int aiStack_154 [80];
  undefined4 uStack_14;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e17c;
  local_c = ExceptionList;
  if ((param_6 == (char *)0x0) || (*param_6 == '\0')) {
    return 1;
  }
  hdc = *(HDC *)(DAT_0065b020 + param_8 * 4);
  if (hdc == (HDC)0x0) {
    hdc = DAT_0065b024;
  }
  pLStack_318 = (LONG *)0x4be30c;
  ExceptionList = &local_c;
  iVar4 = (**(code **)(*param_1 + 0x14))();
  if (iVar4 != 0) {
    pLStack_318 = (LONG *)0x0;
    FUN_00481c10(s_Can_t_draw_text_in_video_surface_005df09c);
  }
  if (param_1 == (int *)PTR_DAT_005d79e0) {
    pLStack_318 = (LONG *)0x4be32b;
    FUN_004aadd0();
  }
  pHVar5 = *(HDC *)(DAT_0065b020 + param_8 * 4);
  if (pHVar5 == (HDC)0x0) {
    pHVar5 = DAT_0065b024;
  }
  if (pHVar5[8].unused != 2) {
    iStack_27c = param_1[2];
    iStack_280 = param_1[1];
    uStack_2b8 = 0;
    uStack_288 = 0;
    uStack_284 = 0;
    piStack_270 = (int *)0x0;
    uStack_2ac = 0;
    uStack_2b0 = 0;
    uStack_29c = 0;
    uStack_2a0 = 0;
    uStack_2a4 = 0;
    uStack_2a8 = 0;
    uStack_274 = 0x1f;
    uStack_278 = 0;
    if ((param_11 & 0x80000000) == 0) {
      uStack_2c0 = param_11;
    }
    else {
      uStack_2c0 = hdc[0xf].unused | param_11 & 0x7fffffff;
    }
    iStack_298 = param_2;
    iStack_294 = param_3;
    puStack_2b4 = &uStack_1f0;
    iStack_290 = param_4;
    iStack_28c = param_5;
    pcStack_2bc = FUN_004b9ca0;
    tStack_2e8.top = hdc[0x14].unused;
    if ((param_10 & 0x80) == 0) {
      tStack_2e8.top = tStack_2e8.top + hdc[0x15].unused;
    }
    iStack_1e0 = hdc[0x17].unused;
    iStack_1d8 = hdc[0x16].unused;
    pcStack_1e8 = param_6;
    iStack_1d0 = param_5 / tStack_2e8.top;
    uStack_1ec = param_11 & 1;
    iStack_1cc = param_4;
    uStack_2c0 = uStack_2c0 | 1;
    uStack_1f0 = param_10;
    uStack_1c8 = 0;
    uStack_1c4 = 0;
    uStack_1c0 = 0;
    pLStack_1d4 = param_7;
    if (((((param_10 & 0x400) == 0) || (iStack_1e4 = hdc[0xb].unused, iStack_1e4 == 0)) &&
        (((param_10 & 0x800) == 0 || (iStack_1e4 = hdc[0xc].unused, iStack_1e4 == 0)))) &&
       (((param_10 & 0x1000) == 0 || (iStack_1e4 = hdc[0xd].unused, iStack_1e4 == 0)))) {
      iStack_1e4 = hdc[10].unused;
    }
    if (iStack_1e4 == 0) {
      iStack_1e4 = hdc[10].unused;
    }
    if (param_9 == (LONG *)0xffffffff) {
      if ((param_10 & 0x800) == 0) {
        if ((param_10 & 0x1000) == 0) {
          param_9 = (LONG *)hdc[0x10].unused;
        }
        else {
          param_9 = (LONG *)hdc[0x12].unused;
        }
      }
      else {
        param_9 = (LONG *)hdc[0x11].unused;
      }
    }
    piStack_2d0 = (int *)&pLStack_318;
    pLStack_318 = param_9;
    iStack_1dc = tStack_2e8.top;
    uStack_278 = FUN_004384e0();
    pLStack_318 = (LONG *)0x0;
    (**(code **)(*param_1 + 0x58))(&uStack_2c0);
    uVar8 = uStack_1c0;
    goto LAB_004befb0;
  }
  pCStack_2f8 = (LPCSTR)param_2;
  iStack_2f4 = param_3;
  iStack_26c = 0;
  iStack_2cc = 0;
  piStack_2ec = param_1;
  if (param_1[0x17] == 0) {
    pLStack_318 = (LONG *)0x4be385;
    iVar4 = (**(code **)(*param_1 + 0xc))();
    if ((iVar4 == 0) || (((param_1[0xe] ^ *(uint *)(PTR_DAT_005d79e0 + 0x38)) & 0x3001f) != 0))
    goto LAB_004be3a3;
  }
  else {
LAB_004be3a3:
    piVar6 = (int *)param_1[0x17];
    if ((piVar6 == (int *)0x0) || ((piVar6[1] < param_4 || (piVar6[2] < param_5)))) {
      pLStack_318 = (LONG *)0x78;
      piVar6 = (int *)FUN_00482fb0();
      uStack_4 = 0;
      piStack_2d0 = piVar6;
      if (piVar6 == (int *)0x0) {
        piStack_2ec = (int *)0x0;
      }
      else {
        pLStack_318 = (LONG *)0x4be420;
        FUN_004bcb00();
        pLStack_318 = (LONG *)0x0;
        uStack_4 = CONCAT31(uStack_4._1_3_,1);
        *piVar6 = (int)&PTR_FUN_005a3980;
        piVar6[0x1a] = 0;
        FUN_004a5740(param_4,param_5,0x400,piVar6[4]);
        piVar6[0x1c] = 1;
        piStack_2ec = piVar6;
      }
      pLStack_318 = (LONG *)0x80000000;
      uStack_4 = 0xffffffff;
      (**(code **)(*piStack_2ec + 100))
                (0,0,piStack_2ec[1],piStack_2ec[2],piStack_2ec[0xc],0xffff,0x7f7f);
      iStack_2cc = 1;
      iStack_2f4 = 0;
      pCStack_2f8 = (LPCSTR)0x0;
    }
    else {
      pLStack_318 = (LONG *)0x80000000;
      piStack_2ec = piVar6;
      (**(code **)(*piVar6 + 100))(0,0,param_4,param_5,piVar6[0xc],0xffff,0x7f7f);
      iStack_26c = 1;
      iStack_2f4 = 0;
      pCStack_2f8 = (LPCSTR)0x0;
    }
  }
  uStack_2c0 = param_11;
  pLStack_318 = (LONG *)0x0;
  iStack_294 = iStack_2f4;
  pcStack_2bc = (code *)0x0;
  uStack_2b8 = 0;
  puStack_2b4 = (uint *)0x0;
  iStack_298 = (int)pCStack_2f8;
  iStack_290 = param_4;
  iStack_28c = param_5;
  uStack_288 = 0;
  uStack_284 = 0;
  iStack_280 = param_4;
  iStack_27c = param_5;
  piStack_270 = (int *)0x0;
  uStack_2ac = 0;
  uStack_2b0 = 0;
  uStack_29c = 0;
  uStack_2a0 = 0;
  uStack_2a4 = 0;
  uStack_2a8 = 0;
  uStack_274 = 0x1f;
  uStack_278 = 0;
  iVar4 = FUN_004bd200(&uStack_2c0,0,0);
  if (iVar4 == 0) {
    ExceptionList = local_c;
    return 0;
  }
  pLStack_318 = &tStack_2e8.top;
  puVar11 = auStack_1b4;
  for (iVar4 = 0x16; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar11 = 0;
    puVar11 = puVar11 + 1;
  }
  iStack_198 = piStack_2ec[1];
  iStack_194 = piStack_2ec[2];
  iStack_190 = piStack_2ec[4];
  uStack_174 = 0;
  uStack_178 = 0;
  uStack_17c = 0;
  iVar4 = FUN_004ad7c0(auStack_1b4,&uStack_2c0,auStack_15c);
  if (iVar4 == 0) {
    ExceptionList = local_c;
    return 0;
  }
  uStack_300 = 0x2810;
  if ((param_10 & 1) == 0) {
    if ((param_10 & 4) == 0) {
      if ((param_10 & 2) != 0) {
        uStack_300 = 0x2811;
      }
    }
    else {
      uStack_300 = 0x2812;
    }
  }
  if ((param_10 & 0x10) == 0) {
    if ((param_10 & 0x20) == 0) {
      if ((param_10 & 0x40) != 0) {
        uStack_300 = uStack_300 & 0xffffdfef | 0x24;
      }
    }
    else {
      uStack_300 = uStack_300 | 0x28;
    }
  }
  else {
    uStack_300 = uStack_300 | 0x20;
  }
  uVar9 = param_10 & 0x80;
  if (uVar9 != 0) {
    uStack_300 = uStack_300 & 0xffffdfef | 0x20;
  }
  if ((param_10 & 0x100) != 0) {
    uStack_300 = uStack_300 | 0x400;
  }
  iStack_264 = 0;
  if ((uVar9 != 0) && ((param_10 & 0x270) == 0)) {
    iStack_264 = hdc[0x17].unused;
  }
  pcStack_2f0 = param_6;
  if (uVar9 == 0) {
    pLStack_318 = param_7;
    iVar4 = FUN_004aceb0(param_8,param_6,param_4,param_10);
    pcStack_2f0 = param_6 + iVar4;
  }
  uVar9 = 0xffffffff;
  pcVar10 = pcStack_2f0;
  do {
    if (uVar9 == 0) break;
    uVar9 = uVar9 - 1;
    cVar1 = *pcVar10;
    pcVar10 = pcVar10 + 1;
  } while (cVar1 != '\0');
  pCStack_2f8 = (LPCSTR)(~uVar9 - 1);
  pLStack_318 = (LONG *)0x4be6f1;
  piVar6 = (int *)(**(code **)(*piStack_2ec + 0xc))();
  if (piVar6 == (int *)0x0) {
    ExceptionList = local_c;
    return 0;
  }
  tStack_2e8.left = 1;
  iVar4 = hdc[0x14].unused;
  if (0xe < iVar4) {
    tStack_2e8.left = 2;
  }
  uStack_2c4 = 1;
  if (0x19 < iVar4) {
    tStack_2e8.left = 2;
    uStack_2c4 = 2;
  }
  uStack_2c8 = (uint)(0x19 >= iVar4);
  pLStack_318 = &iStack_2fc;
  piStack_204 = piVar6;
  iVar4 = (**(code **)(*piVar6 + 0x44))(piVar6);
  if (iVar4 != 0) {
    FUN_004a90d0(iVar4,s_d__revenant_Surface_cpp_005df0d8,0x38f);
  }
  pvStack_2d8 = SelectObject(hdc,*(HGDIOBJ *)(unaff_EBP + 0x38));
  iStack_268 = 0;
  if (0 < (int)piStack_2ec) {
    piStack_204 = (int *)(param_8 & 0x400);
    uStack_208 = (((uint)param_7 & 0xff) << 8 | (uint)param_7 >> 8 & 0xff) << 8 |
                 (uint)param_7 >> 0x10 & 0xff;
    piStack_270 = aiStack_154;
    do {
      iVar3 = iStack_298;
      iStack_2fc = iStack_294;
      iVar4 = piStack_270[1];
      pcVar10 = (char *)(piStack_270[6] + *piStack_270);
      iVar2 = piStack_270[7];
      SetTextColor(hdc,0);
      SetBkMode(hdc,1);
      if (piStack_204 != (int *)0x0) {
        tStack_2e8.left = (LONG)(pcVar10 + (int)pcStack_2f0);
        tStack_2e8.top = iStack_2cc + ((iVar2 + iVar4) - iStack_26c);
        tStack_2e8.right = tStack_2e8.left + iVar3;
        tStack_2e8.bottom = tStack_2e8.top + iStack_2fc;
        iStack_264 = tStack_2e8.top;
        DrawTextA(hdc,pCStack_2f8,uStack_300,&tStack_2e8,unaff_EBX);
        if (piStack_2d0 != (int *)0x0) {
          tStack_2e8.left = (LONG)(pcVar10 + 1 + (int)pcStack_2f0);
          tStack_2e8.top = iStack_264;
          tStack_2e8.right = tStack_2e8.left + iVar3;
          tStack_2e8.bottom = iStack_264 + iStack_2fc;
          DrawTextA(hdc,pCStack_2f8,uStack_300,&tStack_2e8,unaff_EBX);
          tStack_2e8.left = (LONG)(pcStack_2f0 + (int)pcVar10);
          tStack_2e8.right = tStack_2e8.left + iVar3;
          tStack_2e8.top = iStack_264 + 1;
          tStack_2e8.bottom = tStack_2e8.top + iStack_2fc;
          DrawTextA(hdc,pCStack_2f8,uStack_300,&tStack_2e8,unaff_EBX);
        }
      }
      SetTextColor(hdc,uStack_208);
      tStack_2e8.top = (iVar2 + iVar4) - iStack_26c;
      tStack_2e8.right = (LONG)(pcVar10 + iVar3);
      tStack_2e8.bottom = tStack_2e8.top + iStack_2fc;
      tStack_2e8.left = (LONG)pcVar10;
      DrawTextA(hdc,pCStack_2f8,uStack_300,&tStack_2e8,unaff_EBX);
      param_1[0xf] = tStack_2e8.left;
      param_1[0x10] = tStack_2e8.top;
      param_1[0x11] = (int)(tStack_2e8.right + -1);
      param_1[0x12] = tStack_2e8.bottom + -1;
      GetCurrentPositionEx(hdc,atStack_200);
      param_1[0x13] = atStack_200[0].x;
      param_1[0x14] = atStack_200[0].y;
      iStack_268 = iStack_268 + 1;
      piStack_270 = piStack_270 + 0x15;
      param_4 = param_2;
      piVar6 = piStack_20c;
    } while (iStack_268 < (int)piStack_2ec);
  }
  SelectObject(hdc,pvStack_2d8);
  iVar4 = (**(code **)(*piVar6 + 0x68))(piVar6,hdc);
  if (iVar4 != 0) {
    FUN_004a90d0(iVar4,s_d__revenant_Surface_cpp_005df0f0,0x3cc);
  }
  if (iStack_27c == 0) {
    if (tStack_2e8.bottom != 0) {
      pcVar10 = (char *)FUN_00482fb0(0x78);
      uStack_14 = 2;
      tStack_2e8.right = (LONG)pcVar10;
      if (pcVar10 == (char *)0x0) {
        pcVar10 = (char *)0x0;
      }
      else {
        FUN_004bcb00();
        uStack_14 = CONCAT31(uStack_14._1_3_,3);
        *(undefined ***)pcVar10 = &PTR_FUN_005a3980;
        pcVar10[0x68] = '\0';
        pcVar10[0x69] = '\0';
        pcVar10[0x6a] = '\0';
        pcVar10[0x6b] = '\0';
        FUN_004a5740(param_4,unaff_retaddr,0x400,*(undefined4 *)(pcVar10 + 0x10),0);
        pcVar10[0x70] = '\x01';
        pcVar10[0x71] = '\0';
        pcVar10[0x72] = '\0';
        pcVar10[0x73] = '\0';
      }
      pvStack_240 = local_c;
      uStack_14 = 0xffffffff;
      iStack_268 = 0x100;
      iStack_264 = 0;
      uStack_260 = 0;
      uStack_25c = 0;
      puStack_23c = puStack_8;
      uStack_230 = 0;
      uStack_22c = 0;
      uStack_216 = 0;
      uStack_218 = 0;
      uStack_254 = 0;
      uStack_258 = 0;
      uStack_244 = 0;
      uStack_248 = 0;
      uStack_24c = 0;
      uStack_250 = 0;
      uStack_21c = 0x1f;
      uStack_220 = 0;
      iStack_238 = param_4;
      iStack_228 = param_4;
      (**(code **)(*param_1 + 0x5c))(&iStack_268,pcVar10,0,0);
      if (pcVar10 != (char *)0x0) {
        (*(code *)**(undefined4 **)pcVar10)(1);
      }
    }
  }
  else {
    pvStack_240 = local_c;
    puStack_23c = puStack_8;
    iStack_268 = 0x100;
    iStack_264 = 0;
    uStack_260 = 0;
    uStack_25c = 0;
    uStack_230 = 0;
    uStack_22c = 0;
    uStack_216 = 0;
    uStack_218 = 0;
    uStack_254 = 0;
    uStack_258 = 0;
    uStack_244 = 0;
    uStack_248 = 0;
    uStack_24c = 0;
    uStack_250 = 0;
    uStack_21c = 0x1f;
    uStack_220 = 0;
    iStack_238 = param_4;
    iStack_228 = param_4;
    (**(code **)(*param_1 + 0x5c))(&iStack_268,param_1[0x17],0,0);
  }
  pcVar7 = (code *)(**(code **)(*param_1 + 0x4c))();
  if (pcVar7 != (code *)0x0) {
    if ((iStack_26c != 0) || (iStack_2cc != 0)) {
      uStack_2c0 = param_11;
      pLStack_318 = (LONG *)0x0;
      iStack_294 = param_3;
      pcStack_2bc = (code *)0x0;
      uStack_2b8 = 0;
      puStack_2b4 = (uint *)0x0;
      iStack_298 = param_2;
      iStack_28c = param_5;
      uStack_288 = 0;
      uStack_284 = 0;
      iStack_27c = param_5;
      piStack_270 = (int *)0x0;
      uStack_2ac = 0;
      uStack_2b0 = 0;
      uStack_29c = 0;
      uStack_2a0 = 0;
      uStack_2a4 = 0;
      uStack_2a8 = 0;
      uStack_274 = 0x1f;
      uStack_278 = 0;
      iStack_290 = param_4;
      iStack_280 = param_4;
      FUN_004bd200(&uStack_2c0,0,0);
    }
    pLStack_318 = (LONG *)&uStack_2c0;
    (*pcVar7)(auStack_1b4);
  }
  pLStack_318 = (LONG *)0x0;
  uVar8 = FUN_004acb80(param_8,pcStack_2f0,param_4,param_10,param_7,10000);
LAB_004befb0:
  if (param_1 == (int *)PTR_DAT_005d79e0) {
    pLStack_318 = (LONG *)0x4befbd;
    FUN_004aade0();
  }
  ExceptionList = local_c;
  return uVar8;
}


