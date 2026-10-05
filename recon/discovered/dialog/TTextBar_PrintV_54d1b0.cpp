// FUN_0054d1b0 @ 0054d1b0 size=305

void __thiscall FUN_0054d1b0(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined1 *puVar2;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar3;
  char *pcVar4;
  undefined1 local_1c0 [64];
  undefined1 local_180 [128];
  char local_100 [256];
  
  if (*(int *)(param_1 + 0x40) != 0) {
    if (DAT_00668178 != 0) {
      FUN_0058b100(local_180,s__sTextDump_txt_005e582c,&DAT_006666cc);
      iVar1 = FUN_0058b5db(local_180,&DAT_005e583c);
      if (iVar1 != 0) {
        if (DAT_005e5808 != 0) {
          DAT_005e5808 = 0;
          FUN_0058ec43(local_1c0);
          FUN_0058b56e(iVar1,s_Revenant_Text_Dump_executed_at___005e5840,local_1c0);
        }
        FUN_0058ec08(iVar1,param_3,param_4);
        FUN_0058b56e(iVar1,&DAT_005e5864);
        FUN_0058b4f1(iVar1);
      }
    }
    FUN_0058bced(local_100,param_3,param_4);
    pcVar4 = local_100;
    puVar2 = (undefined1 *)FUN_0058ade0(local_100,10);
    uVar3 = extraout_ECX;
    while (puVar2 != (undefined1 *)0x0) {
      *puVar2 = 0;
      FUN_00444e20(0);
      FUN_0054d0c0(param_2,uVar3,pcVar4);
      pcVar4 = puVar2 + 1;
      puVar2 = (undefined1 *)FUN_0058ade0(pcVar4,10);
      uVar3 = extraout_ECX_00;
    }
    if (*pcVar4 != '\0') {
      pcVar4 = local_100;
      FUN_00444e20(0);
      FUN_0054d0c0(param_2,uVar3,pcVar4);
    }
  }
  return;
}


