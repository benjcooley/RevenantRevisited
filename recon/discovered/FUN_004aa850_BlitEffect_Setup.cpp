// FUN_004aa850_BlitEffect_Setup @ 004aa850 size=213

int FUN_004aa850_BlitEffect_Setup(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                int param_5)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  
  if (param_5 == 0) {
    return 0;
  }
  iVar3 = 0;
  piVar1 = &DAT_00669b14;
  do {
    if (*piVar1 == 0) break;
    piVar1 = piVar1 + 0x13;
    iVar3 = iVar3 + 1;
  } while ((int)piVar1 < 0x669e0c);
  if (iVar3 == 10) {
    return -1;
  }
  (&DAT_00669af0)[iVar3 * 0x13] = 0;
  (&DAT_00669afc)[iVar3 * 0x13] = 0;
  (&DAT_00669b00)[iVar3 * 0x13] = 0;
  (&DAT_00669b04)[iVar3 * 0x13] = 0;
  (&DAT_00669b08)[iVar3 * 0x13] = 0;
  (&DAT_00669af4)[iVar3 * 0x13] = param_1;
  (&DAT_00669af8)[iVar3 * 0x13] = param_2;
  (&DAT_00669b0c)[iVar3 * 0x13] = param_3;
  (&DAT_00669b10)[iVar3 * 0x13] = param_4;
  (&DAT_00669b14)[iVar3 * 0x13] = param_5;
  (&DAT_00669b18)[iVar3 * 0x13] = 0;
  uVar2 = FUN_00482fb0(0xa00);
  (&DAT_00669b1c)[iVar3 * 0x13] = uVar2;
  uVar2 = FUN_00482fb0(0xa00);
  (&DAT_00669b20)[iVar3 * 0x13] = uVar2;
  (&DAT_00669b2c)[iVar3 * 0x13] = 0;
  (&DAT_00669b34)[iVar3 * 0x13] = 0;
  (&DAT_00669b30)[iVar3 * 0x13] = 0;
  (&DAT_00669b38)[iVar3 * 0x13] = 0;
  return iVar3;
}


