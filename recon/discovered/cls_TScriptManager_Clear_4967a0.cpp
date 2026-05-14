// FUN_004967a0_TScriptManager_Clear @ 004967a0 size=180

void __thiscall FUN_004967a0_TScriptManager_Clear(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = 0;
  if (0 < *(int *)(param_1 + 0x8004)) {
    do {
      if ((((*(int *)(param_1 + 0x8014) != 0) && (uVar3 < *(uint *)(param_1 + 0x8004))) &&
          (*(int *)(*(int *)(param_1 + 0x8014) + uVar3 * 4) != 0)) &&
         ((iVar1 = *(int *)(*(int *)(param_1 + 0x8014) + uVar3 * 4),
          *(int *)(iVar1 + 0x38) == param_2 && (iVar1 != 0)))) {
        FUN_00496f20(uVar3,0);
        FUN_004948e0_TScriptProto_Dtor();
        FUN_004830f0(iVar1);
      }
      uVar3 = uVar3 + 1;
    } while ((int)uVar3 < *(int *)(param_1 + 0x8004));
  }
  iVar1 = *(int *)(param_1 + 0x802c);
  while (iVar1 = iVar1 + -1, -1 < iVar1) {
    if (*(int *)(*(int *)(*(int *)(param_1 + 0x803c) + iVar1 * 4) + 0x80) == param_2) {
      iVar2 = *(int *)(*(int *)(param_1 + 0x803c) + iVar1 * 4);
      if (iVar2 != 0) {
        FUN_004830f0(iVar2);
      }
      FUN_0041cb80(iVar1);
    }
  }
  return;
}


