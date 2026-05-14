// FUN_004cf2e0_TCharacter_StartSneak @ 004cf2e0 size=423

undefined4 __fastcall FUN_004cf2e0_TCharacter_StartSneak(int *param_1)

{
  int iVar1;
  void *unaff_EDI;
  char *pcVar2;
  void *local_c;
  undefined1 *puStack_8;
  undefined4 uStack_4;
  
  uStack_4 = 0xffffffff;
  puStack_8 = &LAB_0059e823;
  local_c = ExceptionList;
  if ((param_1[0x44] & 0x80000U) == 0) {
    iVar1 = *(int *)(param_1[0x36] + 0x48);
    if ((iVar1 != 0) && ((*(uint *)(iVar1 + 0x24) & 0x2000000) != 0)) {
      return 0;
    }
    iVar1 = *(int *)(param_1[0x36] + 0x4c);
    if ((iVar1 != 0) && ((*(byte *)(iVar1 + 0x24) & 0x80) != 0)) {
      return 0;
    }
  }
  ExceptionList = &local_c;
  iVar1 = (**(code **)(*param_1 + 0x1c0))();
  if (iVar1 < 1) {
    ExceptionList = local_c;
    return 0;
  }
  if ((param_1[0x38] != 0) && (iVar1 = FUN_004dab80_TActionBlock_Is(s_sneak_005c618c), iVar1 != 0)) {
    ExceptionList = local_c;
    return 1;
  }
  iVar1 = FUN_0057d9d0_FireScriptEvent(param_1,0x27,1,0,0);
  if (iVar1 != 0) {
    FUN_0057dc70_RunQueuedScriptEvent();
  }
  iVar1 = FUN_00482fb0(100);
  uStack_4 = 0;
  if (iVar1 == 0) {
    iVar1 = 0;
  }
  else {
    iVar1 = FUN_004daae0_TActionBlock_CopyCtor(param_1[0x38],s_sneak_005e00fc,0);
  }
  uStack_4 = 0xffffffff;
  (**(code **)(*param_1 + 0x1f8))(iVar1);
  if (((int *)param_1[0x36] != (int *)0x0) && (*(int *)param_1[0x36] == 2)) {
    iVar1 = (**(code **)(*param_1 + 0x1f0))(s_sneakr_005e0104,0);
    pcVar2 = s_sneakr_005e010c;
    if (iVar1 == 0) {
      pcVar2 = s_sneakf_005e0114;
    }
    iVar1 = FUN_00482fb0(100);
    puStack_8 = (undefined1 *)0x1;
    if (iVar1 == 0) {
      iVar1 = 0;
    }
    else {
      iVar1 = FUN_004daae0_TActionBlock_CopyCtor(param_1[0x36],pcVar2,2);
    }
    puStack_8 = (undefined1 *)0xffffffff;
  }
  *(uint *)(iVar1 + 0x60) = *(uint *)(iVar1 + 0x60) | 0x20;
  (**(code **)(*param_1 + 0x208))(iVar1,0);
  ExceptionList = unaff_EDI;
  return 1;
}


