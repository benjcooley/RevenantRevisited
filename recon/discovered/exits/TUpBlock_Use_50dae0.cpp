// TUpBlock_Use @ 0x0050dae0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0xbc
// FUN_0050dae0 @ 0050dae0 size=162

undefined4 __thiscall FUN_0050dae0(int *param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = FUN_004705f0(param_2,param_3);
  if (iVar2 != 0) {
    return 1;
  }
  if (param_3 == -1) {
    sVar1 = (short)param_1[3];
    if ((sVar1 == 2) || (sVar1 == 3)) {
      (**(code **)(*param_1 + 0x18))(0);
    }
    else if ((sVar1 == 0) || (sVar1 == 1)) {
      (**(code **)(*param_1 + 0x18))(2);
    }
    iVar2 = FUN_0049c430(s_grind_rock_005e1890);
    if (-1 < iVar2) {
      iVar3 = FUN_0049b650(iVar2);
      if (iVar3 != 0) {
        FUN_0049b990(iVar2,0x7f,1,0,0x50,700);
      }
    }
    return 1;
  }
  return 0;
}


