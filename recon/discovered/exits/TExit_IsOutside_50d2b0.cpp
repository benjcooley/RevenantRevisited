// TExit_IsOutside @ 0x0050d2b0 -- retail Revenant.exe, raw Ghidra decompile (see docs/gameflow/forensics/EXITS.md)
// non-virtual; also the `isoutside` script member (caller 0x0041f51b)
// FUN_0050d2b0 @ 0050d2b0 size=184

undefined4 __thiscall FUN_0050d2b0(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iStack_24;
  int iStack_20;
  int iStack_1c;
  undefined1 auStack_18 [12];
  undefined1 local_c [12];
  
  if (param_2 != 0) {
    iVar1 = (**(code **)(*param_1 + 0x254))(10,local_c,0);
    FUN_0046db20(iVar1 + (uint)*(byte *)((int)param_1 + 0x36));
    iVar1 = (**(code **)(*param_1 + 0x254))(10,auStack_18,0);
    FUN_0046db20(iVar1 + 0x7f + (uint)*(byte *)((int)param_1 + 0x36));
    iStack_24 = *(int *)(param_2 + 0x10) - param_1[4];
    iStack_20 = *(int *)(param_2 + 0x14) - param_1[5];
    iStack_1c = *(int *)(param_2 + 0x18) - param_1[6];
    iVar1 = FUN_0046de60(auStack_18,&iStack_24);
    iVar2 = FUN_0046de60(local_c,&iStack_24);
    if (iVar1 < iVar2) {
      return 1;
    }
  }
  return 0;
}


