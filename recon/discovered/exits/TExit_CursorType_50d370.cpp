// TExit_CursorType @ 0x0050d370 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0xc0
// FUN_0050d370 @ 0050d370 size=45

int __thiscall FUN_0050d370(int *param_1,int param_2)

{
  int iVar1;
  
  iVar1 = (**(code **)(*param_1 + 0x1f0))();
  if ((iVar1 != 0) && ((*(byte *)(param_1 + 2) & 0x80) == 0)) {
    return (-(uint)(param_2 != 0) & 0xfffffffd) + 3;
  }
  return -1;
}


