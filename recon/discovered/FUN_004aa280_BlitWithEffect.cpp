// FUN_004aa280_BlitWithEffects @ 004aa280 size=145

undefined4 FUN_004aa280_BlitWithEffects(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  undefined4 unaff_retaddr;
  
  iVar1 = (**(code **)(*DAT_0066a8f8 + 0x60))(DAT_0066a8f8);
  if (iVar1 == -0x7789fe3e) {
    return 0;
  }
  DAT_0066a8fc = 0;
  uVar2 = FUN_004bd490_Blit(unaff_retaddr,param_1,param_2,param_3);
  iVar1 = 0;
  if (0 < DAT_0066a8fc) {
    puVar3 = &DAT_00669df4;
    do {
      FUN_004aacb0_BlitEffect_Iterate(puVar3[-3],puVar3[-2],puVar3[-1],*puVar3,puVar3[1]);
      iVar1 = iVar1 + 1;
      puVar3 = puVar3 + 5;
    } while (iVar1 < DAT_0066a8fc);
  }
  DAT_0066a8fc = 0;
  return uVar2;
}


