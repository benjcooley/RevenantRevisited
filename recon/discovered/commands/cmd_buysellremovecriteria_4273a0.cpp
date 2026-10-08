// FUN_004273a0 @ 004273a0 size=344

undefined4 FUN_004273a0(undefined4 param_1,int param_2)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  char *pcVar6;
  undefined4 *puVar7;
  char *pcVar8;
  char cStack_20;
  undefined4 auStack_1f [7];
  
  cStack_20 = DAT_006554a8;
  puVar7 = auStack_1f;
  for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar7 = 0;
    puVar7 = puVar7 + 1;
  }
  *(undefined1 *)puVar7 = 0;
  if ((*(int *)(param_2 + 0x10) != 2) && (*(int *)(param_2 + 0x10) != 4)) {
    FUN_0041ee50(s_Name_required_005cc7a8);
    return 4;
  }
  uVar4 = 0xffffffff;
  pcVar6 = *(char **)(param_2 + 0x28);
  do {
    pcVar8 = pcVar6;
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    pcVar8 = pcVar6 + 1;
    cVar1 = *pcVar6;
    pcVar6 = pcVar8;
  } while (cVar1 != '\0');
  uVar4 = ~uVar4;
  pcVar6 = pcVar8 + -uVar4;
  pcVar8 = &cStack_20;
  for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
    *(undefined4 *)pcVar8 = *(undefined4 *)pcVar6;
    pcVar6 = pcVar6 + 4;
    pcVar8 = pcVar8 + 4;
  }
  for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
    *pcVar8 = *pcVar6;
    pcVar6 = pcVar6 + 1;
    pcVar8 = pcVar8 + 1;
  }
  FUN_00478a10();
  FUN_00479580();
  if (*(int *)(param_2 + 0x10) == 8) {
    iVar3 = *(int *)(param_2 + 0x14);
  }
  else {
    if (*(int *)(param_2 + 0x10) != 4) {
      FUN_0041ee50(s_Invalid_Params_005cc7cc);
      return 4;
    }
    iVar3 = FUN_00497800(*(undefined4 *)(param_2 + 0x28),param_1);
    if (iVar3 == -20000000) {
      FUN_0041ee50(s_Invalid_min_value_005cc7b8);
      return 4;
    }
  }
  FUN_00478a10();
  FUN_00479580();
  if (*(int *)(param_2 + 0x10) == 8) {
    iVar2 = *(int *)(param_2 + 0x14);
  }
  else {
    if (*(int *)(param_2 + 0x10) != 4) {
      FUN_0041ee50(s_Invalid_Params_005cc7f0);
      return 4;
    }
    iVar2 = FUN_00497800(*(undefined4 *)(param_2 + 0x28),param_1);
    if (iVar2 == -20000000) {
      FUN_0041ee50(s_Invalid_max_value_005cc7dc);
      return 4;
    }
  }
  FUN_00532340(&cStack_20,iVar3,iVar2);
  return 0;
}


