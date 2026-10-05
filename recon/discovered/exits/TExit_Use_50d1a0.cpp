// TExit_Use @ 0x0050d1a0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0xbc
// FUN_0050d1a0 @ 0050d1a0 size=130

undefined4 __thiscall FUN_0050d1a0(int *param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (**(code **)(*param_1 + 0x1f0))();
  if (iVar1 == 0) {
    return 0;
  }
  uVar2 = FUN_00452690(param_3,0);
  iVar1 = FUN_004dd480(param_2,uVar2);
  if ((iVar1 == 0) && (iVar1 = (**(code **)(*param_1 + 0x208))(), iVar1 != 0)) {
    if (param_2 != DAT_00667fcc) {
      return 0;
    }
    uVar2 = FUN_0049d800(s_DOORLOCKED_005e17cc);
    FUN_0054d170(&DAT_0065c5d0,uVar2);
    return 0;
  }
  FUN_004705f0(param_2,param_3);
  return 1;
}


