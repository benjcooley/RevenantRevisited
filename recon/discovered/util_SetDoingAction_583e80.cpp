// FUN_00583e80_LogActionEvent @ 00583e80 size=97

undefined4 __thiscall
FUN_00583e80_LogActionEvent(int param_1,undefined4 param_2,undefined4 param_3,undefined1 param_4,undefined4 param_5
            ,undefined4 param_6)

{
  int iVar1;
  undefined1 *puVar2;
  
  if (DAT_00676828 != 0) {
    iVar1 = FUN_0057d9d0_FireScriptEvent(param_2,param_3,param_5,param_6,0);
    if (iVar1 != 0) {
      iVar1 = *(int *)(param_1 + 0x60);
      puVar2 = *(undefined1 **)(iVar1 + 0xc);
      *(undefined1 **)(iVar1 + 0xc) = puVar2 + 1;
      if (*(undefined1 **)(iVar1 + 4) < puVar2 + 1) {
        puVar2 = (undefined1 *)FUN_005884a0(puVar2);
      }
      *puVar2 = param_4;
      FUN_0057dc70_RunQueuedScriptEvent();
      return 1;
    }
  }
  return 0;
}


