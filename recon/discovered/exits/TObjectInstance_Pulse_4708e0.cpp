// TObjectInstance_Pulse @ 0x004708e0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// animator pulse + script Continue
// FUN_004708e0 @ 004708e0 size=58

void __fastcall FUN_004708e0(int *param_1)

{
  undefined4 uVar1;
  
  if ((int *)param_1[0x16] != (int *)0x0) {
    (**(code **)(*(int *)param_1[0x16] + 0x2c))();
  }
  if ((param_1[0x21] != 0) && ((param_1[2] & 0x200000U) == 0)) {
    uVar1 = (**(code **)(*param_1 + 0x154))();
    FUN_004933d0(uVar1);
  }
  return;
}


