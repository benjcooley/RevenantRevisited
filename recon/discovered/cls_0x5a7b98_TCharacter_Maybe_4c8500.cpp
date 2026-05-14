// FUN_004c8500_EffectCombatFlash @ 004c8500 size=208

void __fastcall FUN_004c8500_EffectCombatFlash(int param_1)

{
  undefined4 uVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 local_34;
  undefined2 local_2a;
  int local_28;
  int local_24;
  int local_20;
  
  puVar5 = &local_34;
  for (iVar4 = 0xd; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar5 = 0;
    puVar5 = puVar5 + 1;
  }
  local_34._0_2_ = 0x19;
  local_34._2_2_ = FUN_00475210(s_combatflash_005df9ac,0);
  local_24 = *(int *)(param_1 + 0x14) + 0x1e;
  local_2a = (undefined2)DAT_00666970;
  local_20 = *(int *)(param_1 + 0x18) + 0x50;
  local_28 = *(int *)(param_1 + 0x10) + 0x1e;
  uVar1 = FUN_00450e40_TMapPane_NewObject(&local_34,0xffffffff);
  piVar2 = (int *)FUN_00452690_TMapPane_GetInstance(uVar1,0);
  if (piVar2 == (int *)0x0) {
    return;
  }
  if ((int *)piVar2[0x15] == (int *)0x0) {
    iVar4 = 1;
  }
  else {
    iVar4 = (**(code **)(*(int *)piVar2[0x15] + 0x3c))();
    if (iVar4 < 1) {
      (**(code **)(*piVar2 + 0x18))(0);
      return;
    }
  }
  iVar3 = FUN_00483300_RandomRange(0,iVar4 / 2);
  iVar3 = iVar3 * 2;
  iVar4 = iVar4 + -2;
  if (iVar3 - iVar4 != 0 && iVar4 <= iVar3) {
    iVar3 = iVar4;
  }
  (**(code **)(*piVar2 + 0x18))(iVar3);
  return;
}


