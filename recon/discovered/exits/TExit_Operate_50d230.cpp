// TExit_Operate @ 0x0050d230 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x250 (`operate` command); earlier recon called it OnUsed
// FUN_0050d230 @ 0050d230 size=114

void __thiscall FUN_0050d230(int param_1,undefined4 param_2)

{
  short sVar1;
  int iVar2;
  
  sVar1 = *(short *)(param_1 + 0xc);
  if ((sVar1 == 1) || (sVar1 == 0)) {
    iVar2 = FUN_0050d2b0(param_2);
    if (iVar2 != 0) {
      FUN_0050d530(2);
      return;
    }
    FUN_0050d530(5);
  }
  else if ((sVar1 == 3) || (sVar1 == 2)) {
    iVar2 = FUN_0050d2b0(param_2);
    if (iVar2 != 0) {
      FUN_0050d530(0);
      return;
    }
    FUN_0050d530(4);
    return;
  }
  return;
}


