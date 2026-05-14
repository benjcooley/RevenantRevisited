// FUN_004cf000_SetWalkMode @ 004cf000 size=721
//
// IDENTIFICATION: TCharacter::SetWalkMode  (NOT TPlayer-specific despite this
// helper being called from cls_0x5b4f30_TPlayer_Pulse_4c9790.cpp).
// Source-side counterpart: src/character.cpp:2883  bool TCharacter::SetWalkMode().
//
// Anchors:
//   - vtable slot 0x304 = GetCombatRoot()   "combat"/"hand"/"cstaff"/"cknife"
//   - vtable slot 0x308 = GetBowRoot()      "cbow"/"bow"
//   - vtable slot 0x30c = GetTorchRoot()    "torch"/"walk" (TPlayer-only)
//   - vtable slot 0x1f0 = HasActionAni
//   - vtable slot 0x1f8 = SetRoot
//   - vtable slot 0x208 = SetDesired
//   - DAT_005e00f0 = "r", 005e00f4 = "r", 005e00f8 = "f"  (direction suffixes)
//   - FUN_0057d9d0_FireScriptEvent(this, 0x26, 1, 0, 0) = script-event SCMD_SETWALKMODE
//   - FUN_004daae0_TActionBlock_CopyCtor = TActionBlock::CopyCtor
//   - FUN_004dadd0_StName = StName(buf, suffix) — appends suffix in place.
// See recon/discovered/player_combat_notes.md for the full walkthrough.

undefined4 __fastcall FUN_004cf000_SetWalkMode(int *param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  char *pcVar5;
  uint uVar6;
  uint uVar7;
  char *pcVar8;
  undefined *puVar9;
  int iStack_30;
  char acStack_2c [20];
  void *pvStack_18;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e803;
  local_c = ExceptionList;
  if ((param_1[0x44] & 0x80000U) == 0) {
    iVar3 = *(int *)(param_1[0x36] + 0x48);
    if ((iVar3 != 0) && ((*(uint *)(iVar3 + 0x24) & 0x2000000) != 0)) {
      return 0;
    }
    iVar3 = *(int *)(param_1[0x36] + 0x4c);
    if ((iVar3 != 0) && ((*(byte *)(iVar3 + 0x24) & 0x80) != 0)) {
      return 0;
    }
  }
  ExceptionList = &local_c;
  iVar3 = (**(code **)(*param_1 + 0x1c0))();
  if (iVar3 < 1) {
    ExceptionList = local_c;
    return 0;
  }
  if (((int *)param_1[0x38] != (int *)0x0) &&
     ((((*(int *)param_1[0x38] == 3 &&
        ((iVar3 = FUN_004dab80_TActionBlock_Is(s_combat_005e0634), iVar3 != 0 ||
         (iVar3 = FUN_004dab80_TActionBlock_Is(&DAT_005e064c), iVar3 != 0)))) ||
       (((int *)param_1[0x38] != (int *)0x0 &&
        ((*(int *)param_1[0x38] == 0x19 && (iVar3 = FUN_004dab80_TActionBlock_Is(&DAT_005e0648), iVar3 != 0)))))) ||
      (iVar3 = FUN_004dab80_TActionBlock_Is(&DAT_005e062c), iVar3 != 0)))) {
    uVar4 = (**(code **)(*param_1 + 0x30c))(0);
    iVar3 = FUN_004dab80_TActionBlock_Is(uVar4);
    if (iVar3 != 0) {
      ExceptionList = local_c;
      return 1;
    }
  }
  iVar3 = *(int *)param_1[0x38];
  if (iVar3 == 3) {
    pcVar5 = (char *)(**(code **)(*param_1 + 0x304))(0);
  }
  else if (iVar3 == 0x19) {
    pcVar5 = (char *)(**(code **)(*param_1 + 0x308))(0);
  }
  else {
    if (iVar3 != 1) goto LAB_004cf172;
    pcVar5 = (char *)(**(code **)(*param_1 + 0x30c))(0);
  }
  uVar6 = 0xffffffff;
  do {
    pcVar8 = pcVar5;
    if (uVar6 == 0) break;
    uVar6 = uVar6 - 1;
    pcVar8 = pcVar5 + 1;
    cVar1 = *pcVar5;
    pcVar5 = pcVar8;
  } while (cVar1 != '\0');
  uVar6 = ~uVar6;
  pcVar5 = pcVar8 + -uVar6;
  pcVar8 = acStack_2c;
  for (uVar7 = uVar6 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
    *(undefined4 *)pcVar8 = *(undefined4 *)pcVar5;
    pcVar5 = pcVar5 + 4;
    pcVar8 = pcVar8 + 4;
  }
  for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
    *pcVar8 = *pcVar5;
    pcVar5 = pcVar5 + 1;
    pcVar8 = pcVar8 + 1;
  }
LAB_004cf172:
  iVar3 = (**(code **)(*param_1 + 0x1f0))(acStack_2c,0);
  if (iVar3 == 0) {
    ExceptionList = local_c;
    return 0;
  }
  iVar3 = FUN_0057d9d0_FireScriptEvent(param_1,0x26,1,0,0);
  if (iVar3 != 0) {
    FUN_0057dc70_RunQueuedScriptEvent();
  }
  iStack_30 = FUN_00482fb0(100);
  uStack_4 = 0;
  if (iStack_30 == 0) {
    iVar3 = 0;
  }
  else {
    iVar3 = FUN_004daae0_TActionBlock_CopyCtor((undefined4 *)param_1[0x38],acStack_2c,*(undefined4 *)param_1[0x38]);
  }
  uStack_4 = 0xffffffff;
  (**(code **)(*param_1 + 0x1f8))(iVar3);
  if (((int *)param_1[0x36] != (int *)0x0) &&
     (((iVar2 = *(int *)param_1[0x36], iVar2 == 2 || (iVar2 == 4)) || (iVar2 == 0x1a)))) {
    iVar3 = *param_1;
    uVar4 = FUN_004dadd0_StName(&iStack_30,&DAT_005e00f0,0);
    iVar3 = (**(code **)(iVar3 + 0x1f0))(uVar4);
    if (iVar3 == 0) {
      puVar9 = &DAT_005e00f8;
    }
    else {
      puVar9 = &DAT_005e00f4;
    }
    uVar4 = FUN_004dadd0_StName(&iStack_30,puVar9);
    iVar3 = FUN_00482fb0(100);
    puStack_8 = (undefined1 *)0x1;
    if (iVar3 == 0) {
      iVar3 = 0;
    }
    else {
      iVar3 = FUN_004daae0_TActionBlock_CopyCtor((undefined4 *)param_1[0x36],uVar4,*(undefined4 *)param_1[0x36]);
    }
    puStack_8 = (undefined1 *)0xffffffff;
  }
  *(uint *)(iVar3 + 0x60) = *(uint *)(iVar3 + 0x60) | 0x20;
  (**(code **)(*param_1 + 0x208))(iVar3,0);
  ExceptionList = pvStack_18;
  return 1;
}


