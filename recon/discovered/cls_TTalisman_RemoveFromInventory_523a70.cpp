// FUN_00523a70 @ 00523a70 size=94

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00523a70(int param_1)

{
  int iVar1;
  
  if (*(int *)(param_1 + 100) != 0) {
    iVar1 = FUN_0059a530(**(undefined4 **)(*(int *)(param_1 + 100) + 0x4c),s_Spell_Pouch_005e2dc4);
    if (iVar1 != 0) {
      iVar1 = FUN_0059a530(**(undefined4 **)(*(int *)(param_1 + 100) + 0x4c),s_SpellPouch_005e2dd0);
      if (iVar1 != 0) goto LAB_00523ac5;
    }
    _DAT_00666200 = 1;
    (**(code **)(DAT_006661b0 + 0x90))();
  }
LAB_00523ac5:
  FUN_0046faf0();
  return;
}


