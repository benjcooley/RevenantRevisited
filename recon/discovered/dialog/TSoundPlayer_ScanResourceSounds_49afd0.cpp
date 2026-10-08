// FUN_0049afd0 @ 0049afd0 size=590

undefined4 __fastcall FUN_0049afd0(int param_1)

{
  char *pcVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  char *pcVar6;
  char *pcVar7;
  
  pcVar1 = (char *)(param_1 + 200);
  FUN_00483120(&DAT_0065dde8,pcVar1,0x104);
  uVar4 = 0xffffffff;
  pcVar6 = pcVar1;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar2 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar2 != '\0');
  iVar3 = -(~uVar4 - 1);
  _strncpy(pcVar1 + (~uVar4 - 1),s_sound_effects__005da7f0,iVar3 + 0x103);
  (pcVar1 + (~uVar4 - 1))[iVar3 + 0x103] = '\0';
  FUN_0059bd3e(pcVar1);
  iVar3 = FUN_0049ad20(pcVar1);
  if (iVar3 == 0) {
    return 0;
  }
  pcVar1 = (char *)(param_1 + 0x1cc);
  FUN_00483120(&DAT_0065dde8,pcVar1,0x104);
  uVar4 = 0xffffffff;
  pcVar6 = pcVar1;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar2 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar2 != '\0');
  iVar3 = -(~uVar4 - 1);
  _strncpy(pcVar1 + (~uVar4 - 1),s_sound__005da800,iVar3 + 0x103);
  (pcVar1 + (~uVar4 - 1))[iVar3 + 0x103] = '\0';
  uVar4 = 0xffffffff;
  pcVar6 = pcVar1;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar2 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar2 != '\0');
  iVar3 = -(~uVar4 - 1);
  _strncpy(pcVar1 + (~uVar4 - 1),&DAT_0065bc18,iVar3 + 0x103);
  (pcVar1 + (~uVar4 - 1))[iVar3 + 0x103] = '\0';
  uVar4 = 0xffffffff;
  pcVar6 = pcVar1;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar2 = *pcVar6;
    pcVar6 = pcVar6 + 1;
  } while (cVar2 != '\0');
  iVar3 = -(~uVar4 - 1);
  _strncpy(pcVar1 + (~uVar4 - 1),&DAT_005da808,iVar3 + 0x103);
  (pcVar1 + (~uVar4 - 1))[iVar3 + 0x103] = '\0';
  FUN_0059bd3e(pcVar1);
  iVar3 = FUN_0049ad20(pcVar1);
  if (iVar3 == 0) {
    return 0;
  }
  pcVar6 = (char *)(param_1 + 0x2d0);
  FUN_00483120(&DAT_0065bc44,pcVar6,0x104);
  uVar4 = 0xffffffff;
  pcVar7 = pcVar6;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar2 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar2 != '\0');
  iVar3 = -(~uVar4 - 1);
  _strncpy(pcVar6 + (~uVar4 - 1),s_sound_effects__005da80c,iVar3 + 0x103);
  (pcVar6 + (~uVar4 - 1))[iVar3 + 0x103] = '\0';
  FUN_0059bd3e(pcVar6);
  iVar3 = FUN_0059a530(param_1 + 200,pcVar6);
  if (iVar3 != 0) {
    FUN_0049ad20(pcVar6);
  }
  pcVar6 = (char *)(param_1 + 0x3d4);
  FUN_00483120(&DAT_0065bc44,pcVar6,0x104);
  uVar4 = 0xffffffff;
  pcVar7 = pcVar6;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar2 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar2 != '\0');
  iVar3 = -(~uVar4 - 1);
  _strncpy(pcVar6 + (~uVar4 - 1),s_sound__005da81c,iVar3 + 0x103);
  uVar5 = 0xffffffff;
  (pcVar6 + (~uVar4 - 1))[iVar3 + 0x103] = '\0';
  pcVar7 = pcVar6;
  do {
    if (uVar5 == 0) break;
    uVar5 = uVar5 - 1;
    cVar2 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar2 != '\0');
  iVar3 = -(~uVar5 - 1);
  _strncpy(pcVar6 + (~uVar5 - 1),&DAT_0065bc18,iVar3 + 0x103);
  uVar4 = 0xffffffff;
  (pcVar6 + (~uVar5 - 1))[iVar3 + 0x103] = '\0';
  pcVar7 = pcVar6;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar2 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar2 != '\0');
  iVar3 = -(~uVar4 - 1);
  _strncpy(pcVar6 + (~uVar4 - 1),&DAT_005da824,iVar3 + 0x103);
  (pcVar6 + (~uVar4 - 1))[iVar3 + 0x103] = '\0';
  FUN_0059bd3e(pcVar6);
  iVar3 = FUN_0059a530(pcVar1,pcVar6);
  if (iVar3 != 0) {
    FUN_0049ad20(pcVar6);
  }
  return 1;
}


