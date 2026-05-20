// FUN_004aa7c0_BlitEffect_AddOrUpdate @ 004aa7c0 size=75

void FUN_004aa7c0_BlitEffect_AddOrUpdate(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  if (((-1 < param_1) && (param_1 < 10)) && ((&DAT_00669b14)[param_1 * 0x13] != 0)) {
    (&DAT_00669af4)[param_1 * 0x13] = param_2;
    (&DAT_00669af8)[param_1 * 0x13] = param_3;
    (&DAT_00669b0c)[param_1 * 0x13] = param_4;
    (&DAT_00669b10)[param_1 * 0x13] = param_5;
  }
  return;
}


