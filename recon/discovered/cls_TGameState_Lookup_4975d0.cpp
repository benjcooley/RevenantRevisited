// FUN_004975d0_TGameState_FindStateValue @ 004975d0 size=149

uint __thiscall FUN_004975d0_TGameState_FindStateValue(uint *param_1,undefined4 param_2)

{
  undefined **ppuVar1;
  int iVar2;
  undefined **ppuVar3;
  uint uVar4;
  int iVar5;
  uint *puVar6;
  
  iVar5 = 0;
  if (PTR_DAT_005da0b0 != (undefined *)0x0) {
    ppuVar3 = &PTR_DAT_005da0b0;
    do {
      iVar2 = FUN_0059a530_stricmp(param_2,*ppuVar3);
      if (iVar2 == 0) {
        if (-1 < iVar5) {
          return (&DAT_005da0b4)[iVar5 * 2];
        }
        break;
      }
      ppuVar1 = ppuVar3 + 2;
      ppuVar3 = ppuVar3 + 2;
      iVar5 = iVar5 + 1;
    } while (*ppuVar1 != (undefined *)0x0);
  }
  uVar4 = 0;
  if (0 < (int)*param_1) {
    puVar6 = param_1 + 0x1001;
    do {
      iVar5 = FUN_0059a530_stricmp(param_2,*puVar6);
      if (iVar5 == 0) goto LAB_00497634;
      uVar4 = uVar4 + 1;
      puVar6 = puVar6 + 1;
    } while ((int)uVar4 < (int)*param_1);
  }
  uVar4 = 0xffffffff;
LAB_00497634:
  if ((uVar4 < *param_1) && (-1 < (int)uVar4)) {
    return param_1[uVar4 + 1];
  }
  return 0xfeced300;
}


