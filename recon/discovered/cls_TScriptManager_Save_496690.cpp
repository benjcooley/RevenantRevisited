// FUN_00496690_TScriptManager_Save @ 00496690 size=271

undefined4 __thiscall FUN_00496690_TScriptManager_Save(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  undefined1 local_104 [260];
  
  if (*(int *)(param_1 + 0x8040) == 0) {
    return 1;
  }
  FUN_0058b100(local_104,&DAT_005da588,&DAT_0065bd48,param_2);
  iVar1 = FUN_004457c0(local_104,&DAT_005da590);
  if (iVar1 == 0) {
    return 0;
  }
  FUN_0058b56e(iVar1,s___________________Revenant_Scrip_005da594);
  uVar4 = 0;
  uVar3 = 1;
  if (0 < *(int *)(param_1 + 0x8004)) {
    do {
      if (((*(int *)(param_1 + 0x8014) != 0) && (uVar4 < *(uint *)(param_1 + 0x8004))) &&
         (*(int *)(*(int *)(param_1 + 0x8014) + uVar4 * 4) != 0)) {
        iVar2 = *(int *)(*(int *)(param_1 + 0x8014) + uVar4 * 4);
        if (*(int *)(iVar2 + 0x38) == param_3) {
          iVar2 = FUN_0058e6f7(*(undefined4 *)(iVar2 + 4),iVar1);
          if (iVar2 != -1) {
            iVar2 = FUN_0058e6f7(&DAT_005da44c,iVar1);
            if (iVar2 != -1) goto LAB_00496772;
          }
          uVar3 = 0;
        }
      }
LAB_00496772:
      uVar4 = uVar4 + 1;
    } while ((int)uVar4 < *(int *)(param_1 + 0x8004));
  }
  FUN_0058b4f1(iVar1);
  *(undefined4 *)(param_1 + 0x8040) = 0;
  return uVar3;
}


