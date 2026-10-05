// TUpBlock_Pulse @ 0x0050da80 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// vtable slot 0x110
// FUN_0050da80 @ 0050da80 size=87

void __fastcall FUN_0050da80(int *param_1)

{
  short sVar1;
  int iVar2;
  
  if (DAT_00668154 == 0) {
    iVar2 = (**(code **)(*param_1 + 0x154))();
    if (iVar2 != 0) {
      sVar1 = (short)param_1[3];
      if ((sVar1 == 2) || (sVar1 == 5)) {
        (**(code **)(*param_1 + 0x18))(3);
      }
      else if ((sVar1 == 0) || (sVar1 == 4)) {
        (**(code **)(*param_1 + 0x18))(1);
        FUN_0050d640();
        return;
      }
    }
  }
  FUN_0050d640();
  return;
}


