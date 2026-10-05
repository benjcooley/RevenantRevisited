// FUN_0049b220 @ 0049b220 size=474

undefined4 __fastcall FUN_0049b220(int param_1)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  char *pcVar5;
  char *pcVar6;
  
  if (DAT_00668114 == 0) {
    if ((DAT_0065a784 < 0) || (*(int *)(DAT_0065a77c + DAT_0065a784 * 4) == 0)) {
      return 0;
    }
    pcVar5 = (char *)(param_1 + 0x4d8);
    FUN_00483120(&DAT_0065d6a4,pcVar5,0x104);
    if (DAT_0065a784 < 0) {
      iVar4 = 0;
    }
    else {
      iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    uVar3 = 0xffffffff;
    pcVar6 = pcVar5;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar3 - 1);
    _strncpy(pcVar5 + (~uVar3 - 1),(char *)(iVar4 + 0x58),iVar2 + 0x103);
    (pcVar5 + (~uVar3 - 1))[iVar2 + 0x103] = '\0';
    uVar3 = 0xffffffff;
    pcVar6 = pcVar5;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    iVar4 = -(~uVar3 - 1);
    _strncpy(pcVar5 + (~uVar3 - 1),s__sound_effects__005da828,iVar4 + 0x103);
    (pcVar5 + (~uVar3 - 1))[iVar4 + 0x103] = '\0';
    iVar4 = FUN_0049ad20(pcVar5);
    if (iVar4 == 0) {
      return 0;
    }
    pcVar5 = (char *)(param_1 + 0x5dc);
    FUN_00483120(&DAT_0065d6a4,pcVar5,0x104);
    if (DAT_0065a784 < 0) {
      iVar4 = 0;
    }
    else {
      iVar4 = *(int *)(DAT_0065a77c + DAT_0065a784 * 4);
    }
    uVar3 = 0xffffffff;
    pcVar6 = pcVar5;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    iVar2 = -(~uVar3 - 1);
    _strncpy(pcVar5 + (~uVar3 - 1),(char *)(iVar4 + 0x58),iVar2 + 0x103);
    (pcVar5 + (~uVar3 - 1))[iVar2 + 0x103] = '\0';
    uVar3 = 0xffffffff;
    pcVar6 = pcVar5;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    iVar4 = -(~uVar3 - 1);
    _strncpy(pcVar5 + (~uVar3 - 1),s__sound__005da838,iVar4 + 0x103);
    (pcVar5 + (~uVar3 - 1))[iVar4 + 0x103] = '\0';
    uVar3 = 0xffffffff;
    pcVar6 = pcVar5;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    iVar4 = -(~uVar3 - 1);
    _strncpy(pcVar5 + (~uVar3 - 1),&DAT_0065bc18,iVar4 + 0x103);
    (pcVar5 + (~uVar3 - 1))[iVar4 + 0x103] = '\0';
    uVar3 = 0xffffffff;
    pcVar6 = pcVar5;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar6;
      pcVar6 = pcVar6 + 1;
    } while (cVar1 != '\0');
    iVar4 = -(~uVar3 - 1);
    _strncpy(pcVar5 + (~uVar3 - 1),&DAT_005da840,iVar4 + 0x103);
    (pcVar5 + (~uVar3 - 1))[iVar4 + 0x103] = '\0';
    iVar4 = FUN_0049ad20(pcVar5);
    if (iVar4 == 0) {
      return 0;
    }
    FUN_0058c9ff(*(undefined4 *)(param_1 + 0x2c),*(undefined4 *)(param_1 + 0x1c),4,&LAB_0049ab00);
  }
  return 1;
}


