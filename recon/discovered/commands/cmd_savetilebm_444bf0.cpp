// FUN_00444bf0 @ 00444bf0 size=1

undefined4 FUN_00444bf0(int param_1)

{
  char cVar1;
  undefined4 uVar2;
  int iVar3;
  DWORD DVar4;
  int *piVar5;
  uint uVar6;
  undefined **ppuVar7;
  char *pcVar8;
  uint uVar9;
  char *pcVar10;
  undefined1 auStack_450 [68];
  undefined1 auStack_40c [12];
  char acStack_400 [128];
  char acStack_380 [128];
  char acStack_300 [256];
  undefined1 auStack_200 [3];
  undefined1 auStack_1fd [509];
  
  if (param_1 != 0) {
    uVar9 = 0;
    ppuVar7 = &PTR_DAT_005cee30;
    uVar2 = **(undefined4 **)(param_1 + 0x4c);
    do {
      iVar3 = FUN_0059a600(uVar2,*ppuVar7,3);
      if (iVar3 == 0) {
        if (uVar9 < 0xc) goto code_r0x00444c3c;
        break;
      }
      ppuVar7 = ppuVar7 + 1;
      uVar9 = uVar9 + 1;
    } while (ppuVar7 < s_editor_dat_005cee60);
    uVar9 = 0;
code_r0x00444c3c:
    FUN_0058b100(auStack_200,0x5cfd00,**(undefined4 **)(param_1 + 0x4c));
    FUN_0058b100(acStack_400,s__s_s__s_005cfd04,&DAT_0065ba0c,(&PTR_DAT_005cee00)[uVar9],auStack_1fd
                );
    uVar9 = 0xffffffff;
    pcVar8 = acStack_400;
    do {
      pcVar10 = pcVar8;
      if (uVar9 == 0) break;
      uVar9 = uVar9 - 1;
      pcVar10 = pcVar8 + 1;
      cVar1 = *pcVar8;
      pcVar8 = pcVar10;
    } while (cVar1 != '\0');
    uVar9 = ~uVar9;
    pcVar8 = pcVar10 + -uVar9;
    pcVar10 = acStack_380;
    for (uVar6 = uVar9 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
      *(undefined4 *)pcVar10 = *(undefined4 *)pcVar8;
      pcVar8 = pcVar8 + 4;
      pcVar10 = pcVar10 + 4;
    }
    for (uVar9 = uVar9 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
      *pcVar10 = *pcVar8;
      pcVar8 = pcVar8 + 1;
      pcVar10 = pcVar10 + 1;
    }
    iVar3 = -1;
    pcVar8 = acStack_380;
    do {
      pcVar10 = pcVar8;
      if (iVar3 == 0) break;
      iVar3 = iVar3 + -1;
      pcVar10 = pcVar8 + 1;
      cVar1 = *pcVar8;
      pcVar8 = pcVar10;
    } while (cVar1 != '\0');
    *(undefined4 *)(pcVar10 + -1) = DAT_005cfd0c;
    pcVar10[3] = DAT_005cfd10;
    iVar3 = FUN_0058b5db(acStack_380,&DAT_005cfd14);
    if (iVar3 != 0) {
      FUN_0058b4f1(iVar3);
      FUN_0058b100(auStack_450,s__s_already_exists___Overwrite__Y_005cfd18,acStack_380);
      FUN_0041ee50(auStack_450);
      do {
        DVar4 = WaitForMultipleObjects(2,(HANDLE *)&DAT_00656b20,0,0xffffffff);
        if (DVar4 != 1) {
          return 0;
        }
        ResetEvent(DAT_00656b24);
        pcVar8 = DAT_00656db4;
        if (DAT_006581b8 == -1) {
          return 0;
        }
      } while (((DAT_006581b8 != 0xd) || (DAT_00656db4 == (char *)0x0)) ||
              (iVar3 = FUN_0058ade0(DAT_00656db4,10), iVar3 == 0));
      cVar1 = *pcVar8;
      for (iVar3 = 0; ((cVar1 != '\0' && (cVar1 != '\n')) && (iVar3 < 0xff)); iVar3 = iVar3 + 1) {
        pcVar8 = pcVar8 + 1;
        acStack_300[iVar3] = cVar1;
        cVar1 = *pcVar8;
      }
      acStack_300[iVar3] = '\0';
      if ((acStack_300[0] != 'y') && (acStack_300[0] != 'Y')) {
        return 0;
      }
    }
    piVar5 = (int *)FUN_0046e8a0();
    iVar3 = (**(code **)(*piVar5 + 0x30))(acStack_400,0,1);
    if (iVar3 == 0) {
      pcVar8 = s_Error_writing__s_BMP__005cfd5c;
    }
    else {
      pcVar8 = s__s_BMP_successfully_saved__005cfd40;
    }
    FUN_0058b100(&stack0xfffffba4,pcVar8,auStack_40c);
    FUN_0041ee50(&stack0xfffffba4);
  }
  return 0;
}


