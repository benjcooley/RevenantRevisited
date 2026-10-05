// FUN_004ddc40 @ 004ddc40 size=99

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __thiscall FUN_004ddc40(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = FUN_0046f3d0(param_2,param_3);
  iVar2 = FUN_0059a530(*(undefined4 *)(param_1 + 0x38),s_Spell_Pouch_005e0dd8);
  if ((iVar2 != 0) &&
     (iVar2 = FUN_0059a530(*(undefined4 *)(param_1 + 0x38),s_SpellPouch_005e0de4), iVar2 != 0)) {
    return uVar1;
  }
  _DAT_00666200 = 1;
  (**(code **)(DAT_006661b0 + 0x90))();
  return uVar1;
}


