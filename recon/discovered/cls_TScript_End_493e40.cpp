// FUN_00493e40_TScript_End @ 00493e40 size=347

void __fastcall FUN_00493e40_TScript_End(uint *param_1)

{
  int iVar1;
  uint *puVar2;
  uint *puVar3;
  uint uVar4;
  
  param_1[4] = 0xffffffff;
  *(byte *)(param_1 + 0x2d) = 0;
  *(byte *)((int)param_1 + 0xb5) = 0;
  *(undefined2 *)(param_1 + 0x30) = 0;
  if (param_1[0x2e] != 0) {
    FUN_004830f0(param_1[0x2e]);
    param_1[0x2e] = 0;
  }
  if ((((param_1[0x33] == 0) || (iVar1 = FUN_0059a530_stricmp(param_1[0x33],&DAT_005da134), iVar1 != 0)) ||
      (uVar4 = param_1[0x31], uVar4 == 0)) || (*(short *)(uVar4 + 4) != 0xb)) {
    uVar4 = DAT_00667fcc;
  }
  puVar2 = param_1;
  if (uVar4 != 0) {
    puVar2 = *(uint **)(uVar4 + 0x36c);
  }
  if ((*param_1 & 4) != 0) {
    FUN_00536010();
    *param_1 = *param_1 & 0xfffffffb;
  }
  puVar3 = puVar2;
  if ((*param_1 & 1) != 0) {
    if (DAT_0066829c == 0) {
      FUN_0047c580(1);
    }
    else if (uVar4 != 0) {
      puVar3 = (uint *)((uint)puVar2 & 0xfffffffb);
    }
    *param_1 = *param_1 & 0xfffffffe;
  }
  if ((*param_1 & 2) != 0) {
    if ((DAT_0066829c == 0) && (DAT_0067682c != 0)) {
      if (DAT_0065cb30 != (int *)0x0) {
        (**(code **)(*DAT_0065cb30 + 0x28))();
      }
    }
    else if (uVar4 != 0) {
      puVar3 = (uint *)((uint)puVar3 & 0xfffffff7);
    }
    *param_1 = *param_1 & 0xfffffffd;
  }
  if ((*param_1 & 8) != 0) {
    if (DAT_0066829c == 0) {
      if ((-(uint)((DAT_006669b0 & 1) != 0) & DAT_006669b4) != DAT_00667fcc) {
        FUN_004538d0(DAT_00667fcc,8);
      }
    }
    else if (uVar4 != 0) {
      puVar3 = (uint *)((uint)puVar3 & 0xffffffef);
    }
    *param_1 = *param_1 & 0xfffffff7;
  }
  if (((DAT_0066829c != 0) && (uVar4 != 0)) && (puVar2 != puVar3)) {
    FUN_0051d680_SetPlayerState(puVar3);
  }
  return;
}


