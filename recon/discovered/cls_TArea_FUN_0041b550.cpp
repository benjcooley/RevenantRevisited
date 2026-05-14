// FUN_0041b550 @ 0041b550 size=307

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0041b550(int param_1)

{
  uint uVar1;
  
  if ((*(uint *)(param_1 + 0x44) & 0x100) != 0) {
    if ((*(uint *)(param_1 + 0x44) & 0x800) != 0) {
      FUN_00450d20(param_1 + 0xf0);
    }
    uVar1 = *(uint *)(param_1 + 0x44);
    if ((uVar1 & 0x400) != 0) {
      _DAT_006671f4 = (float)(*(int *)(param_1 + 0xe4) - *(int *)(param_1 + 0xf0));
      _DAT_006671f8 = (float)(*(int *)(param_1 + 0xe8) - *(int *)(param_1 + 0xf4));
      _DAT_006671fc = (float)(*(int *)(param_1 + 0xec) - *(int *)(param_1 + 0xf8));
      _DAT_00667218 = 0xc7c35000;
    }
    if ((uVar1 & 0x1000) != 0) {
      _DAT_006671f4 = (float)*(int *)(param_1 + 0xe4);
      _DAT_00667218 = 0xc7c35000;
      _DAT_006671f8 = (float)*(int *)(param_1 + 0xe8);
      _DAT_006671fc = (float)*(int *)(param_1 + 0xec);
    }
    if ((uVar1 & 0x2000) != 0) {
      FUN_0045a990(*(undefined4 *)(param_1 + 0xfc),*(undefined4 *)(param_1 + 0x100),
                   *(undefined4 *)(param_1 + 0x104));
    }
    if ((*(uint *)(param_1 + 0x44) & 0x200) != 0) {
      FUN_0045aa80(*(undefined4 *)(param_1 + 0x120));
    }
  }
  return;
}


