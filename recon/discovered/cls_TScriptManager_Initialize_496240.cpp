// FUN_00496240_TScriptManager_Initialize @ 00496240 size=226

undefined4 __fastcall FUN_00496240_TScriptManager_Initialize(int param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  
  if (*(undefined4 **)(param_1 + 0x8014) != (undefined4 *)0x0) {
    puVar3 = *(undefined4 **)(param_1 + 0x8014);
    for (uVar1 = *(uint *)(param_1 + 0x800c) & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined1 *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
  }
  *(undefined4 *)(param_1 + 0x8008) = 0;
  *(undefined4 *)(param_1 + 0x8004) = 0;
  if (*(undefined4 **)(param_1 + 0x8028) != (undefined4 *)0x0) {
    puVar3 = *(undefined4 **)(param_1 + 0x8028);
    for (uVar1 = *(uint *)(param_1 + 0x8020) & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined1 *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
  }
  *(undefined4 *)(param_1 + 0x801c) = 0;
  *(undefined4 *)(param_1 + 0x8018) = 0;
  if (*(undefined4 **)(param_1 + 0x803c) != (undefined4 *)0x0) {
    puVar3 = *(undefined4 **)(param_1 + 0x803c);
    for (uVar1 = *(uint *)(param_1 + 0x8034) & 0x3fffffff; uVar1 != 0; uVar1 = uVar1 - 1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined1 *)puVar3 = 0;
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
  }
  *(undefined4 *)(param_1 + 0x8030) = 0;
  *(undefined4 *)(param_1 + 0x802c) = 0;
  iVar2 = FUN_00496490_TScriptManager_Load(s_master_s_005da4dc,param_1);
  if (iVar2 == 0) {
    FUN_00481c10(s_Unable_to_load_MASTER_S_file_005da4e8,0);
  }
  if (DAT_0066829c == 0) {
    iVar2 = FUN_00495cf0_TGameState_Load(s_state_def_005da508);
    if (iVar2 == 0) {
      FUN_00481c10(s_Unable_to_load_STATE_DEF_file_005da514,0);
    }
  }
  return 1;
}


