// FUN_004438d0 @ 004438d0 size=1

undefined4 FUN_004438d0(void)

{
  char cVar1;
  short sVar2;
  int iVar3;
  DWORD DVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  undefined4 uVar10;
  undefined1 *puVar11;
  int iVar12;
  char *pcVar13;
  char *pcVar14;
  char *pcVar15;
  undefined4 *puVar16;
  bool bVar17;
  int iVar18;
  HANDLE in_stack_00000010;
  char in_stack_00000020;
  DWORD in_stack_00000154;
  DWORD in_stack_00000180;
  WORD in_stack_00000184;
  char in_stack_00000198;
  
  FUN_0058c030();
  iVar18 = 0;
  FUN_00483300();
  FUN_00483300();
  FUN_00483300();
  FUN_0058b100();
  iVar3 = FUN_0058b5db();
  if (iVar3 == 0) {
    return 0;
  }
  iVar3 = -1;
  pcVar13 = pcRam005ced84;
  do {
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  FUN_0058beb8();
  FUN_0058b4f1();
  do {
    FUN_0041ee50();
    FUN_0041ee50();
    FUN_0041ee50();
    FUN_0041ee50();
    FUN_0041ee50();
    FUN_0041ee50();
    FUN_0041ee50();
    FUN_0041ee50();
    FUN_0041ee50();
    FUN_0041ee50();
    FUN_0041ee50();
    do {
      DVar4 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
      if (DVar4 != 1) {
        return 0;
      }
      ResetEvent(DAT_00656b24);
      pcVar13 = DAT_00656db4;
      if (DAT_006581b8 == -1) {
        return 0;
      }
    } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
            (iVar3 = FUN_0058ade0(), iVar3 == 0));
    cVar1 = *pcVar13;
    for (iVar3 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar3 < 0x103)); iVar3 = iVar3 + 1) {
      pcVar13 = pcVar13 + 1;
      (&stack0x00000198)[iVar3] = cVar1;
      cVar1 = *pcVar13;
    }
    (&stack0x00000198)[iVar3] = 0;
    if (in_stack_00000198 == '\0') {
      return 0;
    }
    iVar3 = FUN_0058b42c();
  } while ((iVar3 < 1) || (0x19 < iVar3));
  uVar5 = iVar3 - 1;
  if (uVar5 == 9) {
    do {
      FUN_0041ee50();
      FUN_0041ee50();
      FUN_0041ee50();
      FUN_0041ee50();
      FUN_0041ee50();
      FUN_0041ee50();
      FUN_0041ee50();
      FUN_0041ee50();
      do {
        DVar4 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
        if (DVar4 != 1) {
          return 0;
        }
        ResetEvent(DAT_00656b24);
        pcVar13 = DAT_00656db4;
        if (DAT_006581b8 == -1) {
          return 0;
        }
      } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
              (iVar3 = FUN_0058ade0(), iVar3 == 0));
      cVar1 = *pcVar13;
      for (iVar3 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar3 < 0x103)); iVar3 = iVar3 + 1) {
        pcVar13 = pcVar13 + 1;
        (&stack0x00000198)[iVar3] = cVar1;
        cVar1 = *pcVar13;
      }
      (&stack0x00000198)[iVar3] = 0;
      if (in_stack_00000198 == '\0') {
        return 0;
      }
      iVar3 = FUN_0058b42c();
    } while ((iVar3 < 1) || (0xc < iVar3));
    pcVar13 = (&PTR_DAT_005cedfc)[iVar3];
    iVar3 = iVar3 + -1;
  }
  else {
    pcVar13 = (&PTR_DAT_005ced9c)[uVar5];
    iVar3 = 0;
  }
  uVar8 = 0xffffffff;
  do {
    pcVar14 = pcVar13;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar14 = pcVar13 + 1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar14;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
  pcVar13 = pcVar14 + -uVar8;
  pcVar14 = &stack0x000005e0;
  for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined4 *)pcVar14 = *(undefined4 *)pcVar13;
    pcVar13 = pcVar13 + 4;
    pcVar14 = pcVar14 + 4;
  }
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *pcVar14 = *pcVar13;
    pcVar13 = pcVar13 + 1;
    pcVar14 = pcVar14 + 1;
  }
