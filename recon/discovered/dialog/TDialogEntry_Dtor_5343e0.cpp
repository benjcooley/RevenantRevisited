// FUN_005343e0 @ 005343e0 size=144

void __fastcall FUN_005343e0(int param_1)

{
  int iVar1;
  int *piVar2;
  
  if (DAT_005d7a18 == 0) {
    FUN_004aacb0(*(int *)(param_1 + 0x20) + *(int *)(param_1 + 0x18),
                 *(int *)(param_1 + 0x24) + *(int *)(param_1 + 0x1c),*(undefined4 *)(param_1 + 0x28)
                 ,*(undefined4 *)(param_1 + 0x2c),1);
  }
  if (*(undefined4 **)(param_1 + 0x48) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x48))(1);
  }
  if (*(undefined4 **)(param_1 + 0x4c) != (undefined4 *)0x0) {
    (**(code **)**(undefined4 **)(param_1 + 0x4c))(1);
  }
  iVar1 = 0;
  if (0 < *(int *)(param_1 + 0x5c)) {
    piVar2 = (int *)(param_1 + 0x100);
    do {
      FUN_004830f0(piVar2[-0x28]);
      if (*piVar2 != 0) {
        FUN_004367d0(iVar1);
        *piVar2 = 0;
      }
      iVar1 = iVar1 + 1;
      piVar2 = piVar2 + 1;
    } while (iVar1 < *(int *)(param_1 + 0x5c));
  }
  return;
}


