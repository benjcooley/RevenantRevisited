// FUN_0041bd10_TArea_Exit @ 0041bd10 size=182

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0041bd10_TArea_Exit(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  if ((*(uint *)(param_1 + 0x44) & 2) != 0) {
    *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) & 0xfffffffd;
    if (*(int *)(param_1 + 0x25c) != 0) {
      FUN_004defe0();
      iVar1 = *(int *)(param_1 + 0x25c);
      *(undefined4 *)(iVar1 + 0x1c) = 0;
      *(undefined4 *)(iVar1 + 0x20) = 0;
    }
    if ((*(uint *)(param_1 + 0x44) & 0x20) != 0) {
      if (DAT_0065abd8 == 0) {
        DAT_0065abd8 = -8;
      }
      else {
        _DAT_0065abdc = 0xfffffff8;
      }
    }
    if ((*(uint *)(param_1 + 0x44) & 0x40) != 0) {
      uVar2 = FUN_0049c430(param_1 + 0x138);
      FUN_0049bd90(uVar2);
      uVar3 = 0;
      uVar2 = FUN_0049c430(param_1 + 0x138);
      FUN_0049b8e0(uVar2,uVar3);
    }
    if ((DAT_0066829c == 0) && ((*(byte *)(param_1 + 0x44) & 0x10) != 0)) {
      FUN_004967a0_TScriptManager_Clear(param_1);
    }
  }
  return;
}


