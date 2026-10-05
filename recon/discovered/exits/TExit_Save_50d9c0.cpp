// TExit_Save @ 0x0050d9c0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x164
// FUN_0050d9c0 @ 0050d9c0 size=63

void __thiscall FUN_0050d9c0(int param_1,int param_2)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
  FUN_004dd470(param_2);
  uVar1 = *(undefined4 *)(param_1 + 0xe4);
  if ((*(int *)(param_2 + 0xc) + *(int *)(param_2 + 4)) - *(int *)(param_2 + 8) < 4) {
    FUN_0049cc70(4);
  }
  puVar2 = *(undefined4 **)(param_2 + 8);
  *puVar2 = uVar1;
  *(undefined4 **)(param_2 + 8) = puVar2 + 1;
  return;
}


