// FUN_004ce1b0_BuildActionName @ 004ce1b0 size=213

void __thiscall FUN_004ce1b0_BuildActionName(int *param_1,char *param_2,char *param_3)

{
  char cVar1;
  char *pcVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  char unaff_BL;
  char *pcVar6;
  char *pcVar7;
  
  pcVar6 = param_2;
  if ((short)param_1[1] == 0xb) {
    pcVar2 = (char *)(**(code **)(*param_1 + 0x310))();
    uVar3 = 0xffffffff;
    do {
      pcVar7 = pcVar2;
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      pcVar7 = pcVar2 + 1;
      cVar1 = *pcVar2;
      pcVar2 = pcVar7;
    } while (cVar1 != '\0');
    uVar3 = ~uVar3;
    pcVar2 = pcVar7 + -uVar3;
    for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined4 *)pcVar6 = *(undefined4 *)pcVar2;
      pcVar2 = pcVar2 + 4;
      pcVar6 = pcVar6 + 4;
    }
  }
  else {
    uVar3 = 0xffffffff;
    pcVar2 = &DAT_005e00b8;
    do {
      pcVar7 = pcVar2;
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      pcVar7 = pcVar2 + 1;
      cVar1 = *pcVar2;
      pcVar2 = pcVar7;
    } while (cVar1 != '\0');
    uVar3 = ~uVar3;
    pcVar2 = pcVar7 + -uVar3;
    for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
      *(undefined4 *)pcVar6 = *(undefined4 *)pcVar2;
      pcVar2 = pcVar2 + 4;
      pcVar6 = pcVar6 + 4;
    }
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar6 = *pcVar2;
    pcVar2 = pcVar2 + 1;
    pcVar6 = pcVar6 + 1;
  }
  uVar3 = 0xffffffff;
  do {
    pcVar6 = param_3;
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    pcVar6 = param_3 + 1;
    cVar1 = *param_3;
    param_3 = pcVar6;
  } while (cVar1 != '\0');
  uVar3 = ~uVar3;
  iVar5 = -1;
  pcVar2 = param_2;
  do {
    pcVar7 = pcVar2;
    if (iVar5 == 0) break;
    iVar5 = iVar5 + -1;
    pcVar7 = pcVar2 + 1;
    cVar1 = *pcVar2;
    pcVar2 = pcVar7;
  } while (cVar1 != '\0');
  pcVar6 = pcVar6 + -uVar3;
  pcVar2 = pcVar7 + -1;
  for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *(undefined4 *)pcVar2 = *(undefined4 *)pcVar6;
    pcVar6 = pcVar6 + 4;
    pcVar2 = pcVar2 + 4;
  }
  for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *pcVar2 = *pcVar6;
    pcVar6 = pcVar6 + 1;
    pcVar2 = pcVar2 + 1;
  }
  if ((((param_2 != (char *)0x0) && ((param_1[0x44] & 0x2000U) != 0)) && (param_1[0x38] != 0)) &&
     (iVar5 = FUN_004dab80_TActionBlock_Is(&DAT_005e00bc), iVar5 != 0)) {
    iVar5 = *param_1;
    *param_2 = 'w';
    iVar5 = (**(code **)(iVar5 + 0x1f0))(param_2,0);
    if (iVar5 == 0) {
      *param_2 = unaff_BL;
    }
  }
  return;
}


