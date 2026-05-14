// FUN_0051eda0_TPlayerManager_Clear @ 0051eda0 size=132

void __fastcall FUN_0051eda0_TPlayerManager_Clear(uint *param_1)

{
  undefined4 *puVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  
  uVar3 = 0;
  uVar2 = *param_1;
  if (0 < (int)uVar2) {
    do {
      if ((((param_1[4] != 0) && (uVar3 < uVar2)) &&
          (iVar4 = *(int *)(param_1[4] + uVar3 * 4), iVar4 != 0)) && (iVar4 != 0)) {
        FUN_004c18a0_TCharacter_ClearChar();
        FUN_00518750_TPlayer_ClearPlayer();
      }
      uVar2 = *param_1;
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < (int)uVar2);
  }
  FUN_0051eef0_TPlayerManager_SetMainPlayerNum(0xffffffff);
  iVar4 = 0;
  if (0 < (int)*param_1) {
    do {
      if ((-1 < iVar4) &&
         (puVar1 = *(undefined4 **)(param_1[4] + iVar4 * 4), puVar1 != (undefined4 *)0x0)) {
        (**(code **)*puVar1)(1);
      }
      FUN_0041cb40(iVar4);
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)*param_1);
  }
  *param_1 = 0;
  param_1[1] = 0;
  return;
}


