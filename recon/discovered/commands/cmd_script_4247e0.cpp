// FUN_004247e0 @ 004247e0 size=449

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_004247e0(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  iVar1 = FUN_00479700(&DAT_005cbc3c,0);
  if ((iVar1 != 0) && (param_1 != 0)) {
    FUN_00479580();
    if (*(int *)(param_1 + 0x84) == 0) {
      uVar2 = FUN_00497120(param_1,*(undefined4 *)(param_1 + 0x38),0,s_master_s_005cbc44);
      FUN_00471150(uVar2);
    }
    DAT_00656d30 = 1;
    _DAT_00656d34 = 1;
    _DAT_00656e08 = 0;
    _DAT_00656e0c = 0;
    (**(code **)(DAT_00656dc0 + 0x28))();
    uVar3 = 7;
    uVar2 = FUN_0048ed60(&DAT_00656dc0);
    FUN_0048eea0(uVar2,uVar3);
    FUN_0043f8b0(param_1);
    return 0;
  }
  iVar1 = FUN_00479700(s_pause_005cbc50,0);
  if (iVar1 == 0) {
    iVar1 = FUN_00479700(s_resume_005cbc5c,0);
    if (iVar1 == 0) {
      iVar1 = FUN_00479700(&PTR_DAT_005cbc68,0);
      if (iVar1 != 0) {
        FUN_00479580();
        iVar1 = FUN_00479700(&DAT_005cbc6c,0);
        if (iVar1 != 0) {
          FUN_00479580();
          FUN_004942c0();
          return 0;
        }
        if ((param_1 != 0) && (*(int *)(param_1 + 0x84) != 0)) {
          FUN_00492490();
        }
      }
    }
    else {
      FUN_00479580();
      iVar1 = FUN_00479700(&DAT_005cbc64,0);
      if (iVar1 != 0) {
        FUN_00479580();
        DAT_0066856c = 0;
        return 0;
      }
      if ((param_1 != 0) && (*(int *)(param_1 + 0x84) != 0)) {
        FUN_004942b0();
        return 0;
      }
    }
  }
  else {
    FUN_00479580();
    iVar1 = FUN_00479700(&DAT_005cbc58,0);
    if (iVar1 != 0) {
      FUN_00479580();
      DAT_0066856c = 1;
      return 0;
    }
    if ((param_1 != 0) && (*(int *)(param_1 + 0x84) != 0)) {
      FUN_004942a0();
      return 0;
    }
  }
  return 0;
}


