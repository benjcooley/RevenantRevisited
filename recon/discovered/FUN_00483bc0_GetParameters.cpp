// FUN_00483bc0 @ 00483bc0 size=2102

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00483bc0(undefined4 *param_1,int param_2)

{
  char *pcVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  char *pcVar5;
  
  iVar3 = FUN_0058ad30(param_1,s_LASTOPTIONS_005d7eb0);
  while ((iVar3 != 0 && (param_2 == 0))) {
    param_1 = &DAT_0066819c;
    iVar3 = FUN_0058ad30(&DAT_0066819c,s_LASTOPTIONS_005d7eb0);
    param_2 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_MPBOT_005d7ebc);
  if (iVar3 != 0) {
    DAT_00668128 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_GAMESPEED__005d7ec4);
  if (iVar3 == 0) {
LAB_00483c60:
    if (DAT_005d79e4 < 2) {
LAB_00483c64:
      DAT_005d7a04 = 0;
      DAT_005c61b4 = 1;
    }
    if (DAT_005d79e4 < 3) {
      DAT_005d7a18 = 0;
      DAT_005c61b8 = 0;
    }
  }
  else {
    DAT_005d79e4 = FUN_0058b42c(iVar3 + 10);
    if (DAT_005d79e4 < 1) {
      DAT_005d79e4 = 1;
      goto LAB_00483c64;
    }
    if (DAT_005d79e4 < 6) goto LAB_00483c60;
    DAT_005d79e4 = 5;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOSOUND_005d7ed0);
  if (iVar3 != 0) {
    DAT_00668114 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOEAX_005d7ed8);
  if (iVar3 == 0) {
    iVar3 = FUN_0058ad30(param_1,s_USEEAX_005d7ee0);
    if (iVar3 != 0) {
      DAT_00668118 = 0;
    }
  }
  else {
    DAT_00668118 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_DEBUG_005d7ee8);
  if (iVar3 != 0) {
    DAT_0066814c = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_FORCE15BIT_005d7ef0);
  if (iVar3 != 0) {
    DAT_00668160 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_FORCE16BIT_005d7efc);
  if (iVar3 != 0) {
    DAT_00668164 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NONWINDOWED_005d7f08);
  if (iVar3 == 0) {
    iVar3 = FUN_0058ad30(param_1,s_WINDOWED_005d7f14);
    if (iVar3 != 0) {
      DAT_00668190 = 1;
    }
  }
  else {
    DAT_00668190 = 0;
  }
  iVar3 = FUN_0058ad30(param_1,s_EDITOR_005d7f20);
  if (iVar3 != 0) {
    DAT_00668158 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOQUICKLOAD_005d7f28);
  if (iVar3 == 0) {
    iVar3 = FUN_0058ad30(param_1,s_QUICKLOAD_005d7f34);
    if (iVar3 != 0) {
      DAT_0066815c = 0;
    }
  }
  else {
    DAT_0066815c = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOHANDLEEXCEPTIONS_005d7f40);
  if (iVar3 == 0) {
    iVar3 = FUN_0058ad30(param_1,s_HANDLEEXCEPTIONS_005d7f54);
    if (iVar3 != 0) {
      _DAT_005d7a5c = 1;
    }
  }
  else {
    _DAT_005d7a5c = 0;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOREPORTEXCEPTIONS_005d7f68);
  if (iVar3 == 0) {
    iVar3 = FUN_0058ad30(param_1,s_REPORTEXCEPTIONS_005d7f7c);
    if (iVar3 != 0) {
      _DAT_00668180 = 1;
    }
  }
  else {
    _DAT_00668180 = 0;
  }
  iVar3 = FUN_0058ad30(param_1,s_MAKERESOURCEDIR_005d7f90);
  if (iVar3 != 0) {
    _DAT_00668174 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_SLIDINGPANES_005d7fa0);
  if (iVar3 != 0) {
    _DAT_005d7a58 = 0;
  }
  iVar3 = FUN_0058ad30(param_1,s_TEXTDUMP_005d7fb0);
  if (iVar3 != 0) {
    DAT_00668178 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_BORDERLESS_005d7fbc);
  if (iVar3 != 0) {
    DAT_00668148 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOWIDE_005d7fc8);
  if (iVar3 != 0) {
    DAT_006680c4 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_FULLSCREEN_005d7fd0);
  if (iVar3 != 0) {
    DAT_00668168 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOPRELOADSECTORS_005d7fdc);
  if (iVar3 != 0) {
    DAT_005d7a30 = 0;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOBOOTPLAYERS_005d7ff0);
  if (iVar3 != 0) {
    DAT_00668124 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_MONITOR__005d8000);
  if (iVar3 != 0) {
    DAT_00668134 = FUN_0058b42c(iVar3 + 8);
  }
  iVar3 = FUN_0058ad30(param_1,s_VIOLENCELEVEL__005d800c);
  if (iVar3 != 0) {
    DAT_005d79e8 = FUN_0058b42c(iVar3 + 0xe);
    if (DAT_005d79e8 < 0) {
      DAT_005d79e8 = 0;
    }
    else if (5 < DAT_005d79e8) {
      DAT_005d79e8 = 5;
    }
  }
  iVar3 = FUN_0058ad30(param_1,s_PRELOADSIZE__005d801c);
  if (iVar3 != 0) {
    DAT_005d79ec = FUN_0058b42c(iVar3 + 0xc);
    if (DAT_005d79ec < 6) {
      if (DAT_005d79ec < 3) {
        DAT_005d7a30 = 0;
        DAT_005d79ec = 3;
      }
    }
    else {
      DAT_005d79ec = 5;
    }
  }
  iVar3 = FUN_0058ad30(param_1,s_CHUNKCACHESIZE__005d802c);
  if ((iVar3 != 0) && (DAT_005d79f0 = FUN_0058b42c(iVar3 + 0xf), 0x100 < DAT_005d79f0)) {
    DAT_005d79f0 = 0x100;
  }
  iVar3 = FUN_0058ad30(param_1,s_DRIVER__005d803c);
  if ((iVar3 != 0) || (iVar3 = FUN_0058ad30(param_1,s_DEVICE__005d8044), iVar3 != 0)) {
    iVar4 = iVar3 + 7;
    pcVar5 = &DAT_0065b0b8;
    cVar2 = *(char *)(iVar3 + 7);
    while ((cVar2 != '\0' && (cVar2 != ' '))) {
      *pcVar5 = cVar2;
      pcVar1 = (char *)(iVar4 + 1);
      pcVar5 = pcVar5 + 1;
      iVar4 = iVar4 + 1;
      cVar2 = *pcVar1;
    }
    *pcVar5 = '\0';
    FUN_0059bd3e(&DAT_0065b0b8);
  }
  iVar3 = FUN_0058ad30(param_1,s_IGNORE3D_005d804c);
  if (iVar3 != 0) {
    DAT_00668150 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOBLITTEXTURES_005d8058);
  if (iVar3 != 0) {
    DAT_006680cc = 1;
    DAT_006680c8 = 1;
    DAT_005db0b8 = 1;
    DAT_00669adc = 0;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOTEXOVERLAYS_005d8068);
  if (iVar3 != 0) {
    DAT_006680c8 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOHITTEST_005d8078);
  if (iVar3 != 0) {
    DAT_006680e4 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOZRESTORE_005d8084);
  if (iVar3 != 0) {
    DAT_006680e8 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOSOFTWARE3D_005d8090);
  if (iVar3 == 0) {
    iVar3 = FUN_0058ad30(param_1,s_SOFTWARE3D_005d80a0);
    if (iVar3 != 0) {
      DAT_0066818c = 1;
    }
  }
  else {
    DAT_0066818c = 0;
  }
  iVar3 = FUN_0058ad30(param_1,s_RAMPMODE_005d80ac);
  if (iVar3 != 0) {
    _DAT_005d7a24 = 1;
    _DAT_006680d0 = 0;
    DAT_0066818c = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_RGBMODE_005d80b8);
  if (iVar3 != 0) {
    _DAT_005d7a24 = 0;
    _DAT_006680d0 = 1;
    DAT_0066818c = 1;
  }
  iVar3 = FUN_0058ad30(param_1,&DAT_005d80c0);
  if (iVar3 != 0) {
    DAT_005d7a28 = 1;
    _DAT_005d7a24 = 0;
    _DAT_006680d0 = 1;
    DAT_0066818c = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOCACHEEXBUFS_005d80c8);
  if (iVar3 != 0) {
    _DAT_005d7a2c = 0;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOVIDZLOCK_005d80d8);
  if (iVar3 != 0) {
    DAT_006680dc = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_CLEARZ_005d80e4);
  if (iVar3 != 0) {
    DAT_006680ec = 1;
    DAT_006680dc = 0;
  }
  iVar3 = FUN_0058ad30(param_1,s_VIDMEMZBUF_005d80ec);
  if (iVar3 != 0) {
    DAT_006680f0 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_VOODOO_005d80f8);
  if (iVar3 != 0) {
    DAT_006680f4 = 1;
    DAT_006680dc = 1;
    DAT_006680e0 = 1;
    DAT_006680ec = 1;
    DAT_005d7a34 = 1;
  }
  iVar3 = FUN_0058ad30(param_1,s_NOMMX_005d8100);
  if (iVar3 != 0) {
    DAT_006680f8 = 0;
  }
  iVar3 = FUN_0058ad30(param_1,s_QUICKSTART_005d8108);
  if (iVar3 != 0) {
    DAT_00668184 = 1;
    DAT_0066603c = 0;
    iVar3 = FUN_0058ade0(iVar3 + 0xb,0x22);
    if (iVar3 != 0) {
      pcVar5 = (char *)(iVar3 + 1);
      iVar3 = FUN_0058ade0(pcVar5,0x22);
      if (iVar3 != 0) {
        iVar4 = (iVar3 - (int)pcVar5) + 1;
        iVar3 = 0x104;
        if (iVar4 < 0x105) {
          iVar3 = iVar4;
        }
        _strncpy(&DAT_0066603c,pcVar5,iVar3 - 1);
        *(undefined1 *)((int)&DAT_00666038 + iVar3 + 3) = 0;
      }
    }
  }
  iVar3 = FUN_0058ad30(param_1,s_MODULE_005d8114);
  if ((iVar3 != 0) && (iVar3 = FUN_0058ade0(iVar3 + 7,0x22), iVar3 != 0)) {
    pcVar5 = (char *)(iVar3 + 1);
    iVar3 = FUN_0058ade0(pcVar5,0x22);
    if (iVar3 != 0) {
      iVar4 = (iVar3 - (int)pcVar5) + 1;
      iVar3 = 0x80;
      if (iVar4 < 0x81) {
        iVar3 = iVar4;
      }
      _strncpy(&DAT_0065c958,pcVar5,iVar3 - 1);
      *(undefined1 *)((int)&DAT_0065c954 + iVar3 + 3) = 0;
    }
  }
  iVar3 = FUN_0058ad30(param_1,s_VIDEOCAP__005d811c);
  if (iVar3 == 0) {
    DAT_006682a0 = 0;
  }
  else {
    DAT_006682a0 = 1;
    DAT_00666038 = FUN_0058b42c(iVar3 + 9);
    if (DAT_00666038 < 4) {
      DAT_00666038 = 4;
    }
    else if (0x80 < DAT_00666038) {
      DAT_00666038 = 0x80;
    }
    if (*(char *)(iVar3 + 10) == ',') {
      iVar3 = iVar3 + 0xb;
    }
    else if (*(char *)(iVar3 + 0xb) == ',') {
      iVar3 = iVar3 + 0xc;
    }
    else if (*(char *)(iVar3 + 0xc) == ',') {
      iVar3 = iVar3 + 0xd;
    }
    else {
      iVar3 = 0;
    }
    DAT_0065c9e4 = 0x18;
    if (iVar3 != 0) {
      DAT_0065c9e4 = FUN_0058b42c(iVar3);
      if (DAT_0065c9e4 < 8) {
        DAT_0065c9e4 = 8;
      }
      else if (0x18 < DAT_0065c9e4) {
        DAT_0065c9e4 = 0x18;
      }
    }
  }
  iVar3 = FUN_0058ad30(param_1,s_NOFASTLOCK_005d8128);
  if (iVar3 == 0) {
    iVar3 = FUN_0058ad30(param_1,s_FASTLOCK__005d8134);
    if (iVar3 == 0) {
      iVar3 = FUN_0058ad30(param_1,s_FASTLOCK_005d8148);
      if (iVar3 == 0) goto LAB_004843b9;
    }
    else {
      iVar4 = FUN_0059a600(iVar3 + 9,&DAT_005d8140,2);
      if ((iVar4 != 0) && (iVar3 = FUN_0059a600(iVar3 + 9,&DAT_005d8144,1), iVar3 != 0)) {
        DAT_005d7a54 = 0;
        goto LAB_004843b9;
      }
    }
    DAT_005d7a54 = 1;
  }
  else {
    DAT_005d7a54 = 0;
  }
LAB_004843b9:
  iVar3 = FUN_0058ad30(param_1,s_WRITECOMMANDS_005d8154);
  if (iVar3 != 0) {
    FUN_00483a10(s___commands_txt_005d8164);
  }
  iVar3 = FUN_0058ad30(param_1,s_VERSION_005d8174);
  if (iVar3 == 0) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  FUN_0058d065(0x7a);
}


