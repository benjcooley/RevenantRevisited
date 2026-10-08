// TTrapLever_Use @ 0x00524da0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0xbc
// FUN_00524da0 @ 00524da0 size=120

undefined4 __thiscall FUN_00524da0(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_004705f0(param_2,param_3);
  if (iVar1 != 0) {
    return 1;
  }
  if (param_2 != 0) {
    *(int *)(param_1 + 0xe0) = param_2;
    iVar1 = FUN_0049c430(s_lever_005e2fa0);
    if (-1 < iVar1) {
      iVar2 = FUN_0049b650(iVar1);
      if (iVar2 != 0) {
        FUN_0049b990(iVar1,0x7f,1,0,0x50,700);
      }
    }
    *(undefined4 *)(param_1 + 0xd8) = 1;
  }
  return 0;
}