code_r0x00443bf1:
  iVar12 = iVar18 * 200;
  *(uint *)(&stack0x00000660 + iVar12) = uVar5;
  uVar8 = 0xffffffff;
  *(int *)(&stack0x00000664 + iVar12) = iVar3;
  pcVar13 = &stack0x000005e0;
  do {
    pcVar14 = pcVar13;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar14 = pcVar13 + 1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar14;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
  pcVar13 = pcVar14 + -uVar8;
  pcVar14 = &stack0x000005e0 + iVar12;
  for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined4 *)pcVar14 = *(undefined4 *)pcVar13;
    pcVar13 = pcVar13 + 4;
    pcVar14 = pcVar14 + 4;
  }
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *pcVar14 = *pcVar13;
    pcVar13 = pcVar13 + 1;
    pcVar14 = pcVar14 + 1;
  }
  FUN_0041ee50();
  FUN_0041ee50();
  do {
    DVar4 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
    if (DVar4 != 1) {
      return 0;
    }
    ResetEvent(DAT_00656b24);
    pcVar13 = DAT_00656db4;
    if (DAT_006581b8 == -1) {
      return 0;
    }
  } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
          (iVar6 = FUN_0058ade0(), iVar6 == 0));
  cVar1 = *pcVar13;
  for (iVar6 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar6 < 0x3f)); iVar6 = iVar6 + 1) {
    pcVar13 = pcVar13 + 1;
    (&stack0x000005a0)[iVar6 + iVar12] = cVar1;
    cVar1 = *pcVar13;
  }
  pcVar13 = &stack0x000005a0 + iVar12;
  pcVar13[iVar6] = '\0';
  if (*pcVar13 == '\0') {
    return 0;
  }
  uVar8 = 0xffffffff;
  pcVar14 = pcVar13;
  do {
    pcVar15 = pcVar14;
    if (uVar8 == 0) break;
    uVar8 = uVar8 - 1;
    pcVar15 = pcVar14 + 1;
    cVar1 = *pcVar14;
    pcVar14 = pcVar15;
  } while (cVar1 != '\0');
  uVar8 = ~uVar8;
  pcVar14 = pcVar15 + -uVar8;
  pcVar15 = &stack0x000003a0;
  for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
    *(undefined4 *)pcVar15 = *(undefined4 *)pcVar14;
    pcVar14 = pcVar14 + 4;
    pcVar15 = pcVar15 + 4;
  }
  for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
    *pcVar15 = *pcVar14;
    pcVar14 = pcVar14 + 1;
    pcVar15 = pcVar15 + 1;
  }
  if (*(int *)(&stack0x00000664 + iVar12) != 0) {
    FUN_0058b100();
    uVar8 = 0xffffffff;
    pcVar14 = &stack0x00000198;
    do {
      pcVar15 = pcVar14;
      if (uVar8 == 0) break;
      uVar8 = uVar8 - 1;
      pcVar15 = pcVar14 + 1;
      cVar1 = *pcVar14;
      pcVar14 = pcVar15;
    } while (cVar1 != '\0');
    uVar8 = ~uVar8;
    pcVar14 = pcVar15 + -uVar8;
    for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
      *(undefined4 *)pcVar13 = *(undefined4 *)pcVar14;
      pcVar14 = pcVar14 + 4;
      pcVar13 = pcVar13 + 4;
    }
    for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
      *pcVar13 = *pcVar14;
      pcVar14 = pcVar14 + 1;
      pcVar13 = pcVar13 + 1;
    }
  }
  uVar8 = *(uint *)(&stack0x00000660 + iVar12);
  if (((DAT_0065a258 <= uVar8) || ((&DAT_0065a148)[uVar8] == 0)) ||
     (iVar6 = FUN_00475210(), iVar6 == -1)) {
code_r0x00443e5d:
    do {
      FUN_0041ee50();
      FUN_0041ee50();
      pcVar13 = &stack0x00000620 + iVar12;
      do {
        DVar4 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
        if (DVar4 != 1) {
          return 0;
        }
        ResetEvent(DAT_00656b24);
        pcVar14 = DAT_00656db4;
        if (DAT_006581b8 == -1) {
          return 0;
        }
      } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
              (iVar6 = FUN_0058ade0(), iVar6 == 0));
      cVar1 = *pcVar14;
      for (iVar6 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar6 < 0x3f)); iVar6 = iVar6 + 1) {
        pcVar14 = pcVar14 + 1;
        pcVar13[iVar6] = cVar1;
        cVar1 = *pcVar14;
      }
      pcVar13[iVar6] = '\0';
      if (*pcVar13 == '\0') {
        return 0;
      }
      iVar6 = 2;
      bVar17 = true;
      pcVar14 = pcVar13;
      pcVar15 = (char *)0x5cf85c;
      do {
        if (iVar6 == 0) break;
        iVar6 = iVar6 + -1;
        bVar17 = *pcVar14 == *pcVar15;
        pcVar14 = pcVar14 + 1;
        pcVar15 = pcVar15 + 1;
      } while (bVar17);
      if (bVar17) {
        uVar8 = 0xffffffff;
        pcVar14 = &stack0x000003a0;
        do {
          pcVar15 = pcVar14;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pcVar15 = pcVar14 + 1;
          cVar1 = *pcVar14;
          pcVar14 = pcVar15;
        } while (cVar1 != '\0');
        uVar8 = ~uVar8;
        pcVar14 = pcVar15 + -uVar8;
        pcVar15 = pcVar13;
        for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
          *(undefined4 *)pcVar15 = *(undefined4 *)pcVar14;
          pcVar14 = pcVar14 + 4;
          pcVar15 = pcVar15 + 4;
        }
        for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
          *pcVar15 = *pcVar14;
          pcVar14 = pcVar14 + 1;
          pcVar15 = pcVar15 + 1;
        }
      }
      iVar6 = FUN_0058ade0();
      if (iVar6 == 0) {
        iVar6 = -1;
        pcVar14 = pcVar13;
        do {
          pcVar15 = pcVar14;
          if (iVar6 == 0) break;
          iVar6 = iVar6 + -1;
          pcVar15 = pcVar14 + 1;
          cVar1 = *pcVar14;
          pcVar14 = pcVar15;
        } while (cVar1 != '\0');
        *(undefined4 *)(pcVar15 + -1) = uRam005cf860;
        pcVar15[3] = cRam005cf864;
      }
      FUN_0058b100();
      uVar8 = 0xffffffff;
      pcVar14 = &stack0x00000198;
      do {
        pcVar15 = pcVar14;
        if (uVar8 == 0) break;
        uVar8 = uVar8 - 1;
        pcVar15 = pcVar14 + 1;
        cVar1 = *pcVar14;
        pcVar14 = pcVar15;
      } while (cVar1 != '\0');
      uVar8 = ~uVar8;
      pcVar14 = pcVar15 + -uVar8;
      for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
        *(undefined4 *)pcVar13 = *(undefined4 *)pcVar14;
        pcVar14 = pcVar14 + 4;
        pcVar13 = pcVar13 + 4;
      }
      for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
        *pcVar13 = *pcVar14;
        pcVar14 = pcVar14 + 1;
        pcVar13 = pcVar13 + 1;
      }
      FUN_0058b100();
      iVar6 = func_0x00445760();
      if (iVar6 != 0) {
        uVar8 = 0xffffffff;
        pcVar13 = &stack0x00000020;
        do {
          pcVar14 = pcVar13;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pcVar14 = pcVar13 + 1;
          cVar1 = *pcVar13;
          pcVar13 = pcVar14;
        } while (cVar1 != '\0');
        uVar8 = ~uVar8;
        pcVar13 = pcVar14 + -uVar8;
        pcVar14 = &stack0x00000198;
        for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
          *(undefined4 *)pcVar14 = *(undefined4 *)pcVar13;
          pcVar13 = pcVar13 + 4;
          pcVar14 = pcVar14 + 4;
        }
        for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
          *pcVar14 = *pcVar13;
          pcVar13 = pcVar13 + 1;
          pcVar14 = pcVar14 + 1;
        }
        pcVar13 = (char *)FUN_0058ade0();
        uVar8 = 0xffffffff;
        pcVar14 = (char *)0x5cf8c0;
        do {
          pcVar15 = pcVar14;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pcVar15 = pcVar14 + 1;
          cVar1 = *pcVar14;
          pcVar14 = pcVar15;
        } while (cVar1 != '\0');
        uVar8 = ~uVar8;
        pcVar14 = pcVar15 + -uVar8;
        for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
          *(undefined4 *)pcVar13 = *(undefined4 *)pcVar14;
          pcVar14 = pcVar14 + 4;
          pcVar13 = pcVar13 + 4;
        }
        for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
          *pcVar13 = *pcVar14;
          pcVar14 = pcVar14 + 1;
          pcVar13 = pcVar13 + 1;
        }
        pcVar13 = (char *)FUN_0058ade0();
        uVar8 = 0xffffffff;
        pcVar14 = (char *)0x5cf8c8;
        do {
          pcVar15 = pcVar14;
          if (uVar8 == 0) break;
          uVar8 = uVar8 - 1;
          pcVar15 = pcVar14 + 1;
          cVar1 = *pcVar14;
          pcVar14 = pcVar15;
        } while (cVar1 != '\0');
        uVar8 = ~uVar8;
        pcVar14 = pcVar15 + -uVar8;
        for (uVar9 = uVar8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
          *(undefined4 *)pcVar13 = *(undefined4 *)pcVar14;
          pcVar14 = pcVar14 + 4;
          pcVar13 = pcVar13 + 4;
        }
        for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
          *pcVar13 = *pcVar14;
          pcVar14 = pcVar14 + 1;
          pcVar13 = pcVar13 + 1;
        }
        iVar6 = func_0x00445760();
        if ((iVar6 != 0) || (iVar6 = func_0x00445760(), iVar6 != 0)) {
          iVar12 = FUN_0058b5db();
          if (iVar12 == 0) {
            return 0;
          }
          iVar12 = -1;
          pcVar13 = PTR_s_IMAGERY_005ced88;
          goto code_r0x0044423d;
        }
        while( true ) {
          FUN_0041ee50();
          FUN_0041ee50();
          FUN_0041ee50();
          do {
            DVar4 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
            if (DVar4 != 1) {
              return 0;
            }
            ResetEvent(DAT_00656b24);
            pcVar13 = DAT_00656db4;
            if (DAT_006581b8 == -1) {
              return 0;
            }
          } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
                  (iVar6 = FUN_0058ade0(), iVar6 == 0));
          cVar1 = *pcVar13;
          for (iVar6 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar6 < 0xff)); iVar6 = iVar6 + 1)
          {
            pcVar13 = pcVar13 + 1;
            (&stack0x00000020)[iVar6] = cVar1;
            cVar1 = *pcVar13;
          }
          (&stack0x00000020)[iVar6] = 0;
          if (in_stack_00000020 == '\0') {
            return 0;
          }
          in_stack_00000020 = FUN_0058b266();
          if (in_stack_00000020 == 'Y') break;
          if (in_stack_00000020 == 'N') goto code_r0x00444346;
        }
        goto code_r0x00443e5d;
      }
      while( true ) {
        FUN_0041ee50();
        FUN_0041ee50();
        FUN_0041ee50();
        do {
          DVar4 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
          if (DVar4 != 1) {
            return 0;
          }
          ResetEvent(DAT_00656b24);
          pcVar13 = DAT_00656db4;
          if (DAT_006581b8 == -1) {
            return 0;
          }
        } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
                (iVar6 = FUN_0058ade0(), iVar6 == 0));
        cVar1 = *pcVar13;
        for (iVar6 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar6 < 0xff)); iVar6 = iVar6 + 1) {
          pcVar13 = pcVar13 + 1;
          (&stack0x00000020)[iVar6] = cVar1;
          cVar1 = *pcVar13;
        }
        (&stack0x00000020)[iVar6] = 0;
        if (in_stack_00000020 == '\0') {
          return 0;
        }
        in_stack_00000020 = FUN_0058b266();
        if (in_stack_00000020 == 'Y') break;
        if (in_stack_00000020 == 'N') goto code_r0x00444346;
      }
    } while( true );
  }
  do {
    FUN_0041ee50();
    FUN_0041ee50();
    FUN_0041ee50();
    do {
      DVar4 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
      if (DVar4 != 1) {
        return 0;
      }
      ResetEvent(DAT_00656b24);
      pcVar13 = DAT_00656db4;
      if (DAT_006581b8 == -1) {
        return 0;
      }
    } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
            (iVar6 = FUN_0058ade0(), iVar6 == 0));
    cVar1 = *pcVar13;
    for (iVar6 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar6 < 0xff)); iVar6 = iVar6 + 1) {
      pcVar13 = pcVar13 + 1;
      (&stack0x00000020)[iVar6] = cVar1;
      cVar1 = *pcVar13;
    }
    (&stack0x00000020)[iVar6] = 0;
    if (in_stack_00000020 == '\0') {
      return 0;
    }
    in_stack_00000020 = FUN_0058b266();
    if (in_stack_00000020 == 'Y') goto code_r0x00443e5d;
  } while (in_stack_00000020 != 'N');
  goto code_r0x00444346;
  while( true ) {
    iVar12 = iVar12 + -1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
    if (cVar1 == '\0') break;
code_r0x0044423d:
    if (iVar12 == 0) break;
  }
  FUN_0058beb8();
  FUN_0058b100();
  iVar12 = -1;
  pcVar13 = &stack0x00000020;
  do {
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  FUN_0058beb8();
  iVar12 = -1;
  pcVar13 = PTR_s_BEGIN_005ced8c;
  do {
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  FUN_0058beb8();
  iVar12 = -1;
  pcVar13 = PTR_s_BITMAP_005ced90;
  do {
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  FUN_0058beb8();
  FUN_0058b100();
  iVar12 = -1;
  pcVar13 = &stack0x00000020;
  do {
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  FUN_0058beb8();
  iVar12 = -1;
  pcVar13 = PTR_s_imflags_ANIIM_UNLIT_bmflags_BM_8_005ced94;
  do {
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  FUN_0058beb8();
  iVar12 = -1;
  pcVar13 = PTR_DAT_005ced98;
  do {
    if (iVar12 == 0) break;
    iVar12 = iVar12 + -1;
    cVar1 = *pcVar13;
    pcVar13 = pcVar13 + 1;
  } while (cVar1 != '\0');
  FUN_0058beb8();
  FUN_0058b4f1();
  iVar18 = iVar18 + 1;
code_r0x00444346:
  if (0x31 < iVar18) {
code_r0x0044441f:
    if (iVar18 == 0) {
      return 0;
    }
    FUN_0041ee50();
    puVar16 = &stack0x00000154;
    for (iVar3 = 0x11; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar16 = 0;
      puVar16 = puVar16 + 1;
    }
    in_stack_00000154 = 0x44;
    in_stack_00000180 = 1;
    in_stack_00000184 = 0;
    FUN_0058b100();
    FUN_0058b100();
    FUN_00446830();
    FUN_0058b100();
    CreateProcessA((LPCSTR)0x0,&stack0x00000020,(LPSECURITY_ATTRIBUTES)0x0,
                   (LPSECURITY_ATTRIBUTES)0x0,0,0x80,(LPVOID)0x0,&DAT_0065ba0c,
                   (LPSTARTUPINFOA)&stack0x00000154,(LPPROCESS_INFORMATION)&stack0x00000010);
    WaitForSingleObject(in_stack_00000010,0xffffffff);
    FUN_0041ee50();
    FUN_0041ee50();
    while( true ) {
      DVar4 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
      if (DVar4 != 1) {
        return 0;
      }
      ResetEvent(DAT_00656b24);
      pcVar13 = DAT_00656db4;
      if (DAT_006581b8 == -1) break;
      if (((DAT_006581b8 == 0xd) && (DAT_00656db4 != (char *)0x0)) &&
         (iVar3 = FUN_0058ade0(), iVar3 != 0)) {
        cVar1 = *pcVar13;
        for (iVar3 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar3 < 0xff)); iVar3 = iVar3 + 1) {
          pcVar13 = pcVar13 + 1;
          (&stack0x00000020)[iVar3] = cVar1;
          cVar1 = *pcVar13;
        }
        (&stack0x00000020)[iVar3] = 0;
        FUN_0058bff1();
        if (0 < iVar18) {
          puVar11 = &stack0x000005a0;
          do {
            FUN_004433c0();
            if ((*(uint *)(puVar11 + 0xc0) < DAT_0065a258) &&
               (iVar3 = (&DAT_0065a148)[*(uint *)(puVar11 + 0xc0)], iVar3 != 0)) {
              iVar12 = FUN_00475210();
              if (iVar12 == -1) {
                FUN_0058b100();
                iVar12 = FUN_004746d0();
                if (iVar12 < 0) {
                  FUN_0041ee50();
                }
                else {
                  sVar2 = *(short *)(iVar3 + 0x14);
                  uVar5 = 0;
                  if (0 < sVar2) {
                    iVar6 = 0;
                    iVar12 = *(int *)(*(int *)(iVar3 + 0x34) + iVar12 * 4);
                    do {
                      if (uVar5 < (uint)(int)*(short *)(iVar3 + 0x14)) {
                        uVar10 = *(undefined4 *)(*(int *)(iVar3 + 0x18) + 0x40 + iVar6);
                      }
                      else {
                        uVar10 = 0;
                      }
                      iVar7 = iVar12;
                      if (iVar12 == 0) {
                        iVar7 = *(int *)(iVar3 + 0x38);
                      }
                      uVar5 = uVar5 + 1;
                      iVar6 = iVar6 + 0x50;
                      *(undefined4 *)(*(int *)(iVar7 + 0x10) + -4 + uVar5 * 4) = uVar10;
                    } while ((int)uVar5 < (int)sVar2);
                  }
                  FUN_0058b100();
                  FUN_0041ee50();
                }
              }
              else {
                FUN_0058b100();
                FUN_00446840();
                sVar2 = *(short *)(iVar3 + 0x14);
                uVar5 = 0;
                if (0 < sVar2) {
                  iVar6 = 0;
                  iVar12 = *(int *)(*(int *)(iVar3 + 0x34) + iVar12 * 4);
                  do {
                    if (uVar5 < (uint)(int)*(short *)(iVar3 + 0x14)) {
                      uVar10 = *(undefined4 *)(*(int *)(iVar3 + 0x18) + 0x40 + iVar6);
                    }
                    else {
                      uVar10 = 0;
                    }
                    iVar7 = iVar12;
                    if (iVar12 == 0) {
                      iVar7 = *(int *)(iVar3 + 0x38);
                    }
                    uVar5 = uVar5 + 1;
                    iVar6 = iVar6 + 0x50;
                    *(undefined4 *)(*(int *)(iVar7 + 0x10) + -4 + uVar5 * 4) = uVar10;
                  } while ((int)uVar5 < (int)sVar2);
                }
                FUN_0058b100();
                FUN_0041ee50();
              }
            }
            puVar11 = puVar11 + 200;
            iVar18 = iVar18 + -1;
          } while (iVar18 != 0);
        }
        return 0;
      }
    }
    return 0;
  }
  while( true ) {
    FUN_0041ee50();
    FUN_0041ee50();
    do {
      DVar4 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
      if (DVar4 != 1) {
        return 0;
      }
      ResetEvent(DAT_00656b24);
      pcVar13 = DAT_00656db4;
      if (DAT_006581b8 == -1) {
        return 0;
      }
    } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
            (iVar12 = FUN_0058ade0(), iVar12 == 0));
    cVar1 = *pcVar13;
    for (iVar12 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar12 < 0xff)); iVar12 = iVar12 + 1) {
      pcVar13 = pcVar13 + 1;
      (&stack0x00000020)[iVar12] = cVar1;
      cVar1 = *pcVar13;
    }
    (&stack0x00000020)[iVar12] = 0;
    if (in_stack_00000020 == '\0') {
      return 0;
    }
    in_stack_00000020 = FUN_0058b266();
    if (in_stack_00000020 == 'Y') break;
    if (in_stack_00000020 == 'N') goto code_r0x0044441f;
  }
  FUN_0041ee50();
  goto code_r0x00443bf1;
}


